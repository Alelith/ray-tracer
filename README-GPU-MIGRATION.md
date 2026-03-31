# Migración de Ray Tracer a GPU (CUDA)

Este documento detalla el análisis de la arquitectura actual del ray tracer para su paralelización en GPU, señalando los puntos fuertes y los pequeños cambios que hay que realizar garantizando **la máxima conservación del código actual de render y matemáticas**.

## 1. Análisis de Eficiencia y Paralelización

### 🟢 Lo que está PERFECTO (No hay que borrar este código)
1. **Ausencia de Polimorfismo y Funciones Virtuales**: La jerarquía `WorldGeometries` que agrupa toda la data en tipos primitivos y listas (como los sub-structs de `SimpleGeometryData` con `vector<Matrix4x4>`, `vector<AABB>`) y el identificador `GeomType` permiten evitar bifurcaciones del hilo de la GPU (divergencia de *Warps*), que es el asesino número uno de la eficiencia en GPU.
2. **RayTracer Iterativo, No Recursivo**: Tu función `Camera::rayColor()` utiliza un loop definido paso a paso (`for (int i = 0; i < depth; i++)`) sin mandarse funciones recursivas. La GPU tiene una pila minúscula (apenas kb por hilo). Si fuera recursivo provocaría un *Stack Overflow*. **¡Esto se puede ir directo al kernel!**
3. **Optimización con Bounding Volume Hierarchy**: Contar con un `LinearBVH` disminuye la complejidad del trazado drásticamente. Construirlo en CPU como haces y mandar el array lineal flat (`bvhData`) es la táctica predilecta de la industria.

### 🔴 Lo que fallaría en GPU (Refactors necesarios)
1. **Allocaciones dinámicas (`std::vector` y `new`) en tiempo de ejecución**:
   Se usa `std::vector` para pasar los datos de geometría y materiales al proceso matemático de render (Ej: `LinearBVH` contiene un `vector<BVHData>`). La memoria de la GPU funciona diferente y los sub-hilos matemáticos no deben reservar datos de tamaño variable.
2. **El búfer de render se envía directamente a `std::cout`**:
   Las funciones `Camera::render()` y `writeColor()` meten la info de los pixeles directamente en salida del sistema (iostream) uno a uno, mientras computa. Un programa de GPU hace *miles* de píxeles a la vez en paralelo pero no puede imprimir en consola como la CPU.
3. **`std::stack` dentro del `LinearBVH::hit`**:
   Línea ~173 de `LinearBVH.cpp`: utilizas un `stack<int> nodesToVisit` para viajar por el árbol. ¡Esto genera memoria dinámica en vivo (allocates the heap)! La memoria paralela colapsaría en GPU. Hay que pasar este `stack` a un arreglo estático.
4. **Sistema de Randomización (PRNG)**:
   Se utiliza `Random::global()` internamente en CPU. Necesitamos una versión que instancie estados aleatorios por cada hilo renderizador individualmente en la GPU.
5. **No hay decoradores de GPU en las funciones base matemáticas**:
   La GPU no llamaría a los métodos (como las multiplicaciones entre `Vector3` y `Matrix4x4`) al compilar, porque hay que indicar a nivel compilador que el código corre tanto en el procesador (Host) como en GPU (Device), con `__host__ __device__`.

---

## 2. Guía paso a paso para la Migración

La migración se debería realizar idealmente a nivel **CUDA** (NVidia), ya que las herramientas son directas para código basado en memoria e interfaces nativas de `C++` como tu proyecto.

### Paso A: Decorador de funciones para Matemáticas y Lógica (`__host__ __device__`)
Tendremos que definir un macro universal para que no rompas ni tu CPU host build ni tu futuro GPU build.
En algún archivo global (o mediante flags en el Makefile) crear algo similar a esto:

```cpp
// En algún global.hpp o similar:
#ifdef __CUDACC__
#  define RT_CALLABLE __host__ __device__
#else
#  define RT_CALLABLE
#endif
```

Luego, esparces `RT_CALLABLE` en frente de TODAS tus funciones de `maths/` y `geometry/` (inclusive constructores de vec3, limit, transform, hit...). Esto le dice al compilador de nVidia que te recompile el mismo código actual tanto para CPU y GPU. Ninguna matemática será eliminada, solo etiquetada.

### Paso B: Remover el `std::stack` de la función de impacto `LinearBVH::hit`
Tienes que cambiar el stack variable por un stack fijo (un arreglo simple en memoria de pila en el entorno). ¡Un árbol BVH de 64 niveles puede contener todos los modelos 3D modernos!.

**El cambio en código:**
```cpp
// src/geometry/LinearBVH.cpp: hit(...)
RT_CALLABLE bool LinearBVH::hit(const Ray &ray, const Limits &limits, HitRecord &record) const {
    bool hitAnything = false;
    Limits currentLimits = limits;
    
    int nodesToVisit[64]; // Reemplaza std::stack
    int stackPtr = 0;
    
    nodesToVisit[stackPtr++] = 0; // root node
    
    while (stackPtr > 0) {
        int currentIdx = nodesToVisit[--stackPtr]; // equivalente al .top() y .pop()
        
        const BVHData &currentNode = bvhData[currentIdx];
        if (currentNode.box.hit(ray, currentLimits)) {
            if (currentNode.geomIdx != -1) {
                //... Lógica matemática igual
            } else {
                nodesToVisit[stackPtr++] = currentNode.rightChildIdx;
                nodesToVisit[stackPtr++] = currentIdx + 1;
            }
        }
    }
    return hitAnything;
}
```

### Paso C: Adaptar el Framebuffer (Buffer de Pantalla) y el Loop a un Macro-Kernel de CUDA
Desacopla el bucle `for` de la cámara actual hacia un *kernel* propio de Cuda (`__global__ void render_kernel()`). En vez de imprimir a `std::cout`, los hilos escribirán un color completo.

* Tu función ahora calculará su posición basándose en su ID de sub-proceso, calculará todo **tu mismo código de matemáticas** y dejará su color puro en la memoria RAM compartida: `frameBuffer[idx] = pixelColor;`
* Una vez que terminen *todos* los hilos, se recoge la memoria de vuelta en tu C++ estándar (el CPU) y lo envuelves directo a tu salida PPM/std::cout secuencial unificada sin dañar nada.

### Paso D: Punteros Crudos (Raw pointers) en vez de `std::vector`
Al instanciar el kernel, enviar por medio los `WorldGeometries` es imposible mediante `std::vector`. 
Deberás construir un *Struct* global puramente de punteros y pasar su locación al hilo.

Ejemplo del paso `materials`:
```cpp
// CPU (Preparación de paso a GPU usando raw arrays)
int materialsCount = static_cast<int>(materials.size());
Material* device_materials;
cudaMalloc((void**)&device_materials, materialsCount * sizeof(Material));
cudaMemcpy(device_materials, materials.data(), materialsCount * sizeof(Material), cudaMemcpyHostToDevice);

// En la función GPU Kernel (...), se usa the Raw Array en vez del vector original
```
¡Todos tus accesos a datos mediante corchetes `[]` seguirán funcionando como si nada hubiese modificado la base!

### Paso E: Modificar el sistema Random al API curandState (Solo GPU)
La función interior `Camera::rayColor` necesitará tomar un `curandState* local_rand_state` que la GPU inicializa. 
Recomendaría reemplazar todas las llamadas atómicas de `Random::global()` por pasar temporalmente en firma el `local_rand_state` por referencia hasta el bucle principal.