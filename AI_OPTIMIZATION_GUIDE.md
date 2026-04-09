# Guía de Optimización del Ray Tracer en CUDA para Agentes de IA

Este documento está diseñado específicamente como una **hoja de ruta detallada para un agente de IA** (como GitHub Copilot o subagentes) que deba implementar las optimizaciones de rendimiento y refactorizaciones arquitectónicas en este proyecto de Ray Tracer en CUDA.

El proyecto sufre actualmente de cuellos de botella clásicos en programación de GPUs: falta de memory coalescing debido a estructuras AoS, divergencia de warps masiva, cálculos matemáticos redundantes en el loop interno y una inicialización ineficiente.

A continuación, se detalla el plan de acción, los archivos exactos a modificar y la estrategia de refactorización para cada punto.

---

## 🛑 PRIORIDAD 1: Refactorización Arquitectónica (Memoria y Data-Oriented Design)

### 1.1 Migración de AoS a SoA en Geometría y Materiales
**Contexto**: El acceso a los datos de la escena desde los *warps* de CUDA está destrozando la caché.
- **Archivos a modificar**: `include/geometry/Geometry.cuh`, `src/geometry/Geometry.cu`, `src/geometry/LinearBVH.cu`.
- **Estado Actual (AoS)**: La geometría se almacena en `SimpleGeometryData` con punteros a arrays de objetos enteros (`Matrix4x4 *inverseTransform`, `AABB *boundingBox`, `int *geomIdx`). Los hilos en un warp acceden a diferentes índices, leyendo estructuras masivas (como `Matrix4x4` de 16 floats continuos) provocando lecturas de memoria no coalescentes. Lo mismo ocurre con `data->materials[record.geomIdx]`.
- **Acción requerida por la IA**:
  1. Reescribir `SimpleGeometryData` (y equivalentes de materiales) para usar el patrón **Structure of Arrays (SoA)**.
  2. Implementar `struct SoAGeometry` que contenga: `float *transform[16];`, `float *aabbMin[3];`, `float *aabbMax[3];`, `int *geomIdx;`.
  3. Modificar la inicialización en `Geometry.cu` para alojar (malloc) los arrays individuales.
  4. Refactorizar las lecturas dentro del recorrido del BVH en `LinearBVH.cu` para extraer los componentes indexando cada array (ej: `transform0[idx]`, `transform1[idx]`, etc.) reconstruyendo la matriz/AABB localmente en registros rápidos.

---

## 🛑 PRIORIDAD 2: Mitigación de Divergencia de Warps (CUDA-Specific)

### 2.1 Refactorización del Bloque Switch en `LinearBVH.cu`
**Contexto**: GPU SIMT Architecture no se lleva bien con `switch(type)`.
- **Archivos a modificar**: `src/geometry/LinearBVH.cu` (aprox. líneas 119-140).
- **Estado Actual**: Durante la intersección, existe un enorme `switch(geomData.type)` con casos como `SPHERE`, `CYLINDER` o `TORUS`. El toroide usa un *Ferrari Solver* (~250 ciclos), mientras que la esfera usa ~50 ciclos. Esto serializa brutalmente la ejecución del warp entero.
- **Acción requerida por la IA**: 
  1. (Opción A - Avanzada): Implementar *Ray Sorting*. Agrupar todos los rayos del warp/bloque por el tipo de geometría con el que intersecarán antes de invocar las funciones complejas.
  2. (Opción B - Intermedia): Transformar la carga de trabajo en múltiples pasadas de kernels. Un kernel sólo prueba contra Esferas y escribe un buffer de intersección temporal, otro para Toroides, etc. (Wavefront shading).

### 2.2 Reescritura del Random Number Generator (RNG)
**Contexto**: Serialización de memoria en estado mutante.
- **Archivos a modificar**: `include/maths/Random.cuh`, `src/maths/Random.cu`.
- **Estado Actual**: Generación de números pseudoaleatorios con estado mutable continuo provoca dependencias de lectura/escritura (Memory Serialization).
- **Acción requerida por la IA**: Sustituir la lógica actual por un PRNG *Stateless* (Sin Estado). Usa un hash rápido (ej. *Wang Hash* o similar a *PCG*) basando la semilla (seed) pura y exclusivamente en el estado constante `(threadID, número de rebote actual, número de pixel)`.

---

## 🟡 PRIORIDAD 3: Optimización Matemática y Matemáticas Redundantes

### 3.1 Eliminación de la Normalización de Vectores Redundante
**Contexto**: Calcular raíz cuadrada repetidamente cuesta ~40 ciclos extra en la GPU.
- **Archivos a modificar**: `src/render/Material.cu`, entre otros donde opere el vector de Rayo.
- **Estado Actual**: Se invoca a `ray.getDirection().normalized()` al menos 4 veces durante cada cálculo de bounce o evaluación del BSDF en los materiales.
- **Acción requerida por la IA**: Almacenar el vector direccional normalizado en el instante en que el rayo "nace" (o rebota) dentro de su `struct Ray` en forma de parámetro caché, y usar un getter `getNormalizedDirection()` que simplemente devuelva ese valor pre-calculado sin volver a lanzar el cómputo.

### 3.2 Pre-Cálculo de Inversas y Transpuestas (Transformaciones TRS)
**Contexto**: Cómputo matricial pesado en tiempo de render.
- **Archivos a modificar**: `src/geometry/Sphere.cu`, `Cone.cu`, `Cylinder.cu`, `src/maths/Matrix4x4.cu`.
- **Estado Actual**: 
  - Dentro de las funciones de intersección (`intersect` / `hit`), se computa dinámicamente o se accede a inversas transpuestas (`transpose()`).
  - La inversión de matriz `Matrix4x4` en CPU/Setup usa la inversa analítica de 4x4.
- **Acción requerida por la IA**:
  1. Computar offline/en setup (CPU o inicio CUDA) la `Matrix_Inverse_Transpose` de cada geometría y subirla a la GPU como un array constante (dentro del nuevo SoA). Usarla directamente.
  2. Modificar el código en `Matrix4x4.cu` / setup para que no use la inversa global, si no la inversa específica de **T**ranslation x **R**otation x **S**cale (`inverseScale * inverseRotation * inverseTranslation`).

---

## 🟢 PRIORIDAD 4: Diseño General de Software y Limpieza (CPU/Host Setup)

### 4.1 Corregir el Factor de Escalabilidad $O(N^2)$ en Inicialización
**Contexto**: Alocar nuevas primitivas es insostenible con escenas muy masivas (ej. millones de esferas).
- **Archivos a modificar**: Funciones tipo `addMaterial()` masivamente usadas (ej. en `Material.cu`, `Sphere.cu` y constructores en host).
- **Estado Actual**: A nivel de Host/Setup, por cada vez que se llama a instanciar una geometría/material, el motor re-asigna (`malloc` / `new`), copia toda la memoria existente al nuevo array completo (`memcpy`), añade el elemento y borra el viejo.
- **Acción requerida por la IA**: Refactorizar estas estructuras tipo array nativo a clases que manejen capacidad dinámica con *Crecimiento Exponencial* (capacidad doblada mediante `capacity *= 2` y un `count`).

### 4.2 Limpieza de Código Muerto
- **Archivos a modificar**: `include/maths/Vector3.cuh`.
- **Acción requerida por la IA**: Purga completa de los grandes bloques de código/constantes comentados (ej. `Vector3.cuh` líneas ~28 en adelante) y limpieza de macros inútilmente definidos observados tras el refactor SoA.

---

### 🔨 INSTRUCCIONES ESTRICTAS PARA EL AGENTE
1. Modifica **un módulo a la vez**, compilando con `make` (ya que el `Makefile` parece estar en uso) para comprobar integridad.
2. Inicia sistemáticamente por el cambio SoA vs AoS (Prioridad 1) antes de optimizar los métodos en la sección de matemáticas (Prioridad 3), dado que los punteros matriciales se verán modificados en ese paso.
3. Al modificar `Random.cuh`, asegúrate de mantener el determinismo entre renders si es que se contaba con una constante tipo semilla global.
4. Aplica comentarios concisos al inicio de las variables alteradas con motivo SoA/AoS (ej. `// SoA format para memory coalescing`).