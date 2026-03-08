# Guía de Paralelización GPU para el Ray Tracer

## Índice

1. [Opciones de paralelización GPU](#1-opciones-de-paralelización-gpu)
2. [Decisión: SYCL con AdaptiveCpp](#2-decisión-sycl-con-adaptivecpp)
   - [Arquitectura híbrida: render GPU + físicas CPU](#21-arquitectura-híbrida-render-gpu--físicas-cpu)
   - [Data-Oriented Programming: ¿cuánto cambio extra añade?](#22-data-oriented-programming-cuánto-cambio-extra-añade)
3. [Cambios necesarios en el proyecto](#3-cambios-necesarios-en-el-proyecto)
   - [Random — estado estático global](#31-random--estado-estático-global)
   - [Jerarquía Geometry — dispatch virtual](#32-jerarquía-geometry--dispatch-virtual)
   - [Jerarquía Solver — dispatch virtual y `std::vector`](#33-jerarquía-solver--dispatch-virtual-y-stdvector)
   - [Jerarquía Material / Texture — dispatch virtual y `shared_ptr`](#34-jerarquía-material--texture--dispatch-virtual-y-shared_ptr)
   - [RenderHit / RenderHitList — `vector` y `shared_ptr`](#35-renderhit--renderhitlist--vector-y-shared_ptr)
   - [Vector3, Matrix4x4, Ray, Transform — tipos valor](#36-vector3-matrix4x4-ray-transform--tipos-valor)
   - [Camera::render() — el bucle de renderizado](#37-camerarender--el-bucle-de-renderizado)
   - [Recursión de rayos (maxDepth)](#38-recursión-de-rayos-maxdepth)
   - [Salida de imagen — `vector<vector<Color>>`](#39-salida-de-imagen--vectorvectorcolor)
   - [Sistema de build (Makefile)](#310-sistema-de-build-makefile)
4. [Resumen de impacto por opción](#4-resumen-de-impacto-por-opción)

---

## 1. Opciones de paralelización GPU

### 1.1 CUDA (NVIDIA)

| | |
|---|---|
| **Plataformas** | Solo NVIDIA |
| **Sintaxis** | C++ con extensiones (`__global__`, `__device__`, `__host__`) |
| **Madurez** | Muy alta — ecosistema extenso, documentación excelente |

**Ventajas:**
- El mejor rendimiento disponible en hardware NVIDIA.
- Soporte nativo de CUDA Math (sqrt, fabs, etc. en device).
- Virtual dispatch en device soportado desde CUDA 5.0 (con restricciones: todos los objetos deben ser allocated en `cudaMalloc`, no en stack/host heap).
- Profiling con Nsight, nvprof — herramientas de clase industrial.
- Enorme cantidad de ejemplos de ray tracers en CUDA.

**Desventajas:**
- **Sólo NVIDIA.** No funciona en AMD, Intel, ARM Mali, ni Apple Silicon.
- Requiere nvcc o clang con target CUDA. No compila con g++ estándar.
- Los `std::shared_ptr`, `std::vector`, y la mayoría de la STL **no están disponibles en device code**.
- La semántica de memoria (global, shared, constant, texture) requiere diseño explícito.
- El static seed de `Random` causa race conditions entre threads CUDA.

---

### 1.2 HIP / ROCm (AMD, con compatibilidad CUDA)

| | |
|---|---|
| **Plataformas** | AMD primario; NVIDIA via hipify-perl |
| **Sintaxis** | Casi idéntica a CUDA (`__global__`, `__device__`, `__host__`) |
| **Madurez** | Media-alta — estable para AMD moderno |

**Ventajas:**
- API prácticamente idéntica a CUDA: una migración de CUDA a HIP es mecánica (`hipify-perl`).
- Soporta AMD (RX/RX Vega/RDNA) y NVIDIA con el mismo código fuente.
- `hipcc` acepta archivos `.cpp` con anotaciones HIP, sin extensión de archivo especial.

**Desventajas:**
- Mismo conjunto de limitaciones que CUDA respecto a STL y virtual dispatch.
- El soporte NVIDIA vía HIP tiene overhead y es menos óptimo que CUDA directo.
- El ecosistema es bastante menor que CUDA (menos ejemplos, foros, profiling tools).
- No soporta Apple Silicon ni la mayoría de GPUs integradas Intel antiguas.

---

### 1.3 OpenCL (Khronos)

| | |
|---|---|
| **Plataformas** | NVIDIA, AMD, Intel, ARM Mali, CPU x86, FPGA |
| **Sintaxis** | Kernels en C99 (subset), host en C/C++ |
| **Madurez** | Muy alta — es el estándar más portable que existe |

**Ventajas:**
- **La plataforma de mayor cobertura**: cualquier GPU discreta o integrada con driver OpenCL.
- Puede usar CPU como fallback sin cambiar el kernel.
- Sin restricciones de vendor, código abierto de especificación.

**Desventajas:**
- Los kernels deben escribirse en **C99 puro** (no C++): no hay clases, templates, virtual dispatch, STL, ni lambdas.
- Toda la jerarquía de objetos (Geometry, Material, Solver) debe ser completamente reescrita en kernels C.
- Código extremadamente verboso en el host: crear plataforma, contexto, queue, compilar programa, crear buffers, set args, enqueue, sync...
- **Mayor cantidad de cambios** en el proyecto de todas las opciones.
- OpenCL 3.0 introduce algo de C++ vía `cl_khr_extended_versioning`, pero el soporte es inconsistente.

---

### 1.4 SYCL (Khronos) ⭐ Recomendado

| | |
|---|---|
| **Plataformas** | NVIDIA (CUDA backend), AMD (ROCm backend), Intel (Level Zero / OpenCL), CPU, ARM |
| **Sintaxis** | **C++17 estándar** — los kernels son lambdas C++ |
| **Madurez** | Alta — implementaciones: Intel oneAPI DPC++, AdaptiveCpp (ex-hipSYCL), triSYCL |

**Ventajas:**
- **Sintaxis C++ nativa**: los kernels son lambdas `[=](sycl::nd_item<2> item) { ... }` — sin lenguaje nuevo que aprender.
- El **código host no cambia casi nada**: la lógica de la cámara, la escena, etc. permanecen en C++ estándar.
- Funciona sobre CUDA, ROCm, OpenCL, Level Zero y CPU — un solo código fuente.
- Los tipos valor puro (Vector3, Matrix4x4, Ray, Limits, HitRecord, Transform) funcionan en device **sin ninguna anotación** si se compilan con el compilador SYCL.
- AdaptiveCpp compila con clang++ estándar + plugin; oneAPI con `icpx`.
- `sycl::sqrt`, `sycl::fabs`, etc. son equivalentes directos de `std::sqrt`.

**Desventajas:**
- Virtual dispatch en device code tiene soporte limitado y experimental (no fiable en todos los backends). Requiere aplanar la jerarquía en kernels.
- `std::shared_ptr` y `std::vector` no son usables en device buffers — igual que en CUDA.
- La compilación es más lenta que CUDA nativo y requiere instalar AdaptiveCpp o Intel oneAPI.
- El rendimiento puro en NVIDIA es ligeramente inferior a CUDA nativo (overhead de abstracción pequeño).
- Curva de aprendizaje del modelo de memoria SYCL (USM vs buffers/accessors).

---

### 1.5 Vulkan Compute Shaders

| | |
|---|---|
| **Plataformas** | NVIDIA, AMD, Intel, Qualcomm (mobile), Apple via MoltenVK |
| **Sintaxis** | GLSL/HLSL compilado a SPIR-V |
| **Madurez** | Muy alta |

**Ventajas:**
- El API gráfico más portable de la industria.
- Integración natural si ya se usa Vulkan para presentar el resultado (display en ventana).

**Desventajas:**
- Los shaders se escriben en GLSL — **reescritura total** de toda la lógica de intersección.
- Código host increíblemente verboso (VkInstance, VkDevice, VkPipeline, VkDescriptorSet, sync barriers...).
- **Mayor esfuerzo de cambio** del proyecto junto a OpenCL.
- Sin tipos ni clases C++ en shaders.

---

### 1.6 Metal (Apple)

| | |
|---|---|
| **Plataformas** | macOS / iOS / visionOS únicamente |
| **Sintaxis** | Metal Shading Language (C++14 subset) |

**Ventajas:**
- Rendimiento óptimo en Apple Silicon (M1/M2/M3).
- MetalRT ofrece soporte nativo de ray tracing acelerado por hardware.

**Desventajas:**
- **Solo Apple**. No existe en ningún otro sistema operativo.
- Requiere Xcode o metal-cpp.

---

## 2. Decisión: SYCL con AdaptiveCpp

> **Opción elegida: SYCL con AdaptiveCpp.**

**Motivo:**
1. El proyecto está en C++ moderno con tipos valor bien definidos (`Vector3`, `Matrix4x4`, `Ray`, etc.) — estos funcionan directamente en device SYCL **sin ningún cambio**.
2. El bucle de render (`Camera::render()`) aún no está implementado — es el momento ideal para diseñarlo orientado a GPU desde el principio.
3. Un único código fuente funciona en NVIDIA, AMD, Intel y CPU.
4. AdaptiveCpp se instala via `apt` o `brew`; la integración con CMake o Makefile es de 2-3 líneas.
5. Los cambios al código existente son los mínimos entre todas las opciones GPU reales, porque los kernels son lambdas C++ normales.
6. La separación en capas que requiere SYCL es exactamente la misma que permite reutilizar las matemáticas y geometrías en un motor físico CPU (ver sección 2.1).
7. Los cambios estructurales necesarios para SYCL son en la práctica los mismos que implementar Data-Oriented Programming, sin coste adicional (ver sección 2.2).

---

### 2.1 Arquitectura híbrida: render GPU + físicas CPU

La intención de reutilizar las capas de matemáticas y geometría para un motor físico que correrá en CPU es **directamente compatible** con SYCL, siempre que se respete la siguiente separación de capas:

```
┌─────────────────────────────────────────────────────────────────────┐
│  CAPA 0 — Matemáticas puras                                         │
│  include/maths/  →  Vector3, Matrix4x4, Ray, Limits, HitRecord      │
│  include/geometry/Transform.hpp                                     │
│                                                                     │
│  Sin includes de SYCL. Compila con g++ estándar.                    │
│  Reutilizable por el motor físico sin ningún cambio.                │
├─────────────────────────────────────────────────────────────────────┤
│  CAPA 1 — Geometría e intersección                                  │
│  include/geometry/  →  GeometryData + geometryHit()                 │
│  include/maths/Solver.hpp  →  Solutions + solveXxx()                │
│  include/geometry/AABB.hpp                                          │
│                                                                     │
│  Sin includes de SYCL. Compila con g++ estándar.                    │
│  El motor físico llama a geometryHit() directamente en CPU.         │
├─────────────────────────────────────────────────────────────────────┤
│  CAPA 2 — Escena y materiales                                       │
│  include/render/RenderHit.hpp  →  SceneObject, RenderHitList        │
│  include/render/Material.hpp  →  MaterialData                       │
│                                                                     │
│  Sin includes de SYCL.                                              │
│  RenderHitList sigue usando vector/shared_ptr en host.              │
├─────────────────────────────────────────────────────────────────────┤
│  CAPA 3 — Dispatch GPU  ← ÚNICA CAPA QUE TOCA SYCL                  │
│  src/render/Camera.cpp                                              │
│  #include <sycl/sycl.hpp> solo aquí                                 │
│  Kernel parallel_for, buffer transfers, RngState                    │
│                                                                     │
│  El motor físico nunca incluye este archivo.                        │
└─────────────────────────────────────────────────────────────────────┘
```

**Consecuencia práctica**: el motor físico es un ejecutable separado que incluye solo las capas 0 y 1, compilado con `g++` estándar **sin instalar AdaptiveCpp ni ningún runtime GPU**. Al refactorizar la geometría para SYCL (sección 3.2), la capa 1 queda como funciones libres en C++ puro, que el motor físico puede llamar directamente:

```cpp
// Motor físico (CPU, compilado con g++ -std=c++17) — sin tocar SYCL
#include "geometry/Geometry.hpp"   // GeometryData + geometryHit()
#include "geometry/AABB.hpp"
#include "maths/Vector3.hpp"

// La detección de colisión reutiliza exactamente la misma lógica
// de intersección que el ray tracer usa en los kernels GPU
bool physicsNarrowPhase(const GeometryData &a, const GeometryData &b)
{
    Point3  originA = a.transform.getMatrix() * Point3::Zero;
    Point3  originB = b.transform.getMatrix() * Point3::Zero;
    Vector3 dir     = (originB - originA).normalized();
    Ray     probe(originA, dir);
    return geometryHit(b, probe, Limits(1e-4, 1e6)).hit;
}
```

**Regla de oro de mantenimiento**: las capas 0 y 1 **nunca** deben incluir `<sycl/sycl.hpp>` ni ningún header de CUDA/HIP/OpenCL. Si en el futuro se quiere portar también el motor físico a GPU, esas capas funcionarán sin ninguna modificación.

---

### 2.2 Data-Oriented Programming: ¿cuánto cambio extra añade?

**Respuesta corta: prácticamente ninguno.** Los cambios ya descritos en las secciones 3.2–3.5 **son** Data-Oriented Design aplicado al problema.

DOP se define por tres principios, todos cubiertos por los cambios ya recomendados:

| Principio DOP | Cambio ya recomendado | Sección |
|---|---|---|
| Separar datos del comportamiento | `GeometryData` (datos) + `geometryHit()` (función libre) | 3.2 |
| Estructuras de datos planas y contiguas | `SceneObject[]` buffer, `Solutions` array fijo de tamaño 4 | 3.3, 3.5 |
| Eliminar indirecciones en hot path | Suprimir `shared_ptr<Geometry>`, `shared_ptr<Texture>`, vtables | 3.2, 3.4, 3.5 |

Lo único que DOP estricto añadiría más allá de esto es **Structure of Arrays (SoA)** en lugar de Array of Structures (AoS):

```cpp
// AoS — lo que se recomienda aquí
SceneObject objects[] = { {geom0, mat0}, {geom1, mat1}, ... };

// SoA — DOP estricto
GeometryType types[]     = { Sphere, Torus, ...  };
Transform    transforms[] = { t0, t1, ...         };
MaterialData materials[]  = { m0, m1, ...         };
```

**¿Vale la pena SoA para este proyecto?**

| Caso de uso | AoS (recomendado) | SoA (DOP estricto) |
|---|---|---|
| **Ray tracer GPU** | Óptimo: cada rayo accede a 1 objeto → misma cacheline | Marginal: la GPU ya carga cachelines completas por warp |
| **Motor físico CPU — broad phase** | Aceptable para <10k objetos | Beneficioso si se itera solo `transforms[]` para AABB checks |
| **Motor físico CPU — narrow phase** | Equivalente: se accede a un objeto completo de todas formas | Sin diferencia |
| **Esfuerzo de implementación** | Directo, igual que hoy | Requiere índices separados y builds de escena más complejos |

**Conclusión DOP**: el AoS recomendado (`SceneObject[]`) aporta el **95% de los beneficios de DOP** — eliminar vtables, `shared_ptr` y heap allocations en hot path — sin ningún trabajo adicional respecto a lo ya documentado. Para el motor físico en CPU, si en el futuro el broad-phase resulta ser el cuello de botella con escenas de >50k objetos, considera migrar solo la lista de AABBs a un array independiente (`AABB[]` plano). El ray tracer no se beneficiaría de ese cambio.

---

## 3. Cambios necesarios en el proyecto

---

### 3.1 `Random` — estado estático global

**Archivo:** [include/maths/Random.hpp](include/maths/Random.hpp), [src/maths/Random.cpp](src/maths/Random.cpp)

**Problema:**  
`Random::seed` es un `static unsigned long` — un único valor compartido por todos los threads. En GPU, miles de work-items ejecutan en paralelo: el resultado sería indeterminado y havría race conditions.

**Opción recomendada (SYCL / CUDA / HIP):**  
Convertir `Random` a un tipo de valor sin estado estático. Cada píxel (work-item) mantiene su propio estado de PRNG:

```cpp
// Nuevo diseño — sin static, instanciable
struct RngState {
    unsigned long seed;

    RngState() = default;
    explicit RngState(unsigned long seed) : seed(seed) {}

    void xorshift64() {
        seed ^= seed << 13;
        seed ^= seed >> 7;
        seed ^= seed << 17;
    }

    float randomfloat() {
        xorshift64();
        return static_cast<float>(seed) / static_cast<float>(ULONG_MAX);
    }

    // ... resto de métodos, todos non-static
};
```

En el kernel, cada work-item inicializa su `RngState` con una semilla única:
```cpp
// En SYCL:
size_t id = item.get_global_linear_id();
RngState rng(baseSeed ^ (id * 6364136223846793005ULL + 1442695040888963407ULL));
```

**Cambio en código existente:** Solo el interior del bucle render y los sitios que llaman a `Random::randomfloat()` etc. — el host no cambia.

---

### 3.2 Jerarquía `Geometry` — dispatch virtual

**Archivos:** [include/geometry/Geometry.hpp](include/geometry/Geometry.hpp), todos los `.cpp` en [src/geometry/](src/geometry/)

**Problema:**  
`Geometry` tiene un método `virtual hit()` puro. En GPU (SYCL, CUDA, HIP), el dispatch virtual funciona con restricciones severas: los objetos con vtable deben ser creados **en device** (no pueden ser copiados desde host como datos planos). Esto hace inviable el patrón `shared_ptr<Geometry>` + polimorfismo en device.

**Opción recomendada:**  
Aplanar la jerarquía en un **tagged union** (discriminated union / sum type):

```cpp
enum class GeometryType : uint8_t {
    Sphere, Plane, Cylinder, Cone, Hyperboloid, Torus, Kiss, Triangle
};

struct GeometryData {
    GeometryType type;
    Transform    transform;
    union {
        struct { float thickness; }  torus;   // Torus::thickness
        struct { float waist; }      hyperboloid;
        struct { float sharpness; }  kiss;
        struct { Point3 v0, v1, v2; Vector3 normal; } triangle;
        // Sphere, Plane, Cylinder, Cone: solo usan transform
    };
};

// Función libre (no virtual) — funciona en GPU
HitRecord geometryHit(const GeometryData &g, const Ray &ray, const Limits &limits);
```

El interior de `geometryHit` es un `switch(g.type)` que llama a la lógica ya implementada en cada `.cpp` actual, ahora como funciones libres.

**Cambio en código existente:**
- Los constructores de cada clase (Sphere, Torus, etc.) se convierten en funciones factory que rellenan un `GeometryData`.
- El método `hit()` virtual desaparece y pasa a ser la función libre `geometryHit()`.
- La variable `transform` (ya en `Geometry`) se mueve a `GeometryData`.
- `getLocalRay()` pasa a ser función libre que toma un `Transform`.

---

### 3.3 Jerarquía `Solver` — dispatch virtual y `std::vector`

**Archivos:** [include/maths/Solver.hpp](include/maths/Solver.hpp), [src/maths/BhaskaraSolver.cpp](src/maths/BhaskaraSolver.cpp), [src/maths/CardanoSolver.cpp](src/maths/CardanoSolver.cpp), [src/maths/FerrariSolver.cpp](src/maths/FerrariSolver.cpp), [src/maths/LinearSolver.cpp](src/maths/LinearSolver.cpp)

**Problemas:**
1. `Solver::getSolutions()` es virtual — mismo problema de dispatch.
2. Retorna `std::vector<Solution>` — heap allocation no disponible en device code.

**Opción recomendada:**  
Abandonar el polimorfismo y usar arrays de tamaño fijo:

```cpp
// Máximo de raíces es 4 (FerrariSolver)
struct Solutions {
    Solution data[4];
    int      count;
};

// Funciones libres directas — no virtuales
Solutions solveLinear(float a, float b);
Solutions solveBhaskara(float a, float b, float c);
Solutions solveCardano(float a, float b, float c, float d);
Solutions solveFerrari(float a, float b, float c, float d, float e);
```

El código interno de cada solver (ya implementado en los `.cpp`) no cambia — solo el empaquetado del resultado pasa de `push_back` a índice fijo.

**Cambio en código existente:**
- Reemplazar `vector<Solution>` por `Solutions` en los `.cpp` de geometría (Sphere.cpp, Torus.cpp, etc.).
- El bucle `for (Solution sol : sols)` se convierte en `for (int i = 0; i < sols.count; ++i)`.

---

### 3.4 Jerarquía `Material` / `Texture` — dispatch virtual y `shared_ptr`

**Archivo:** [include/render/Material.hpp](include/render/Material.hpp)

**Problemas:**
1. `Material` y `Texture` tienen métodos virtuales.
2. `Material::albedo` es `shared_ptr<Texture>` — no funciona en device.

**Opción recomendada:**  
Mismo patrón de tagged union:

```cpp
enum class MaterialType : uint8_t { Lambertian, Metal, Dielectric, DiffuseLight };
enum class TextureType  : uint8_t { SolidColor, Checker, Image };

struct TextureData {
    TextureType type;
    Color       solidColor; // para SolidColor
    // otros campos según tipo...
};

struct MaterialData {
    MaterialType type;
    TextureData  albedo;
    float       param; // roughness (Metal), ior (Dielectric), etc.
};

// Función libre
bool materialScatter(const MaterialData &mat, const Ray &rayIn,
                     const HitRecord &rec, Color &attenuation, Ray &scattered, RngState &rng);
Color materialEmitted(const MaterialData &mat, float u, float v, const Point3 &p);
```

**Cambio en código existente:**
- Eliminar `shared_ptr<Texture>` y las clases virtuales.
- `RenderHit` usa `MaterialData` en lugar de `Material`.

---

### 3.5 `RenderHit` / `RenderHitList` — `vector` y `shared_ptr`

**Archivo:** [include/render/RenderHit.hpp](include/render/RenderHit.hpp)

**Problemas:**
1. `RenderHitList::objects` es `vector<RenderHit>` — heap allocation, no sirve en device.
2. `RenderHit::geometry` es `shared_ptr<Geometry>` — mismo problema.
3. El dispatch virtual de `RenderHit::hit()` tampoco sirve en device.

**Opción recomendada:**  
Exponer la escena como un **buffer plano** de structs en un array de device:

```cpp
struct SceneObject {
    GeometryData geometry;
    MaterialData material;
};

// En el host:
std::vector<SceneObject> hostScene;
// Se copia a device una sola vez antes del render
```

En SYCL se usa `sycl::buffer<SceneObject>` o Unified Shared Memory (`sycl::malloc_shared`).  
En CUDA se usa `cudaMalloc` + `cudaMemcpy`.

El hit de la escena completa es una función libre que itera el array:
```cpp
bool sceneHit(const SceneObject *objects, int count,
              const Ray &r, const Limits &limits, HitRecord &rec, int &hitIdx);
```

**Cambio en código existente:**
- `RenderHitList` solo se usa en host para construir la escena y trasvasarla al buffer de device.
- El método virtual `hit()` desaparece del kernel — se usa la función libre anterior.

---

### 3.6 `Vector3`, `Matrix4x4`, `Ray`, `Transform` — tipos valor

**Archivos:** [include/maths/Vector3.hpp](include/maths/Vector3.hpp), [include/maths/Matrix4x4.hpp](include/maths/Matrix4x4.hpp), [include/maths/Ray.hpp](include/maths/Ray.hpp), [include/geometry/Transform.hpp](include/geometry/Transform.hpp)

**Problema:**
Ninguno de carácter funcional. Estos son tipos valor con layout de memoria contiguo (arrays internos `float v[4]`, `float m[4][4]`). Sin herencia virtual, sin heap allocation.

**Con SYCL (decisión tomada):**  
No se requiere ningún cambio de código. AdaptiveCpp y oneAPI compilarán estos tipos directamente en device sin anotaciones adicionales.

**Beneficio adicional para el motor físico**: al no requerir ninguna anotación especial, estos mismos headers se incluyen directamente en el motor físico compilado con `g++` estándar. La capa de matemáticas es completamente agnóstica de la plataforma de ejecución (ver sección 2.1).

**Nota para referencia futura — CUDA / HIP:**  
Si en algún momento se quisiera portar a CUDA, habría que añadir `__host__ __device__` a todos los métodos y funciones inline:

```cpp
// Antes:
Vector3 operator+(const Vector3 &v1, const Vector3 &v2);

// Con CUDA:
__host__ __device__ Vector3 operator+(const Vector3 &v1, const Vector3 &v2);
```

Esto aplica a ~30 funciones en Vector3.hpp/cpp, ~10 en Matrix4x4, y ~5 en Ray. Es mecánico pero tedioso — otra razón para preferir SYCL.

**Consideración de layout:** `Vector3` almacena `float v[4]` (padding de 4 componentes). En GPU, los tipos de 4 floats son nativos y muy eficientes. Este diseño es conveniente tanto para GPU como para SIMD en el motor físico CPU (`__m256d` AVX2 encaja perfectamente con 4 floats).

---

### 3.7 `Camera::render()` — el bucle de renderizado

**Archivo:** [src/render/Camera.cpp](src/render/Camera.cpp)

**Estado actual:** El método está implementado pero solo crea la imagen con el color de fondo y retorna — el TODO de adaptive sampling está pendiente.

**Cambio necesario (el más importante):**  
Este es el único lugar donde se escribe el kernel GPU. La estructura natural es:

```
Por cada píxel (i, j)  →  1 work-item en GPU
  Por cada sample       →  loop interno (o dimensión extra)
    Lanzar ray primario
    Iterar rebotes (hasta maxDepth — ver sección 3.8)
    Acumular color
  Promediar samples → escritura en buffer de salida
```

**En SYCL:**
```cpp
vector<vector<Color>> Camera::render(RenderHitList &world)
{
    // 1. Construir escena plana desde world
    std::vector<SceneObject> hostScene = world.flatten();
    
    sycl::queue q;
    sycl::buffer<SceneObject> sceneBuf(hostScene.data(), hostScene.size());
    sycl::buffer<Color>       imageBuf(width * height);
    
    q.submit([&](sycl::handler &cgh) {
        auto sceneAcc = sceneBuf.get_access<sycl::access::mode::read>(cgh);
        auto imageAcc = imageBuf.get_access<sycl::access::mode::write>(cgh);
        
        // Capturar parámetros de cámara por valor
        auto cam = *this;
        
        cgh.parallel_for(sycl::range<2>(height, width),
            [=](sycl::item<2> item) {
                int j = item.get_id(0);
                int i = item.get_id(1);
                size_t linearId = item.get_linear_id();
                
                RngState rng(baseSeed ^ linearId);
                Color    pixelColor = Color::Zero;
                
                for (int s = 0; s < cam.samplesPerPixel; ++s) {
                    Ray ray = cam.getRay(i, j, rng);
                    pixelColor += rayColor(ray, sceneAcc.get_pointer(),
                                           sceneAcc.size(), cam.maxDepth, rng,
                                           cam.backgroundColor);
                }
                imageAcc[j * cam.width + i] = pixelColor * cam.pixelSamplesScale;
            });
    });
    
    // 2. Copiar resultado de device a host
    auto hostAccess = imageBuf.get_host_access();
    vector<vector<Color>> image(height, vector<Color>(width));
    for (int j = 0; j < height; ++j)
        for (int i = 0; i < width; ++i)
            image[j][i] = hostAccess[j * width + i];
    return image;
}
```

**Cambio en código existente:** Solo el cuerpo de `Camera::render()`. La firma pública y los atributos de la clase no cambian (excepto que hay que exponer `getRay()` como método público o hacer el kernel `friend`).

---

### 3.8 Recursión de rayos (`maxDepth`)

**Problema crítico:** Los GPUs tienen una pila de llamadas muy limitada. La recursión profunda (hasta `maxDepth` = N) puede causar stack overflow en device o en el peor caso simplemente no compilar en ciertos backends.

**Opción recomendada:**  
Convertir el trace recursivo en un **bucle iterativo** con acumulación de atenuación:

```cpp
// Versión recursiva (NO apta para GPU):
Color rayColor(const Ray &ray, ..., int depth) {
    if (depth <= 0) return Color::Zero;
    HitRecord rec;
    if (sceneHit(..., rec)) {
        Ray scattered; Color attenuation;
        if (mat.scatter(..., scattered)) {
            return attenuation * rayColor(scattered, ..., depth - 1); // recursión
        }
        return mat.emitted(...);
    }
    return backgroundColor;
}

// Versión iterativa (apta para GPU):
Color rayColor(Ray ray, const SceneObject *objects, int count,
               int maxDepth, RngState &rng, const Color &bg)
{
    Color   throughput = Color::One; // acumulador multiplicativo
    Color   emitted    = Color::Zero;

    for (int bounce = 0; bounce < maxDepth; ++bounce) {
        HitRecord rec;
        int hitIdx;
        if (!sceneHit(objects, count, ray, Limits(1e-4, INFINITY), rec, hitIdx)) {
            emitted += throughput * bg;
            break;
        }
        const MaterialData &mat = objects[hitIdx].material;
        emitted += throughput * materialEmitted(mat, 0, 0, rec.point);

        Color attenuation;
        Ray   scattered;
        if (!materialScatter(mat, ray, rec, attenuation, scattered, rng)) {
            break; // rayo absorbido
        }
        throughput *= attenuation;
        ray = scattered;
    }
    return emitted;
}
```

**Cambio en código existente:** Solo la función interna de trace — no afecta a ninguna clase pública.

---

### 3.9 Salida de imagen — `vector<vector<Color>>`

**Archivo:** [src/render/Camera.cpp](src/render/Camera.cpp)

**Problema menor:**  
`vector<vector<Color>>` es un array 2D no contiguo en memoria (cada fila es una heap allocation separada). Para la transferencia GPU→host es preferible un array plano.

**Opción recomendada:**  
El buffer de device es `Color[width * height]` (plano). La conversión al `vector<vector<Color>>` que retorna `render()` se hace en host tras la copia — la interfaz pública de `Camera` **no cambia**:

```cpp
// El buffer interno es plano:
std::vector<Color> flatBuffer(width * height);
// ... GPU escribe aquí ...

// Conversión de vuelta al formato de retorno actual:
vector<vector<Color>> image(height, vector<Color>(width));
for (int j = 0; j < height; ++j)
    image[j].assign(flatBuffer.begin() + j * width,
                    flatBuffer.begin() + (j + 1) * width);
return image;
```

**Cambio en código existente:** Ninguno visible desde fuera de `Camera` — la firma de `render()` no cambia.

---

### 3.10 Sistema de build (Makefile)

**Archivo:** [Makefile](Makefile) (actualmente vacío)

**Cambio necesario** para compilar con SYCL (AdaptiveCpp):

```makefile
CXX      = acpp          # Compilador de AdaptiveCpp
ACPP_FLAGS = --acpp-targets="cuda:sm_86"  # Ajustar a tu GPU
             # "omp" para CPU, "hip:gfx1030" para AMD, 
             # "opencl:cpu" para CPU via OpenCL

CXXFLAGS = -O3 -std=c++17 $(ACPP_FLAGS)
INCLUDES = -I include/maths -I include/geometry -I include/render -I include
SRCDIR   = src
SRCS     = $(shell find $(SRCDIR) -name "*.cpp")
OBJS     = $(SRCS:.cpp=.o)
TARGET   = ray_tracer

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	find $(SRCDIR) -name "*.o" -delete
	rm -f $(TARGET)
```

Para **CUDA** en su lugar:
```makefile
CXX      = nvcc
CXXFLAGS = -O3 -std=c++17 --expt-relaxed-constexpr
# Renombrar archivos de kernel a .cu o usar -x cu
```

---

## 4. Resumen de impacto por opción

| Componente | SYCL (rec.) | CUDA | HIP | OpenCL | Vulkan |
|---|---|---|---|---|---|
| `Vector3`, `Matrix4x4`, `Ray` | Sin cambios | +`__device__` en ~40 funciones | Igual que CUDA | Reescribir en C99 | Reescribir en GLSL |
| `Geometry` (virtual) | Tagged union | Tagged union | Tagged union | C structs + switch | GLSL structs |
| `Solver` (virtual + vector) | Array fijo | Array fijo | Array fijo | Array fijo C | GLSL arrays |
| `Material` / `Texture` (virtual) | Tagged union | Tagged union | Tagged union | C structs | GLSL structs |
| `RenderHitList` (vector + shared_ptr) | Buffer plano | Buffer CUDA | Buffer HIP | Buffer OpenCL | Descriptor sets |
| `Random` (static seed) | Por work-item | Por thread | Por thread | Por work-item | Por invocation |
| `Camera::render()` | Kernel SYCL | Kernel CUDA | Kernel HIP | Kernel OpenCL | Compute shader |
| Recursión → iteración | Sí | Sí | Sí | Sí | Sí |
| Cambios en código host | Mínimos | Moderados | Moderados | Extensos | Total |
| Cambios en lógica device | Moderados | Moderados | Moderados | Total | Total |
| Plataformas soportadas | ★★★★★ | ★★ | ★★★ | ★★★★★ | ★★★★ |
| Esfuerzo total | ★★★ | ★★★ | ★★★ | ★★★★★ | ★★★★★ |

> **Conclusión**: SYCL y CUDA requieren el mismo nivel de cambios en la lógica interna del ray tracer (aplanar jerarquías, array fijo para solvers, RNG por work-item, recursión iterativa). La diferencia decisiva es que SYCL soporta todas las plataformas GPU con el mismo código fuente, mientras que CUDA solo corre en NVIDIA. **Opción elegida: SYCL con AdaptiveCpp.**

> **Sobre el motor físico y DOP**: los cambios recomendados (tagged unions, funciones libres, arrays fijos) son en sí mismos Data-Oriented Design. No requieren trabajo adicional respecto a lo ya documentado, y además producen una capa de geometría en C++ puro que el motor físico puede reutilizar compilando con `g++` estándar, sin ninguna dependencia de SYCL ni de GPU.
