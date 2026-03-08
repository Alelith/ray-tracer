# Guía de Data-Oriented Programming (DOP)

Una introducción práctica usando este ray tracer como ejemplo.

## Índice

1. [¿Qué es DOP y por qué existe?](#1-qué-es-dop-y-por-qué-existe)
2. [El problema raíz: cómo funciona la memoria moderna](#2-el-problema-raíz-cómo-funciona-la-memoria-moderna)
3. [OOP vs DOP: la misma lógica, diferente organización](#3-oop-vs-dop-la-misma-lógica-diferente-organización)
4. [Los patrones de DOP y sus tipos](#4-los-patrones-de-dop-y-sus-tipos)
   - [Tipos valor vs tipos referencia](#41-tipos-valor-vs-tipos-referencia)
   - [Array of Structures (AoS)](#42-array-of-structures-aos)
   - [Structure of Arrays (SoA)](#43-structure-of-arrays-soa)
   - [Tagged union (sum type)](#44-tagged-union-sum-type)
   - [Funciones libres vs métodos virtuales](#45-funciones-libres-vs-métodos-virtuales)
5. [Aplicado a este proyecto: antes y después](#5-aplicado-a-este-proyecto-antes-y-después)
   - [Geometry: de jerarquía virtual a tagged union](#51-geometry-de-jerarquía-virtual-a-tagged-union)
   - [Solver: de vector dinámico a array fijo](#52-solver-de-vector-dinámico-a-array-fijo)
   - [Escena: de shared_ptr a buffer plano](#53-escena-de-shared_ptr-a-buffer-plano)
   - [Random: de estado global a estado por instancia](#54-random-de-estado-global-a-estado-por-instancia)
6. [¿Cuándo NO usar DOP?](#6-cuándo-no-usar-dop)
7. [Resumen: reglas prácticas](#7-resumen-reglas-prácticas)

---

## 1. ¿Qué es DOP y por qué existe?

**Data-Oriented Programming** (DOP, también llamado Data-Oriented Design o DOD) es un paradigma de programación cuya premisa central es:

> **Organiza los datos según cómo se van a acceder, no según cómo los modelas conceptualmente.**

Nació en la industria de videojuegos alrededor de 2009, popularizado por Mike Acton (Insomniac Games) y Noel Llopis, como respuesta a un problema real: los juegos modernos tenían jerarquías de objetos en C++ perfectamente elegantes desde un punto de vista de diseño de software, pero el rendimiento era inaceptable porque el hardware no podía ejecutarlas eficientemente.

DOP **no reemplaza** a la programación orientada a objetos — es un conjunto de técnicas que se aplican en las partes críticas del código donde el rendimiento importa. El código de configuración, parsing, UI, e inicialización puede seguir siendo OOP sin problema.

---

## 2. El problema raíz: cómo funciona la memoria moderna

Para entender DOP hay que entender una realidad del hardware que la mayoría de tutoriales de C++ ignoran.

### La jerarquía de memoria

```
CPU (operación a 3 GHz)
│
├── Registros        ~0.3 ns   ~  1 ciclo    ←  donde ocurren los cálculos
├── L1 cache          ~1 ns   ~  4 ciclos    ←  32–64 KB por núcleo
├── L2 cache          ~4 ns   ~ 12 ciclos
├── L3 cache         ~15 ns   ~ 40 ciclos
└── RAM             ~100 ns   ~300 ciclos    ←  si fallas la caché, esperas esto
```

**Un cache miss (fallo de caché) cuesta entre 40 y 300 ciclos** — tiempo en el que la CPU podría haber hecho cientos de operaciones matemáticas. En un ray tracer, donde se lanzan millones de rayos y cada uno comprueba intersecciones contra todos los objetos de la escena, este coste se multiplica dramáticamente.

### La cacheline

La RAM no se lee byte a byte: se lee en bloques de **64 bytes** llamados cachelines. Cuando accedes a `objeto.x`, el hardware carga automáticamente `objeto.x`, `objeto.y`, `objeto.z`, y los siguientes bytes hasta completar 64 bytes. Esto es favorable **si realmente vas a usar todos esos campos**. Pero si solo necesitas `objeto.x` y el resto de los 64 bytes corresponden a campos que nunca tocas en ese bucle, estás desperdiciando ancho de banda de memoria.

### El problema de los punteros

```cpp
// Cuando la CPU ejecuta esto:
for (auto &obj : objects) {
    obj->hit(ray, limits);  // obj es shared_ptr<Geometry>
}
```

Ocurre lo siguiente:
1. Leer `obj` (8 bytes del puntero) — puede estar en caché
2. **Seguir el puntero** → saltar a una dirección aleatoria en heap → probable cache miss
3. Leer la vtable del objeto → **otro puntero** → otra dirección aleatoria → otro probable cache miss
4. Leer la función virtual → saltar a código → posible instrucción cache miss
5. Por fin ejecutar el cálculo de intersección

Con 1000 objetos en escena, esto son potencialmente 3000 cache misses por rayo. Con 4K píxeles × 100 samples = 400M rayos, el número es astronómico.

---

## 3. OOP vs DOP: la misma lógica, diferente organización

Ambos paradigmas implementan exactamente la **misma lógica**. La diferencia es únicamente dónde viven los datos y cómo se accede a ellos.

### OOP — Organizado por entidad

```
Objeto Sphere #1:       Objeto Sphere #2:       Objeto Torus #1:
┌──────────────┐        ┌──────────────┐        ┌──────────────┐
│ vtable ptr   │        │ vtable ptr   │        │ vtable ptr   │
│ transform    │        │ transform    │        │ transform    │
│   matrix     │        │   matrix     │        │   matrix     │
│   (128 bytes)│        │   (128 bytes)│        │   (128 bytes)│
│   invMatrix  │        │   invMatrix  │        │   invMatrix  │
│   (128 bytes)│        │   (128 bytes)│        │   (128 bytes)│
│   transMatrix│        │   transMatrix│        │   transMatrix│
│   (128 bytes)│        │   (128 bytes)│        │   thickness  │
└──────────────┘        └──────────────┘        └──────────────┘
```

Cada objeto está en una dirección aleatoria del heap (porque se creó con `new` / `make_shared`). Para procesar N objetos, el hardware tiene que perseguir N punteros a N posiciones aleatorias.

### DOP — Organizado por acceso

```
Buffer plano de SceneObject[N]:
┌────────────────┬────────────────┬────────────────┬─────
│ SceneObject[0] │ SceneObject[1] │ SceneObject[2] │ ...
│ type: Sphere   │ type: Sphere   │ type: Torus    │
│ transform      │ transform      │ transform      │
│ material       │ material       │ thickness      │
└────────────────┴────────────────┴────────────────┴─────
    ↑
    Dirección base: todo contiguo en memoria
```

Para procesar N objetos, el hardware lee secuencialmente un bloque de memoria contiguo. El prefetcher de la CPU predice el patrón y carga las cachelines por adelantado, eliminando los cache misses.

---

## 4. Los patrones de DOP y sus tipos

### 4.1 Tipos valor vs tipos referencia

Este es el concepto más fundamental. DOP prefiere **tipos valor** (value types) sobre **tipos referencia** (reference types).

| Característica | Tipo valor | Tipo referencia |
|---|---|---|
| Almacenamiento | En el stack o embebido en el struct padre | En el heap, accedido vía puntero |
| Copia | Copia real de los datos | Copia del puntero (mismos datos) |
| Caché | Contiguo con los datos circundantes | Salto de puntero → aleatorio |
| Ownership | Implícito (RAII automático) | Manual o con `shared_ptr` |
| Ejemplos en C++ | `int`, `float`, `struct`, arrays | `new T`, `unique_ptr`, `shared_ptr` |

**En este proyecto**, `Vector3`, `Matrix4x4`, `Ray`, `HitRecord`, `Transform` y `Limits` ya son **tipos valor puros** — no heap allocation, no punteros internos. Son la base correcta sobre la que construir DOP.

```cpp
// Vector3: tipo valor — los datos están aquí mismo, en 32 bytes contiguos
class Vector3 {
    float v[4];  // ← los 4 floats están literalmente aquí, sin indireccion
};

// Ray: tipo valor — contiene dos Vector3 por valor (sin punteros)
class Ray {
    Point3  origin;    // ← 32 bytes aquí
    Vector3 direction; // ← 32 bytes aquí, inmediatamente después
};
```

En contraste, `shared_ptr<Geometry>` es un tipo referencia: los datos reales de la geometría están en otra parte del heap, y hay que seguir el puntero para llegar a ellos.

---

### 4.2 Array of Structures (AoS)

El layout más natural en OOP y el punto de partida de DOP.

```cpp
// Array of Structures: un array de objetos completos
struct SceneObject {
    GeometryData geometry;  // ~400 bytes
    MaterialData material;  // ~50 bytes
};

SceneObject objects[1000]; // 1000 objetos, cada uno completo
```

**Diagrama de memoria:**
```
[geom0 | mat0][geom1 | mat1][geom2 | mat2][geom3 | mat3]...
 ←──obj 0──→  ←──obj 1──→  ←──obj 2──→  ←──obj 3──→
```

**Cuándo es óptimo:** Cuando en cada iteración accedes a **todos o la mayoría** de los campos de cada objeto. En el ray tracer, cuando un rayo intersecta un objeto, necesita tanto la geometría (para el cálculo de intersección) como el material (para el scatter). AoS es exactamente lo correcto aquí: todo el objeto cabe en pocas cachelines.

---

### 4.3 Structure of Arrays (SoA)

La alternativa que maximiza el uso de caché cuando solo accedes a **un subconjunto** de campos en cada iteración.

```cpp
// Structure of Arrays: arrays separados por campo
struct Scene {
    GeometryType types[MAX_OBJECTS];      // array de tipos — 1 byte cada uno
    Transform    transforms[MAX_OBJECTS]; // array de transforms
    MaterialData materials[MAX_OBJECTS];  // array de materiales
    int          count;
};
```

**Diagrama de memoria:**
```
types:      [Sphere][Sphere][Torus][Cone ][Plane]...
transforms: [  t0  ][  t1  ][  t2 ][  t3 ][  t4 ]...
materials:  [  m0  ][  m1  ][  m2 ][  m3 ][  m4 ]...
```

**Cuándo es óptimo:** En el broad-phase de un motor físico, donde primero iteras **solo los bounding boxes** de todos los objetos para descartar la mayoría de colisiones, antes de hacer el cálculo completo de intersección:

```cpp
// Broad phase: solo toca transforms[], ignora completamente materials[]
for (int i = 0; i < scene.count; ++i) {
    if (aabb_overlap(a_transform, scene.transforms[i])) {
        // Solo entonces accedes al resto (narrow phase)
    }
}
```

Con SoA, el bucle de broad-phase carga cachelines densas con solo datos de `transforms`, sin desperdiciar ancho de banda en `materials` que no usa.

**Con AoS el mismo bucle carga:** `[geom+mat][geom+mat][geom+mat]...` — datos de material que no se usan en cada cacheline.

**La regla:** si tienes N objetos y en el 80% de las iteraciones solo tocas el 20% de los campos, SoA gana. Si siempre tocas todo el objeto, AoS gana.

---

### 4.4 Tagged union (sum type)

Este patrón sustituye a la herencia virtual cuando el conjunto de tipos es cerrado y conocido en tiempo de compilación.

#### El problema con virtual

```cpp
// OOP clásico: un puntero + una vtable = dos indirecciones
class Geometry {
    virtual HitRecord hit(const Ray &ray, const Limits &limits) const = 0;
};
class Sphere   : public Geometry { ... };  // vtable propia
class Torus    : public Geometry { ... };  // vtable propia
class Cylinder : public Geometry { ... };  // vtable propia

shared_ptr<Geometry> obj = make_shared<Sphere>(...);
obj->hit(ray, limits);
// En memoria: obj → (heap, Sphere) → vtable → hit() de Sphere
//             ↑ ptr 1              ↑ ptr 2
```

Dos punteros que seguir, dos potenciales cache misses, y el compilador **no puede hacer inlining** porque el tipo real no se conoce en tiempo de compilación.

#### La solución: tagged union

```cpp
// DOP: un struct plano con un campo que dice qué tipo es
enum class GeometryType : uint8_t {
    Sphere, Plane, Cylinder, Cone, Hyperboloid, Torus, Kiss, Triangle
};

struct GeometryData {
    GeometryType type;      // 1 byte — "la etiqueta" (tag)
    Transform    transform; // datos comunes a todos
    union {                 // datos específicos por tipo (solo uno activo)
        struct { /* Sphere, Plane, Cylinder, Cone: sin datos extra */ };
        struct { float thickness;  } torus;
        struct { float waist;      } hyperboloid;
        struct { float sharpness;  } kiss;
        struct { Point3 v0, v1, v2; Vector3 normal; } triangle;
    };
};
```

El dispatch se hace con un `switch` en una función libre:

```cpp
// La lógica de hit() pasa a ser una función libre, no un método virtual
HitRecord geometryHit(const GeometryData &g, const Ray &ray, const Limits &limits)
{
    switch (g.type) {
        case GeometryType::Sphere:   return sphereHit(g, ray, limits);
        case GeometryType::Torus:    return torusHit(g, ray, limits);
        case GeometryType::Plane:    return planeHit(g, ray, limits);
        // ...
    }
}
```

**Ventajas sobre virtual:**
- Sin punteros: `GeometryData` es un tipo valor, copiable, almacenable en arrays
- El compilador puede hacer **inlining** de las funciones del `switch`
- El branch predictor de la CPU aprende el patrón de tipos de la escena
- Compatible con GPU (SYCL/CUDA): las vtables no pueden copiarse al device

**Desventajas:**
- **La jerarquía es cerrada**: añadir un nuevo tipo de geometría requiere modificar el enum y el switch. En OOP, solo heredas. Para un ray tracer con tipos de geometría fijos, esto es un compromiso aceptable.
- El `union` puede ser confuso si los tipos tienen constructores no triviales (en C++17 se resuelve con `std::variant` si se acepta algo de overhead).

#### `std::variant` — el tagged union de la STL

Si los tipos hijos tienen constructores y destructores no triviales y el proyecto no va a GPU, `std::variant` hace lo mismo con menos código manual:

```cpp
#include <variant>

using GeometryVariant = std::variant<SphereData, TorusData, PlaneData, CylinderData>;

struct GeometryData {
    GeometryVariant data;
    Transform       transform;
};

HitRecord geometryHit(const GeometryData &g, const Ray &ray, const Limits &limits)
{
    return std::visit([&](const auto &shape) {
        return shapeHit(shape, g.transform, ray, limits);
    }, g.data);
}
```

`std::visit` compila a un `switch` o a una tabla de función punteros — similar a una vtable pero con los datos embebidos en el struct, no en el heap.

**Nota**: `std::variant` **no funciona en device SYCL/CUDA**. Para GPU hay que usar la versión manual con `union` + `enum`.

---

### 4.5 Funciones libres vs métodos virtuales

En DOP, el comportamiento se separa de los datos. En lugar de métodos en una clase, se usan **funciones libres** que reciben los datos como parámetro.

```cpp
// OOP: el comportamiento está atado al tipo (método)
class Sphere : public Geometry {
    HitRecord hit(const Ray &ray, const Limits &limits) const override { ... }
};

// DOP: el comportamiento es una función que opera sobre datos
HitRecord sphereHit(const GeometryData &g, const Ray &ray, const Limits &limits) { ... }
```

**¿Por qué importa esta diferencia?**

1. **Las funciones libres se pueden compilar para GPU** igual que cualquier función — el compilador SYCL/CUDA las inline directamente en el kernel.

2. **Son más fáciles de testear**: `sphereHit(data, ray, limits)` no requiere construir un objeto `Sphere` completo con todas sus dependencias.

3. **Los datos son portables** sin el comportamiento: puedes enviar `GeometryData[]` a la GPU y usar `geometryHit()` allí, o quedarte en CPU y usarlo también. Los datos no "saben" dónde van a ser procesados.

4. **Permiten múltiples implementaciones**: la misma `GeometryData` puede tener `geometryHit()` para ray tracing y `geometryOverlap()` para el motor físico, sin que el struct sepa de ninguna de las dos.

---

## 5. Aplicado a este proyecto: antes y después

### 5.1 Geometry: de jerarquía virtual a tagged union

**Antes (OOP):**

```cpp
// Geometry.hpp
class Geometry {
protected:
    Transform transform;
    Ray getLocalRay(const Ray &ray) const;
public:
    virtual HitRecord hit(const Ray &ray, const Limits &limits) const = 0;
};

class Sphere : public Geometry {
public:
    Sphere(const Point3 &center, float radius);
    virtual HitRecord hit(const Ray &ray, const Limits &limits) const override;
};

// Uso:
shared_ptr<Geometry> obj = make_shared<Sphere>(center, radius);
// obj vive en el heap, en una dirección aleatoria
// Para usar en GPU: imposible copiar el vtable pointer
```

**Después (DOP):**

```cpp
// Geometry.hpp
enum class GeometryType : uint8_t { Sphere, Plane, Cylinder, Cone,
                                    Hyperboloid, Torus, Kiss, Triangle };

struct GeometryData {
    GeometryType type;
    Transform    transform;
    union {
        struct { float thickness;  } torus;
        struct { float waist;      } hyperboloid;
        struct { float sharpness;  } kiss;
        struct { Point3 v0, v1, v2; Vector3 n; } triangle;
    };
};

// Función factory — reemplaza al constructor
GeometryData makeSphere(const Point3 &center, float radius);
GeometryData makeTorus(float majorR, float minorR, const Point3 &c, const Vector3 &n);

// Función de hit — reemplaza al método virtual
HitRecord geometryHit(const GeometryData &g, const Ray &ray, const Limits &limits);

// Uso:
GeometryData obj = makeSphere(center, radius);
// obj es un struct plano en el stack o en un array
// Para GPU: se copia byte a byte como cualquier dato
```

**Qué cambia en los archivos `.cpp` existentes:**  
El cuerpo de `Sphere::hit()`, `Torus::hit()`, etc. se mueve a `sphereHit()`, `torusHit()`, etc. — la **lógica matemática no cambia en absoluto**. Solo cambia dónde vive y cómo se llama.

---

### 5.2 Solver: de vector dinámico a array fijo

**Antes (OOP):**

```cpp
class BhaskaraSolver : public Solver {
    float a, b, c;
public:
    BhaskaraSolver(float a, float b, float c);
    vector<Solution> getSolutions() const override;  // heap allocation
};

// En Sphere::hit():
vector<Solution> sols = BhaskaraSolver(a, b, c).getSolutions();
for (Solution sol : sols) { ... }
// vector<Solution>: los datos están en el heap, con su propio puntero y size
```

**Después (DOP):**

```cpp
// Máximo de raíces reales para cada ecuación:
// Lineal: 1,  Cuadrática: 2,  Cúbica: 3,  Cuártica: 4
struct Solutions {
    Solution data[4];  // array fijo: sin heap, sin punteros
    int      count;    // cuántas son válidas
};

// Función libre — sin clase, sin virtual
Solutions solveBhaskara(float a, float b, float c);
Solutions solveFerrari(float a, float b, float c, float d, float e);

// En geometryHit() (que reemplaza a Sphere::hit()):
Solutions sols = solveBhaskara(a, b, c);
for (int i = 0; i < sols.count; ++i) {
    Solution &sol = sols.data[i];
    ...
}
// Todo en el stack: cero heap allocations en el hot path del ray tracer
```

**Por qué importa:** En el hot path (la función que se llama millones de veces por frame), cada `vector<Solution>` implica `malloc` + `free` internamente. Con `Solutions` como struct de stack, el compilador puede almacenar los 4 floats **en registros de la CPU** — sin tocar la memoria en absoluto.

---

### 5.3 Escena: de shared_ptr a buffer plano

**Antes (OOP):**

```cpp
// RenderHit.hpp
class RenderHit {
    shared_ptr<Geometry> geometry;  // puntero al heap
    Material             color;
};

class RenderHitList : public RenderHit {
    vector<RenderHit> objects;  // vector de objetos con punteros internos
};
```

El árbol de punteros resultante para una escena de 5 objetos:

```
RenderHitList
├── objects[0]
│   ├── geometry → (heap addr 0x7f3c...)  → Sphere (vtable, transform...)
│   └── color    (Material en stack)
├── objects[1]
│   ├── geometry → (heap addr 0x8a91...)  → Torus (vtable, transform, thickness)
│   └── color
...
```

Para renderizar, el bucle sigue 5 punteros → 5 direcciones aleatorias → hasta 5 cache misses por rayo.

**Después (DOP):**

```cpp
// El "rendering world" como buffer plano
struct SceneObject {
    GeometryData geometry;  // tipo valor, sin punteros
    MaterialData material;  // tipo valor, sin punteros
};

// En Camera::render():
std::vector<SceneObject> hostScene = world.flatten(); // construido una sola vez
// hostScene[0], [1], [2]... están contiguos en memoria
// Para GPU: sycl::buffer<SceneObject>(hostScene.data(), hostScene.size())
```

`RenderHitList` se convierte en un **builder** que solo existe en el host para construir la escena con una API conveniente, y luego la "aplana" en un array:

```cpp
// RenderHitList sigue existiendo — pero ahora como helper de construcción
class RenderHitList {
    std::vector<SceneObject> objects; // ya son tipos valor, no punteros
public:
    void add(GeometryData geometry, MaterialData material);
    std::vector<SceneObject> flatten() const { return objects; }
};
```

---

### 5.4 Random: de estado global a estado por instancia

**Antes:**

```cpp
// Random.hpp — estado estático: un solo seed para todos los threads
class Random {
    static unsigned long seed;  // ← global, compartido
    static void xorshift64();
public:
    static float randomfloat();
    static Vector3 randomUnitVector3();
    // ...
};
```

Problema GPU: si 1024 work-items llaman a `Random::randomfloat()` simultáneamente, todos leen y escriben el mismo `seed` → condición de carrera → resultados incorrectos.

Problema CPU multithread: idéntico. Con `std::thread` o OpenMP también habría race conditions.

**Después (DOP):**

```cpp
// El estado del PRNG es un dato que pertenece a quien lo usa
struct RngState {
    unsigned long seed;

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

    Vector3 randomUnitVector3() { ... }
    // Todos los métodos son non-static: operan sobre this->seed
};
```

Cada work-item / thread crea su propio `RngState` con una semilla derivada de su ID:

```cpp
// Thread CPU:
void renderTile(int tileX, int tileY, unsigned long baseSeed) {
    RngState rng(baseSeed ^ ((tileX * 73856093UL) ^ (tileY * 19349663UL)));
    // rng vive en el stack de este thread — jamás se comparte
    for (int j = ...) for (int i = ...) {
        Ray ray = getRay(i, j, rng);
        ...
    }
}

// Work-item GPU (SYCL):
cgh.parallel_for(..., [=](sycl::item<2> item) {
    RngState rng(baseSeed ^ item.get_linear_id());
    // cada work-item tiene su propio rng en su propio stack
    ...
});
```

**La enseñanza DOP aquí**: el estado no es "global del sistema" — es un **dato** que tiene un dueño claro (el thread/work-item que renderiza ese píxel). Modelarlo como estado global era un error de diseño de OOP clásico, no inevitabilidad del lenguaje.

---

## 6. ¿Cuándo NO usar DOP?

DOP no es una bala de plata. Aplicarlo donde no corresponde complica el código sin beneficio:

| Código | ¿DOP útil? | Motivo |
|---|---|---|
| Bucle de renderizado (millones de rayos) | ✅ Muy útil | Hot path: cache misses cuestan mucho |
| Broad-phase de físicas (miles de AABBs) | ✅ Muy útil | Itera muchos objetos tocando pocos campos |
| Parser de escena (se ejecuta una vez) | ❌ No útil | El coste de parsear es dominado por I/O |
| Sistema de UI y configuración | ❌ No útil | Se ejecuta a <60 Hz, no en hot path |
| Tests unitarios | ❌ No útil | La claridad de código importa más que la velocidad |
| API pública del motor | ❌/⚠️ Parcialmente | Depende de si la API está en el hot path |

**Regla práctica:** perfila primero. Si una función no aparece en el top 10 del profiler, probablemente no vale la pena aplicar DOP. La complejidad de código de DOP (tagged unions, arrays fijos) tiene un coste de mantenimiento real; hay que justificarlo con datos reales de rendimiento.

---

## 7. Resumen: reglas prácticas

Estos son los cambios concretos que transforman código OOP común en código DOP, ordenados de mayor a menor impacto:

### Regla 1: Preferir tipos valor sobre tipos referencia en hot path

```cpp
// ❌ OOP habitual
shared_ptr<Geometry> obj = make_shared<Sphere>(...);
vector<shared_ptr<Geometry>> scene;

// ✅ DOP
GeometryData obj = makeSphere(...);
std::vector<GeometryData> scene;  // el vector SÍ puede ser heap — sus elementos son valor
```

### Regla 2: Eliminar herencia virtual en datos que se iterar frecuentemente

```cpp
// ❌
class Geometry { virtual HitRecord hit(...) const = 0; };
class Sphere : public Geometry { ... };

// ✅
struct GeometryData { GeometryType type; Transform transform; union { ... }; };
HitRecord geometryHit(const GeometryData &g, const Ray &ray, const Limits &limits);
```

### Regla 3: Cero heap allocations en el hot path

```cpp
// ❌ vector<Solution> en hit() — malloc/free por cada rayo
vector<Solution> sols = solver.getSolutions();

// ✅ struct de stack — cero heap
Solutions sols = solveBhaskara(a, b, c);
```

### Regla 4: Separar datos del comportamiento

```cpp
// ❌ El objeto "sabe" calcular su intersección
obj->hit(ray, limits);

// ✅ Una función libre opera sobre datos pasivos
geometryHit(obj, ray, limits);
```

### Regla 5: El estado mutable pertenece a quien lo usa

```cpp
// ❌ Estado global / estático
static unsigned long seed;
static float randomfloat();

// ✅ Estado por instancia, ownership explícito
struct RngState { unsigned long seed; float randomfloat(); };
```

### Regla 6: Buffers contiguos en lugar de árboles de punteros

```cpp
// ❌ Árbol de shared_ptr: puntero → nodo → puntero → dato
vector<shared_ptr<RenderHit>> objects;

// ✅ Buffer plano: dato[0] dato[1] dato[2] ...  (todo contiguo)
std::vector<SceneObject> objects;
```

---

> **Conclusión**: DOP no exige reescribir todo el proyecto ni abandonar C++. Se resume en una idea: **coloca los datos que se usan juntos, juntos en memoria**. Los patrones — tipos valor, tagged unions, arrays fijos, funciones libres — son las herramientas para conseguirlo. Aplicados en los hot paths críticos del ray tracer (el bucle de intersección y el trace de rayos), producen código que el hardware puede ejecutar a su velocidad máxima.
