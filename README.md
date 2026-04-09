# Informe de Optimización y Análisis de Arquitectura (Ray Tracer CUDA)

A continuación se detalla un análisis profundo del estado actual del proyecto, centrado en el rendimiento, optimizaciones matemáticas evitables, cuellos de botella específicos de GPU y decisiones de diseño arquitectónico.

## 1. Arquitectura de Memoria y Diseño de Datos (Data-Oriented Design)

### ❌ El problema: Array of Structures (AoS)
Actualmente, los datos de la escena (Geometría, Materiales y parámetros dentro del BVH) tienen una tendencia hacia arquitecturas **AoS** (Array of Structures). 
Por ejemplo, los nodos de datos de geometría mezclan transformaciones matriciales (16 floats continuos por array), datos del bounding box y metadatos. Cuando un *warp* de 32 hilos atraviesa el árbol BVH y carga estos datos en la GPU, las lecturas de memoria se dirigen a posiciones dispersas. Esto destruye la coalescencia de la memoria (*memory coalescing*), forzando a cargar múltiples líneas de caché para poder atender a todos los hilos, bloqueando el ancho de banda.

### ✅ La solución: Structure of Arrays (SoA)
Debes transformar este diseño hacia **SoA** (Structure of Arrays). En lugar de tener una lista `Geometria[N]` con diferentes tipos de variables por elemento, deberías usar un solo _struct_ con arrays individuales. De igual forma para Materiales.
De esta manera, cuando el warp #1 acceda a `matrices_X[i]`, todos los hilos consecutivos leerán fragmentos de memoria contiguos llenando a la perfección transacciones únicas de 128-bytes.

### ❌ Escalabilidad Lineal $O(N^2)$ en Inicialización
Funciones de reserva de escena como `makeSphere()` o añadir materiales duplican el array y copian todos los datos anteriores (crecimiento $O(N^2)$).
**Mejora:** Implementar un sistema de arrays dinámicos (estilo `std::vector`) con crecimiento exponencial (capacidad \*= 2 al desbordar) o pre-reserva de memoria a gran escala. Esto eliminará los micro-bloqueos en la configuración (setup) de la escena.

---

## 2. Cálculos Redundantes y Optimizaciones Matemáticas

### Normalización de Rayos Repetida
El método asociado a los rayos suele llamar a `.normalized()` (`ray.getDirection().normalized()`) constantemente, hasta en 4 ocasiones distintas por cada rebote de luz en los tests de interesección y cálculos de material (`Material.cu`). 
Normalizar vectores usa raíces cuadradas (`sqrt()`), matemáticas que son altamente costosas (~40 ciclos GPU por llamada) si no eran necesarias o cambian raramente.
**Mejora:** Transforma o pre-calcula los rayos y almacena (`cachea`) su vector normalizado al generarlo, leyendo posteriormente ese valor precalculado.

### Transposición e Inversas de Matrices Constantes
Durante la fase principal del ray tracer, los tests matemáticos de intersección (cilindros, esferas, conos) computan frecuentemente transposiciones y operaciones complejas usando la matriz modelo.
**Mejora:** Pre-calcula la matriz **Inversa-Transpuesta** en la fase de creación (CPU o una vez en Setup de CUDA) y almacénala junto a los datos del objeto o `Transform`.
Incluso durante el setup, estás usando `Matrix4x4.inverse()` de propósito general. Tratándose de matrices TRS (Traslación, Rotación, Escala), multiplicar directamente `inverseScale * inverseRotation * inverseTranslation` acelera inmensamente la carga de la escena en general y evita imprecisiones de flotante.

### Cálculos Pesados de Solvers Geométricos
Geometrías complejas como el `Torus` emplean el *Ferrari solver* a 80 iteraciones con trigonometría extensa (`acosf()`, `cbrtf()`). Asegúrate de rodearlo en el BVH con mallas o AABBs sumamente ajustados para minimizar cuántas veces se ejecuta este cálculo en *misses* del rayo.

---

## 3. Optimizaciones Específicas de Hardware (CUDA)

### Divergencia Experimental de Warps (Warp Divergence)
Dentro de las intersecciones por Rayo, tu código bifurca severamente. Existe un `switch(geomData.type)` enorme (ej. Esfera, Cilindro, Toro). 
En la arquitectura SIMT de CUDA, si en 32 hilos (1 warp) algunos tocan `Esfera`, otros `Cilindro` y otros `Torus`, la GPU no puede ejecutar los tres caminos al mismo tiempo. Debe serializar (procesar las esferas, congelar el resto; luego los cilindros... etc). Sumar un camino de 50 ciclos + uno de 200 ciclos ahoga el rendimiento.
**Mejora fundamental:** Usa "Ray Sorting" (radix sort). Clasificar/Ordenar o agrupar los rayos según el tipo de geometría con la que o probabilidad que van a interactuar. O mejor aún, dividir los kernels: lanzar un kernel sólo para esferas con `is_active=1` y luego sólo para toros, etc.

### Generador de Números Aleatorios (RNG) y Memoria
Revisar tu estado interno de `Random.cuh`. Generadores complejos con estado mutante imponen altos cuellos de botella. Es preferible un PRNG funcional y sin estado dependiente ("stateless PRNG" como *PCG*, *Sobol* o un *Wang Hash*) indexado meramente a través de `(threadIdx, iteracion)`.

### Presión de Ancho de Banda y G-Buffers
Dispones de buffers masivamente pesados (`albedo`, `normales`, `colorBuffer`, `pingPongBuffer`), los cuales sumados llegan a gigabytes de VRAM y estrangulan el bus global en operaciones de lecto/escritura (Camera). Comprimir datos (usar formatos de 16-bits para Normales y tipos _half_ / _fp16_ si usas _Tensor Cores_) puede liberar más del 30% del gasto de memoria. Por último, comprueba si no son necesarios los bordes y márgenes de los filtros (donde `neighborX < 0`), los cuales causan *divergencia de filtro*.

---

## 📋 Resumen de Prioridad de Acción

| Objetivo de Refactorización | Tipo | Estimación de Prioridad |
| :--- | :--- | :--- |
| **Cambio a arquitectura SoA (Mem. Coalescing)** | Arquitectura y Memoria | **CRÍTICA 🔥** (Estimado: 15-25% más rápido) |
| **Mitigar Divergencia en Intersecciones (Switch)** | Hardware CUDA | **CRÍTICA 🔥** (Agrupar rayos salva ciclos) |
| **Cachear Normalizaciones de Direcciones** | Cálculos Matemáticos | **ALTA 🔴** (Prevenir `sqrt()` a cada paso) |
| **Pre-Computar Matrices Transpuestas/Inversas** | Cálculos Matemáticos | **ALTA 🔴** (Sólo cuesta memoria, gana tiempo) |
| **Eliminar micro-copias y reservas O(N^2)** | Diseño / CPU | MEDIA 🟡 (Carga de escena más fluida) |
| **Refactorizar RNG a modelo *Stateless*** | Hardware CUDA | MEDIA 🟡 |

*Las secciones marcadas con **Crítica** son decisiones de diseño perjudiciales actualmente, mientras que las catalogadas como **Alta** representan redundancia de cómputo matemática y son más fáciles de solucionar en días aislados.*