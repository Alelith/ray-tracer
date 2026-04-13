# 🧠 Guía de documentación con Doxygen para IA

Este proyecto utiliza un sistema de documentación estandarizado basado en **Doxygen**, definido en el archivo `comments_template.yaml`.  
El propósito de esta guía es que los agentes de IA (como **GitHub Copilot**, **ChatGPT**, **Claude**, etc.) entiendan **cuándo, dónde y cómo** aplicar cada tipo de comentario de forma consistente y profesional.

---

## ⚙️ Estructura del sistema

El archivo `comments_template.yaml` define:

- **style:** Reglas generales de formato (indentación, longitud de línea, prefijo de comentario)
- **default_format:** Estructura base de apertura/cierre de bloques `/** ... */`
- **comment blocks:** Plantillas específicas para cada tipo de entidad con sus respectivas variables `${...}`

### Reglas de estilo obligatorias:

```yaml
style: 
  indent: 1              # Nivel de indentación
  line_length: 80        # Longitud máxima de línea
  prefix: " * "          # Prefijo para cada línea de comentario
```

Todos los comentarios deben:
- Ser **compatibles con Doxygen**
- Respetar la sintaxis establecida en el YAML
- Usar **inglés técnico formal y claro**
- Mantener formato consistente entre todos los archivos

---

## 🧩 Tipos de comentarios y su aplicación

### 1. **mainpage_comment** - Página principal de documentación

**Ubicación:** Al **inicio del header principal** del proyecto (generalmente `project.h` o archivo similar)

**Propósito:** Proporciona la página de entrada a toda la documentación Doxygen generada.

**Cuándo usar:**
- Solo UNA vez en todo el proyecto
- En el archivo header principal que define la interfaz del proyecto
- Antes de cualquier otro código o comentario

**Variables a completar:**
- `${project_name}`: Nombre del proyecto
- `${introduction_text}`: Introducción general (2-4 líneas)
- `${features_description}`: Lista de características principales
- `${architecture_description}`: Descripción de la arquitectura del sistema
- `${module_overview_ref}`: Referencia al módulo principal
- `${implementation_details}`: Detalles de implementación importantes
- `${dependencies_description}`: Dependencias externas requeridas
- `${installation_instructions}`: Pasos de instalación
- `${usage_information}`: Información básica de uso
- `${contribution_guidelines}`: Guías para contribuir (si aplica)
- `${license_information}`: Tipo de licencia del proyecto

**Ejemplo de aplicación:**
```c
/**
 * @mainpage MyProject Documentation
 * 
 * @section intro_sec Introduction
 * This is a high-performance data processing library designed for...
 * 
 * @section features_sec Features
 * - Fast data processing
 * - Memory efficient
 * - Thread-safe operations
 * ...
 */
```

---

### 2. **file_comment** - Comentario de archivo

**Ubicación:** Al **principio de CADA archivo** `.h` o `.c`/`.cpp`

**Propósito:** Describe el propósito del archivo en el contexto del proyecto.

**Cuándo usar:**
- En TODOS los archivos del proyecto (headers e implementaciones)
- Inmediatamente después del `mainpage_comment` si está en el mismo archivo
- Antes de cualquier código o include

**Variables a completar:**
- `${file_name}`: Nombre del archivo (con extensión)
- `${file_purpose}`: Propósito específico del archivo
- `${author}`: Nombre del autor
- `${date}`: Fecha de creación
- `${optional_note}`: Notas adicionales (opcional)
- `${optional_warning}`: Advertencias importantes (opcional)
- `${optional_todo}`: Tareas pendientes (opcional)

**Ejemplo:**
```c
/**
 * @file graphics_engine.c
 * @brief Graphics rendering engine implementation
 * 
 * @author Lilith Estévez Boeta
 * @date 2025-11-11
 * 
 * @note Requires OpenGL 4.5 or higher
 */
```

---

### 3. **module_comment** - Agrupación de funcionalidades

**Ubicación:** En **archivos header (`.h`)** para agrupar funciones relacionadas

**Propósito:** Organiza la documentación en módulos/grupos lógicos usando `@defgroup`.

**Cuándo usar:**
- Para agrupar funciones relacionadas (ej: funciones gráficas, matemáticas, I/O)
- En headers, antes del primer elemento del grupo
- Una vez por cada grupo de funcionalidades

**Variables a completar:**
- `${module_name}`: Identificador único del módulo (sin espacios)
- `${module_title}`: Título descriptivo del módulo
- `${brief_description}`: Descripción breve (1 línea)
- `${detailed_description}`: Descripción detallada del módulo
- `${module_features_description}`: Características del módulo
- `${module_usage_information}`: Información de uso del módulo
- `${optional_note}`: Notas adicionales
- `${optional_warning}`: Advertencias
- `${optional_todo}`: Tareas pendientes

**Ejemplo:**
```c
/**
 * @defgroup graphics_module Graphics Module
 * @brief Graphics rendering and manipulation functions
 * 
 * @details This module provides all graphics-related functionality
 * including rendering, transformations, and visual effects.
 * 
 * @section module_features_sec Features
 * - 2D and 3D rendering
 * - Texture management
 * - Shader support
 * 
 * @section module_usage_sec Usage
 * Initialize the graphics system before using any function...
 */
```

---

### 4. **define_comment** - Documentación de macros

**Ubicación:** Justo **encima de cada macro `#define`**

**Propósito:** Explica qué representa la macro y cómo se usa.

**Cuándo usar:**
- Para TODAS las macros públicas
- Para macros complejas o no obvias (incluso privadas)

**Variables a completar:**
- `${define_name}`: Nombre de la macro
- `${brief_description}`: Descripción breve
- `${detailed_description}`: Explicación detallada
- `${module_name}`: Módulo al que pertenece
- `${optional_note}`: Notas de uso
- `${optional_warning}`: Advertencias sobre uso incorrecto
- `${optional_todo}`: Mejoras futuras

**Ejemplo:**
```c
/**
 * @def MAX_BUFFER_SIZE
 * @brief Maximum size of the internal buffer
 * 
 * @details This value defines the maximum number of elements
 * that can be stored in the internal buffer before overflow.
 * 
 * @ingroup memory_module
 * 
 * @warning Exceeding this value will cause undefined behavior
 */
#define MAX_BUFFER_SIZE 1024
```

---

### 5. **function_comment** - Documentación de funciones

**Ubicación:** Justo **encima de las funciones en los archivos de implementación (`.c`/`.cpp`)**

**Propósito:** Documenta el comportamiento, parámetros y valores de retorno de funciones.

**Cuándo usar:**
- Para TODAS las funciones públicas
- Para funciones privadas complejas
- **SOLO en archivos de implementación**, NO en headers

**Variables a completar:**
- `${brief_description}`: Descripción breve (1 línea)
- `${detailed_description}`: Descripción detallada del comportamiento
- `${module_name}`: Módulo al que pertenece
- `${param_name}`: Nombre del parámetro de entrada
- `${param_description}`: Descripción del parámetro
- `${param_output_name}`: Nombre del parámetro de salida (si aplica)
- `${param_output_description}`: Descripción del parámetro de salida
- `${return_description}`: Descripción general del retorno
- `${return_value}`: Valor específico de retorno
- `${return_value_description}`: Descripción del valor de retorno específico
- `${optional_note}`: Notas adicionales
- `${optional_warning}`: Advertencias
- `${optional_todo}`: Mejoras pendientes

**Directivas de parámetros:**
- `@param[in]`: Parámetro de entrada (solo lectura)
- `@param[out]`: Parámetro de salida (la función lo modifica)
- `@param[in,out]`: Parámetro de entrada/salida

**Ejemplo:**
```c
/**
 * @brief Calculates the sum of two integers
 * 
 * @details This function performs addition of two signed integers
 * with overflow detection. Returns error code if overflow occurs.
 * 
 * @ingroup math_module
 * 
 * @param[in] a First operand
 * @param[in] b Second operand
 * @param[out] result Pointer to store the result
 * @return Operation status code
 * @retval 0 Success
 * @retval -1 Overflow detected
 * @retval -2 Invalid pointer
 * 
 * @warning The result pointer must be valid
 */
int safe_add(int a, int b, int *result) {
    // Implementation...
}
```

---

### 6. **struct_comment** - Documentación de estructuras

**Ubicación:** Justo **encima de la declaración de `struct`**

**Propósito:** Explica el propósito de la estructura y su contexto.

**Cuándo usar:**
- Para TODAS las estructuras públicas
- Para estructuras privadas complejas
- Cada campo interno debe tener comentario `///< descripción`

**Variables a completar:**
- `${struct_name}`: Nombre de la estructura
- `${brief_description}`: Descripción breve
- `${detailed_description}`: Descripción detallada
- `${optional_note}`: Notas
- `${optional_warning}`: Advertencias
- `${optional_todo}`: Mejoras futuras

**Ejemplo:**
```c
/**
 * @struct Point2D
 * @brief Represents a 2D point in Cartesian coordinates
 * 
 * This structure stores the x and y coordinates of a point
 * in a 2D plane using floating-point precision.
 * 
 * @note All coordinates are in pixels by default
 */
typedef struct {
    float x;  ///< Horizontal coordinate
    float y;  ///< Vertical coordinate
} Point2D;
```

---

### 7. **enum_comment** - Documentación de enumeraciones

**Ubicación:** Justo **encima de la declaración de `enum`**

**Propósito:** Explica el propósito del enum y sus valores.

**Cuándo usar:**
- Para TODOS los enums públicos
- Cada valor debe tener comentario `///< descripción`

**Variables a completar:**
- `${enum_name}`: Nombre del enum
- `${brief_description}`: Descripción breve
- `${detailed_description}`: Descripción detallada
- `${optional_note}`: Notas
- `${optional_warning}`: Advertencias
- `${optional_todo}`: Mejoras futuras

**Ejemplo:**
```c
/**
 * @enum ErrorCode
 * @brief Error codes returned by API functions
 * 
 * This enumeration defines all possible error codes that
 * can be returned by the library functions.
 * 
 * @note Error codes are negative values
 */
typedef enum {
    ERROR_NONE = 0,        ///< No error occurred
    ERROR_INVALID_ARG = -1, ///< Invalid argument provided
    ERROR_OUT_OF_MEMORY = -2, ///< Memory allocation failed
    ERROR_FILE_NOT_FOUND = -3 ///< Requested file not found
} ErrorCode;
```

---

### 8. **class_comment** - Documentación de clases (C++/OOP)

**Ubicación:** Justo **encima de la declaración de clase**

**Propósito:** Documenta atributos, métodos y propósito de la clase.

**Cuándo usar:**
- Para TODAS las clases públicas en C++
- Para clases privadas complejas

**Variables a completar:**
- `${class_name}`: Nombre de la clase
- `${brief_description}`: Descripción breve
- `${detailed_description}`: Descripción detallada
- `${optional_note}`: Notas
- `${optional_warning}`: Advertencias
- `${optional_todo}`: Mejoras futuras

**Ejemplo:**
```cpp
/**
 * @class DataProcessor
 * @brief Main data processing class
 * 
 * This class provides methods for loading, processing,
 * and exporting data in various formats.
 * 
 * @note Thread-safe for concurrent read operations
 * @warning Write operations must be synchronized externally
 */
class DataProcessor {
    // Implementation...
};
```

---

## 🎯 Reglas de aplicación

### **Obligatorias:**

1. ✅ **Todos los comentarios en inglés técnico formal**
2. ✅ **Respetar el formato Doxygen** (`/** ... */`)
3. ✅ **Seguir el estilo del YAML** (indent, line_length, prefix)
4. ✅ **Reemplazar TODAS las variables `${...}`** con contenido real
5. ✅ **NO dejar secciones opcionales vacías** - eliminarlas si no aplican
6. ✅ **Mantener longitud de línea ≤ 80 caracteres**

### **Prohibidas:**

1. ❌ **NO duplicar documentación entre header e implementación**
   - Headers: `module_comment`, `struct_comment`, `enum_comment`, `define_comment` (macros públicas)
   - Implementación: `function_comment`, `define_comment` (macros privadas)

2. ❌ **NO usar comentarios de estilo C++ (`//`)** para documentación Doxygen

3. ❌ **NO mezclar idiomas** (español/inglés)

4. ❌ **NO incluir información redundante** o que el código ya expresa claramente

5. ❌ **NO dejar placeholders `${...}`** sin reemplazar

---

## 🤖 Prompt recomendado para IA

Cuando quieras documentar código con Doxygen, usa este prompt:

```
Documenta este código siguiendo el sistema Doxygen definido en comments_template.yaml y las reglas de docs-guide.md.

Tareas específicas:
1. Analiza el código para identificar: funciones, estructuras, enums, macros, módulos
2. Aplica el tipo de comentario apropiado según la entidad (usa la tabla de tipos)
3. Reemplaza TODAS las variables ${...} con descripciones técnicas claras
4. Usa inglés formal y técnico
5. Respeta el estilo: indent=1, line_length=80, prefix=" * "
6. Organiza funciones en módulos lógicos (@defgroup, @ingroup)
7. Documenta parámetros con @param[in/out], retornos con @return/@retval
8. Elimina secciones opcionales (@note, @warning, @todo) si no aplican
9. Asegúrate de que cada struct/enum tenga comentarios inline (///<) en sus campos

NO documentes código obvio o trivial.
NO dupliques documentación entre headers e implementación.
NO dejes variables ${...} sin reemplazar.
NO uses español.
```

---

## ✅ Checklist de validación

Antes de considerar la documentación completa, verificar:

- [ ] Existe `mainpage_comment` en el header principal
- [ ] Todos los archivos tienen `file_comment` al inicio
- [ ] Funciones relacionadas están agrupadas en módulos (`@defgroup`)
- [ ] Todas las funciones públicas tienen `function_comment` en implementación
- [ ] Todas las estructuras tienen `struct_comment` y comentarios inline
- [ ] Todos los enums tienen `enum_comment` y comentarios inline
- [ ] Macros públicas están documentadas con `define_comment`
- [ ] No quedan variables `${...}` sin reemplazar
- [ ] Secciones opcionales vacías fueron eliminadas
- [ ] Todo está en inglés técnico formal
- [ ] Longitud de línea ≤ 80 caracteres
- [ ] Formato consistente con el YAML (indent, prefix)
- [ ] Parámetros documentados con dirección correcta [in/out]
- [ ] Valores de retorno documentados con @return/@retval

---

## 🚨 Errores comunes a evitar

1. ❌ Dejar variables `${...}` sin reemplazar
2. ❌ Documentar funciones en headers en lugar de implementación
3. ❌ Usar español en lugar de inglés
4. ❌ Incluir secciones opcionales vacías (@note, @warning sin contenido)
5. ❌ No usar `@ingroup` para asociar funciones a módulos
6. ❌ Olvidar comentarios inline (`///<`) en campos de struct/enum
7. ❌ Usar comentarios de estilo C++ (`//`) para documentación
8. ❌ Exceder 80 caracteres por línea
9. ❌ No especificar dirección de parámetros [in/out]
10. ❌ Documentar código obvio o auto-explicativo

---

## �� Resumen de ubicaciones

| Tipo de Comentario | Archivo | Ubicación |
|-------------------|---------|-----------|
| `mainpage_comment` | Header principal | Al inicio, una sola vez |
| `file_comment` | Todos | Primera línea del archivo |
| `module_comment` | Headers | Antes del primer elemento del grupo |
| `define_comment` | Headers/Source | Encima de cada macro |
| `function_comment` | **Solo Source** | Encima de cada función |
| `struct_comment` | Headers | Encima de cada struct |
| `enum_comment` | Headers | Encima de cada enum |
| `class_comment` | Headers (C++) | Encima de cada clase |

---

## 📝 Notas finales

- Esta guía y el YAML están diseñados para mantener documentación consistente entre proyectos
- La IA debe priorizar claridad y utilidad sobre exhaustividad
- En caso de duda, es mejor omitir secciones opcionales que incluir información inútil
- La documentación debe ser técnicamente precisa y profesional
- Usar siempre inglés como idioma estándar
- La documentación debe generarse automáticamente con `doxygen` sin errores
