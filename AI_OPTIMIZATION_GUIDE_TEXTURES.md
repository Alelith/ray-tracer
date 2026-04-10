# Guía de Implementación de Texturas para el Ray Tracer en CUDA

Este documento detalla los pasos necesarios para implementar soporte de texturas en los diferentes canales de los materiales (color/albedo, roughness, metallic, transmission, ior, emission y emissionStrength). Está diseñado para que sirva de guía paso a paso junto con un modelo de IA (como Gemini).

## 1. Visión General de la Arquitectura Necesaria

Actualmente, el motor de render trabaja con valores uniformes (escalares o colores sólidos) para cada canal del `Material`. Para soportar texturas necesitamos:

1. **Coordenadas UV:** Calcular las coordenadas de mapeo en cada intersección geométrica y almacenarlas.
2. **Objeto de Textura (GPU):** Cargar imágenes desde el disco (usando `stb_image.h`), copiarlas a la memoria de la GPU y utilizar preferiblemente **CUDA Texture Objects (`cudaTextureObject_t`)** para aprovechar la interpolación por hardware.
3. **Gestor de Texturas:** Un sistema que almacene y referencie las texturas para evitar duplicar las mismas imágenes en la VRAM.
4. **Actualización de Materiales:** Modificar las estructuras de material para que acepten tanto valores base como texturas, y combinar ambos durante la evaluación del rayo (`calculateBounce`).

---

## 2. Modificaciones Requeridas por Archivo

### A. `include/geometry/HitRecord.cuh`
El registro de intersección (`HitRecord`) necesita almacenar las coordenadas UV de la superficie impactada para saber qué píxel de la textura leer.
* **Cometido:** Almacenar `float u, v;`.
* **Modificación:** 
  Añadir las variables `float u; float v;` y actualizar los constructores para inicializarlas.

### B. Archivos de Geometría (`src/geometry/Sphere.cu`, `Cylinder.cu`, `Cone.cu`, `Torus.cu`)
Las primitivas matemáticas deben calcular las coordenadas UV exactas basándose en el punto de impacto.
* **Cometido:** Proyectar el punto 3D sobre un espacio 2D (0.0 a 1.0) usando proyecciones polares, cilíndricas o esféricas.
* **Modificación (Ejemplo Esfera):**
  Usar `atan2` y `asin` con la normal para calcular `u` (longitud) y `v` (latitud) y pasarlos al `HitRecord` en el momento de registrar el hit (`hit.u = ..., hit.v = ...`).

### C. Nuevo subsistema de Texturas (`include/render/Texture.cuh` y `src/render/Texture.cu`)
Necesitas crear una nueva abstracción para manejar imágenes en la memoria de la tarjeta gráfica.
* **Cometido:** Cargar imágenes, enviarlas al device (GPU) y proporcionar un acceso eficiente.
* **Estructura recomendada:** 
  Crear una clase o struct que contenga un `cudaTextureObject_t`. La carga desde disco en el Host (CPU) usará `stb_image.h`. Una vez cargada, se crea un `cudaArray` y se parametriza el objeto de textura para que use coordenadas normalizadas (0 a 1) e interpolación bilineal.

### D. `include/render/Material.cuh` y `src/render/Material.cu`
El sistema de materiales es el consumidor final de las texturas.
* **Cometido:** Sustituir los atributos estáticos actuales (o complementarlos) por identificadores/punteros a texturas.
* **Modificación en Estructura:**
  En el struct `Material`, para cada canal añadirás algo como `cudaTextureObject_t texAlbedo = 0;`, `cudaTextureObject_t texRoughness = 0;`, etc. (El valor 0 indicaría que no hay textura).
* **Modificación en Lógica (`calculateBounce`):**
  Dentro del device, antes de calcular el BRDF o la refracción, hay que muestrear (samplear) las texturas usando las coordenadas UV del `HitRecord` (ej. `tex2D<float4>(mat.texAlbedo, record.u, record.v)`). Este valor muestreado se multiplica/suma al valor base del material.

### E. `src/main.cu`
El punto de entrada debe orquestar toda la carga.
* **Cometido:** Cargar los archivos de imagen mediante el gestor de texturas y asignar estas texturas a los materiales creados.
* **Modificación:** 
  Incluir llamadas a la nueva API de texturas, cargar imágenes de prueba y modificar los llamados a `addMaterial(...)` para pasar las referencias de las texturas cargadas.

---

## 3. Consideraciones Técnicas para la IA (Gemini)

Cuando pidas a Gemini que genere el código, indícale explícitamente estas directrices:

1. **Uso de `cudaTextureObject_t`:** Solicita que las texturas en GPU se implementen usando el API de Texturas por Objetos de CUDA (`cudaCreateTextureObject`). Esto es inmensamente más rápido y proporciona interpolado (*filtering*) gratis y manejo de bordes (*wrap modes*) por hardware en lugar de arrays crudos (`float*`).
2. **Fallback / Valores por defecto:** Si un canal de material no tiene textura de entrada (un objeto nulo o `ID == 0`), el renderer debe utilizar directamente el valor o color estático (`albedo`, `roughness`, etc.). Si hay textura, se debe evaluar y, preferiblemente, usar como un multiplicador del valor base (ej. `final_color = base_color * tex2D(...)`).
3. **Formatos de datos:**
   * Texturas de **Color/Albedo/Emisión**: Se deben mapear como `float4` (o vectores tipo uchar4 normalizados dinámicamente) asumiendo espacio sRGB.
   * Texturas **Roughness/Metallic/Transmission/IOR**: Sólo se necesita el valor del canal Rojo (monocromo / r8).
4. **`stb_image` integration:** Solicita incluir `stb_image.h` (probablemente el proyecto ya cuente con `stb_image_write.h`, habría que descargar/utilizar el encabezado de lectura equivalente).

## 4. Orden de Implementación Sugerido para Gemini

Puedes pasarle las siguientes peticiones a tu IA en este orden:

1. *"Añade soporte de coordenadas UV (`u, v`) a `HitRecord.cuh` y actualiza la intersección de `Sphere.cu` para calcular y almacenar el U y V basándose en la normal."*
2. *"Escribe una clase/struct `Texture` que implemente la carga de un archivo JPEG/PNG con `stb_image.h` y cree un `cudaTextureObject_t` mediante `cudaMallocArray` y `cudaCreateTextureDesc`. Haz el código separado en `.cuh` y `.cu`."*
3. *"Actualiza el `Material.cuh` y la función CUDA de `calculateBounce` para que pueda recibir varios `cudaTextureObject_t` (albedo, roughness, metallic, etc.). El valor final de roughness, por ejemplo, debe ser el `m.roughness` base multiplicado por el valor leído del tex2D (o solo el base si no hay textura)."*
4. *"Muestra un ejemplo usando `main.cu` para instanciar una textura en disco, pasarla a un material y renderizar la esfera con ella."*
