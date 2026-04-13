# 🧠 Guía para generación y modificación de RELEASE.md con IA

Este proyecto utiliza una plantilla estándar (`RELEASE_TEMPLATE.md`) para generar o actualizar archivos de release notes.  
El objetivo de esta guía es indicar a los agentes de IA (como **GitHub Copilot**, **ChatGPT**, **Claude**, etc.) cómo usar correctamente la plantilla para crear documentación profesional de releases, qué secciones conservar o suprimir y qué tono o formato seguir.

---

## ⚙️ Reglas generales

1. **Usar siempre `RELEASE_TEMPLATE.md` como referencia base.**  
   Cada sección debe conservar su título, icono y jerarquía Markdown exactamente como aparece en la plantilla.

2. **Personalizar TODOS los campos entre corchetes `[ ... ]`** según la información real del release.  
   - Reemplazar `[Project Name]` con el nombre real del proyecto
   - Reemplazar `[X.Y.Z]` con el número de versión real (seguir Semantic Versioning)
   - Reemplazar `[username]` con el username de GitHub del autor
   - Reemplazar `[repo_name]` con el nombre del repositorio
   - Completar badges con información real del release
   - NO dejar ningún placeholder sin reemplazar

3. **Eliminar secciones vacías o no aplicables al tipo de proyecto.**  
   - Diferentes tipos de proyectos necesitan diferentes secciones
   - No todos los proyectos tienen ejecutables, APIs, o componentes visuales
   - Adaptar el contenido al contexto específico del release

4. **Detectar automáticamente información del proyecto y release:**
   - Analizar el changelog o commits desde la última versión
   - Examinar archivos modificados para identificar cambios principales
   - Detectar el tipo de proyecto (librería, API, ejecutable, aplicación, etc.)
   - Identificar breaking changes en la documentación o commits
   - Calcular métricas del proyecto (líneas de código, cobertura de tests, etc.)

5. **Mantener coherencia de estilo:**
   - Lenguaje: **Inglés técnico formal y claro**
   - Mantener TODOS los emojis e iconos (🚀, 📦, ✨, etc.) tal como aparecen en la plantilla
   - Conservar los separadores `---` entre secciones
   - Usar formato Markdown correcto para código, enlaces y listas
   - Seguir Semantic Versioning (MAJOR.MINOR.PATCH)

6. **Información de versión:**
   - MAJOR: Cambios incompatibles con versiones anteriores (breaking changes)
   - MINOR: Nueva funcionalidad compatible con versiones anteriores
   - PATCH: Correcciones de bugs compatibles con versiones anteriores
   - Fecha en formato ISO: YYYY-MM-DD

7. **Priorizar información útil para usuarios:**
   - Destacar nuevas funcionalidades y mejoras
   - Documentar breaking changes con ejemplos de migración
   - Incluir instrucciones claras de instalación y uso
   - Proporcionar ejemplos funcionales y probados

---

## 🧩 Estructura general esperada

El archivo de release debe seguir el orden y estilo de las siguientes secciones (todas definidas en `RELEASE_TEMPLATE.md`):

| Sección | Descripción | Obligatoria | Notas |
|----------|--------------|--------------|--------|
| **Title and Badges** | Nombre del proyecto + versión y badges de release, lenguaje, estándar, licencia | ✅ Sí | Usar versión específica en todos los badges. Detectar info del proyecto. |
| **📋 Description** | Resumen del proyecto y qué introduce este release | ✅ Sí | 2-4 líneas. Primera línea: qué es el proyecto. Segunda línea: qué trae esta versión. |
| **📦 Release Contents** | Archivos incluidos en el release y opcionales | ✅ Sí | Adaptar según tipo de proyecto: binarios, paquetes, código fuente, assets, etc. |
| **✨ Implemented Features** | Lista organizada de funcionalidades por módulos | ✅ Sí | Agrupar por componentes/módulos. Incluir paths. Describir cada función/clase/endpoint. |
| **📥 Installation and Usage** | Múltiples opciones de instalación y requisitos | ✅ Sí | Incluir: descarga directa, compilación, package manager. Requisitos del sistema detallados. |
| **🧪 Quick Usage Example** | Ejemplos de uso común del proyecto | ✅ Sí | 3-5 ejemplos prácticos. Comandos o código funcional. Controles interactivos si aplica. |
| **🔧 System Requirements** | Tabla de requisitos técnicos del sistema | ✅ Sí | OS, runtime/compiler, arquitectura, dependencias, recursos mínimos/recomendados. |
| **📚 Documentation** | Enlaces a documentación y cómo generarla | ✅ Sí | Docs online, README, código fuente. Comando para generar docs localmente. |
| **🐛 Known Issues** | Problemas conocidos del release | ⚙️ Opcional | Solo si existen. Describir impacto y workarounds. Enlazar a issues. |
| **🔜 Roadmap for v[Next]** | Funcionalidades planeadas para próxima versión | ⚙️ Opcional | Lista de checkbox con mejoras futuras. Siguiente versión MINOR o MAJOR. |
| **📝 Changelog** | Changelog detallado siguiendo Keep a Changelog | ✅ Sí | Secciones: Added, Changed, Fixed, Deprecated, Removed, Security, Technical Highlights. |
| **🔄 Breaking Changes** | Cambios incompatibles con versiones anteriores | ⚠️ Condicional | OBLIGATORIO si hay breaking changes. Incluir guía de migración con ejemplos before/after. |
| **📊 Project Statistics** | Métricas del proyecto | ⚙️ Opcional | Líneas de código, módulos, funciones, tiempo de desarrollo, cobertura de tests, etc. |
| **🎯 Performance Benchmarks** | Benchmarks de rendimiento | ⚙️ Opcional | Solo para proyectos donde el performance es crítico. Incluir métricas comparativas. |
| **👩‍💻 Author** | Información del autor con enlaces | ✅ Sí | Mantener datos de la plantilla o personalizar según se indique. |
| **🙏 Acknowledgments** | Agradecimientos y referencias | ⚙️ Opcional | Reconocer contribuidores, librerías, inspiración. Incluir referencias técnicas. |
| **📜 License** | Licencia del proyecto con requisitos clave | ✅ Sí | Nombre completo de licencia. Resumir requisitos principales. Enlace al archivo LICENSE. |
| **Footer** | Call-to-action con enlaces | ✅ Sí | Star en GitHub, reportar bugs, solicitar features. |

---

## 🎯 Instrucciones específicas por sección

### **Title and Badges**
- Formato: `# 🚀 [Project Name] v[X.Y.Z]`
- Badge de Release: usar versión exacta (v1.0.0, v2.3.1, etc.)
- Badge de Language: lenguaje principal del proyecto
- Badge de Standard/Version: estándar del lenguaje o versión del framework
- Badge de License: detectar del archivo LICENSE
- Todos los badges deben ser consistentes con la versión específica

### **📋 Description**
- **Párrafo 1:** ¿Qué es el proyecto? Propósito y características principales.
- **Párrafo 2:** ¿Qué introduce este release específico? Destacar 2-4 mejoras clave.
- Máximo 4 líneas total
- Tono técnico pero accesible

### **📦 Release Contents**
- **Included Files:** Archivos esenciales del release
  - Ejecutables/binarios con descripción
  - Código fuente con métricas (líneas, módulos)
  - Archivos de configuración o assets
  - Build files (Makefile, package.json, etc.)
  - Documentación estándar (README, LICENSE)
- **Optional:** Elementos generables o secundarios
  - Documentación generada (Doxygen, Sphinx, JSDoc)
  - Tests y cobertura
  - Ejemplos y demos
- Adaptar según tipo de proyecto:
  - **Librería:** paquetes, headers, bindings
  - **API:** servidor, cliente, specs (OpenAPI/Swagger)
  - **Ejecutable:** binarios por plataforma, assets
  - **Aplicación:** instaladores, assets, configs

### **✨ Implemented Features**
- Organizar por módulos/componentes con paths relativos
- Formato: `[item]()` — Descripción clara y concisa
- Usar nomenclatura técnica correcta:
  - Funciones: `function_name()`
  - Clases: `ClassName`
  - Endpoints: `GET /api/resource`
  - Comandos: `command-name`
- Describir qué hace cada elemento, no solo su nombre
- Agrupar lógicamente (no mezclar frontend con backend, utils con core, etc.)

### **📥 Installation and Usage**
- **Siempre incluir múltiples opciones cuando sea posible:**
  1. **Direct Download:** Para binarios/ejecutables
  2. **Compile from Source:** Para proyectos open source
  3. **Package Manager:** npm, pip, cargo, maven, etc.
- **System Requirements:** Especificar versiones mínimas exactas
- Comandos deben ser copy-paste ready
- Incluir pasos de verificación después de instalación

### **🧪 Quick Usage Example**
- Mínimo 3 ejemplos de casos de uso comunes
- Código o comandos funcionales y probados
- Incluir parámetros y opciones relevantes
- Si es interactivo, documentar controles/comandos
- Formato:
  ```bash
  comando --opcion valor
  ```
  o
  ```[language]
  código de ejemplo
  ```

### **🔧 System Requirements**
- Tabla obligatoria con:
  - **OS:** Plataformas soportadas (primary/compatible/experimental)
  - **Runtime/Compiler:** Versiones exactas requeridas
  - **Architecture:** x86_64, ARM64, etc.
  - **Dependencies:** Librerías y herramientas necesarias
  - **Resources:** RAM mínima/recomendada, disco, etc.
  - Otros requisitos específicos (GPU, network, etc.)

### **📚 Documentation**
- Enlaces a documentación online (si existe)
- Enlace al README del proyecto
- Enlace al repositorio de código
- Comando para generar documentación localmente
- Especificar qué incluye la documentación

### **🐛 Known Issues**
- Solo incluir si existen issues conocidos
- Formato: `**Título del Issue**: Descripción detallada, impacto, workarounds.`
- Enlazar a GitHub Issues si están trackeados
- Indicar si el issue será corregido en próxima versión
- Si no hay issues, usar frase estándar: "No other critical bugs are currently known."

### **🔜 Roadmap**
- Versión objetivo debe ser MINOR o MAJOR siguiente
- Lista de checkbox `- [ ]` con features planeadas
- Agrupar por categorías si son muchas:
  - Performance
  - Features
  - Platform support
  - Developer experience
- No comprometerse a fechas específicas

### **📝 Changelog**
- **CRÍTICO:** Seguir formato Keep a Changelog
- Secciones en orden:
  1. **Added:** Nuevas funcionalidades
  2. **Changed:** Cambios en funcionalidad existente
  3. **Fixed:** Correcciones de bugs
  4. **Deprecated:** Funcionalidad que será removida
  5. **Removed:** Funcionalidad eliminada en esta versión
  6. **Security:** Fixes de seguridad
  7. **Technical Highlights:** Detalles técnicos relevantes
- Cada item debe ser una línea clara y concisa
- Priorizar información útil para usuarios sobre detalles de implementación

### **🔄 Breaking Changes**
- **OBLIGATORIO si hay breaking changes en el release**
- **Eliminar sección completa si NO hay breaking changes**
- Para cada breaking change:
  - Título descriptivo
  - **What changed:** Qué cambió exactamente
  - **Migration guide:** Ejemplos before/after con código
  - **Why this change:** Justificación técnica
- Los breaking changes justifican un MAJOR version bump

### **📊 Project Statistics**
- Métricas cuantificables del proyecto
- Incluir siempre que sea posible:
  - Lines of Code (total)
  - Número de módulos/archivos
  - Funciones/Clases/Endpoints documentados
  - Tiempo de desarrollo
  - Cobertura de documentación
  - Cobertura de tests
- Adaptar métricas al tipo de proyecto

### **🎯 Performance Benchmarks**
- Solo para proyectos donde performance es crítico
- Tabla con escenarios, métricas y resultados
- Incluir unidades claras (ms, MB/s, requests/s, etc.)
- Comparar con versión anterior si es posible
- Especificar condiciones del benchmark (hardware, configuración)

### **👩‍💻 Author**
- Datos por defecto: Lilith Estévez Boeta
- Solo modificar si se indica explícitamente
- Mantener formato de enlaces (GitHub, LinkedIn)

### **🙏 Acknowledgments**
- Reconocer personas, teams, organizaciones que contribuyeron
- Mencionar librerías/frameworks principales usados
- Agradecer a la comunidad si aplica
- Incluir referencias técnicas relevantes (papers, RFCs, especificaciones)

### **📜 License**
- Nombre completo de la licencia (GNU General Public License v3, MIT License, Apache License 2.0, etc.)
- **Key Requirements:** Resumir 3-5 requisitos principales de la licencia
- Enlace al archivo LICENSE en el repositorio
- Detectar del archivo LICENSE del proyecto

### **Footer**
- Call to action para dar star en GitHub
- Enlaces a Report Bug y Request Feature (GitHub Issues)
- Usar formato HTML centrado como en la plantilla

---

## 📋 Adaptaciones por tipo de proyecto

### 🔧 Ejecutable / CLI Tool
- **Enfatizar:** Installation options, Quick Usage Examples, System Requirements
- **Incluir:** Comandos con todas las flags y opciones
- **Release Contents:** Binarios por plataforma, assets si los hay

### 📚 Librería / Package
- **Enfatizar:** Installation (package managers), API documentation, Usage Examples
- **Incluir:** Ejemplos de código de integración, breaking changes
- **Release Contents:** Paquetes distribuibles, headers/interfaces

### 🌐 API / Web Service
- **Enfatizar:** Main Endpoints, Installation (deployment), System Requirements
- **Incluir:** Tabla de endpoints con métodos HTTP, ejemplos de requests/responses
- **Release Contents:** Servidor, cliente SDK, OpenAPI spec

### 🎮 Aplicación / GUI
- **Enfatizar:** Screenshots/Demo, Installation, Interactive Features
- **Incluir:** Capturas de pantalla o GIFs, controles de usuario
- **Release Contents:** Instaladores por plataforma, assets gráficos

### 🔌 Plugin / Extension
- **Enfatizar:** Installation (en plataforma host), Compatibility, Usage Examples
- **Incluir:** Versiones soportadas de la plataforma host
- **Release Contents:** Package del plugin, assets, configs

---

## 🤖 Prompt recomendado para IA

Cuando quieras generar o actualizar release notes, usa este prompt:

```
Genera/Actualiza el archivo de release notes para la versión [X.Y.Z] de este proyecto usando RELEASE_TEMPLATE.md como plantilla base y siguiendo estrictamente las reglas de release-guide.md.

Información del release:
- Versión: [X.Y.Z]
- Fecha: [YYYY-MM-DD]
- Tipo: [Major/Minor/Patch]
- Breaking changes: [Sí/No]

Tareas específicas:
1. Analiza los cambios desde la última versión (commits, PRs, issues cerrados)
2. Reemplaza TODOS los placeholders [entre corchetes] con información real
3. Detecta automáticamente: tipo de proyecto, lenguaje, dependencias, licencia
4. Genera el changelog categorizado (Added, Changed, Fixed, etc.)
5. Si hay breaking changes, crea la sección con guías de migración
6. Calcula métricas del proyecto (líneas de código, módulos, cobertura)
7. Adapta las secciones al tipo de proyecto (librería/API/ejecutable/etc.)
8. Elimina secciones no aplicables
9. Genera ejemplos de uso funcionales y probados
10. Verifica que todos los enlaces y comandos sean correctos

Tipo de proyecto: [Librería/API/Ejecutable/Aplicación/Plugin/Otro]

NO incluyas secciones vacías o con solo placeholders.
NO agregues secciones que no estén en la plantilla.
SI hay breaking changes, documéntalos exhaustivamente con ejemplos.
Prioriza información útil para usuarios sobre detalles de implementación.
```

---

## ✅ Checklist de validación

Antes de considerar el release notes completo, verificar:

### Información Básica
- [ ] Título con nombre del proyecto y versión exacta (sin `[Project Name]` ni `[X.Y.Z]`)
- [ ] Badges actualizados con información real de esta versión
- [ ] Descripción clara: qué es el proyecto + qué trae esta versión
- [ ] Versión sigue Semantic Versioning (MAJOR.MINOR.PATCH)
- [ ] Fecha en formato ISO (YYYY-MM-DD)

### Contenido del Release
- [ ] Release Contents adaptado al tipo de proyecto
- [ ] Implemented Features organizado por módulos con paths
- [ ] Todas las funciones/clases/endpoints descritas claramente
- [ ] Comandos de instalación probados y funcionales
- [ ] Múltiples opciones de instalación cuando sea posible

### Ejemplos y Documentación
- [ ] Al menos 3 ejemplos de uso funcionales
- [ ] Ejemplos de código copy-paste ready
- [ ] System Requirements con versiones específicas
- [ ] Enlaces a documentación válidos
- [ ] Comando para generar docs localmente (si aplica)

### Changelog y Cambios
- [ ] Changelog completo siguiendo Keep a Changelog
- [ ] Categorías: Added, Changed, Fixed, (Deprecated), (Removed), (Security)
- [ ] Technical Highlights incluidos
- [ ] Breaking Changes documentados SI EXISTEN (con guías de migración)
- [ ] Sección Breaking Changes ELIMINADA si NO EXISTEN

### Información Adicional
- [ ] Known Issues solo si existen (o frase "No critical bugs known")
- [ ] Roadmap con versión objetivo correcta
- [ ] Project Statistics con métricas reales
- [ ] Performance Benchmarks solo si son relevantes
- [ ] Licencia correcta detectada del archivo LICENSE

### Formato y Estilo
- [ ] NO quedan placeholders `[...]` sin reemplazar
- [ ] Secciones no aplicables eliminadas
- [ ] Formato Markdown correcto (enlaces, código, tablas)
- [ ] Emojis preservados exactamente como en la plantilla
- [ ] Separadores `---` entre secciones
- [ ] Información del autor correcta

### Validación Técnica
- [ ] Tipo de versión correcto (MAJOR si breaking, MINOR si features, PATCH si fixes)
- [ ] Todos los enlaces funcionan correctamente
- [ ] Comandos pueden ejecutarse sin modificación
- [ ] Ejemplos de código tienen sintaxis correcta
- [ ] Paths de archivos son correctos

---

## 🚨 Errores comunes a evitar

### Errores de Contenido
1. ❌ Dejar placeholders `[...]` sin reemplazar
2. ❌ Versión incorrecta o inconsistente entre título, badges y changelog
3. ❌ Fecha incorrecta o en formato no ISO
4. ❌ Breaking changes sin documentar o sin guía de migración
5. ❌ Changelog genérico sin detalles específicos del release
6. ❌ Ejemplos de uso que no funcionan o con sintaxis incorrecta
7. ❌ System Requirements vagos (sin versiones específicas)

### Errores de Formato
8. ❌ Incluir sección Breaking Changes cuando NO hay breaking changes
9. ❌ Mezclar categorías del changelog (poner fixes en Added, etc.)
10. ❌ Comandos de instalación genéricos que no funcionan
11. ❌ Enlaces rotos o placeholders en URLs
12. ❌ Paths de archivos incorrectos en Implemented Features
13. ❌ Modificar el estilo, emojis o estructura de la plantilla

### Errores de Versioning
14. ❌ Version bump incorrecto (PATCH cuando hay breaking changes)
15. ❌ No seguir Semantic Versioning
16. ❌ Roadmap con versión objetivo incorrecta

### Errores de Adaptación
17. ❌ No adaptar contenido al tipo de proyecto (usar ejemplos de CLI en una librería)
18. ❌ Incluir secciones no relevantes (Performance Benchmarks en proyecto simple)
19. ❌ Release Contents genérico sin adaptar a binarios/packages/código

### Errores de Calidad
20. ❌ Información del autor incorrecta o modificada sin autorización
21. ❌ Licencia hardcodeada en lugar de detectarla del proyecto
22. ❌ Project Statistics con métricas inventadas o sin verificar
23. ❌ Known Issues triviales o irrelevantes
24. ❌ Acknowledgments genéricos sin mencionar dependencias reales

---

## 📚 Referencias y recursos

### Semantic Versioning
- Formato: MAJOR.MINOR.PATCH
- MAJOR: Breaking changes (incompatible con versiones anteriores)
- MINOR: Nuevas features (compatible hacia atrás)
- PATCH: Bug fixes (compatible hacia atrás)
- Docs: https://semver.org/

### Keep a Changelog
- Formato estándar para changelog
- Categorías: Added, Changed, Deprecated, Removed, Fixed, Security
- Docs: https://keepachangelog.com/

### Buenas Prácticas
- Ser específico y claro
- Priorizar información útil para usuarios
- Documentar breaking changes exhaustivamente
- Incluir ejemplos funcionales
- Mantener consistencia entre releases
- Enlazar a issues y PRs relevantes

---

## 📝 Notas finales

- Esta guía y la plantilla están diseñadas para mantener consistencia entre releases
- La IA debe priorizar la detección automática de cambios desde la última versión
- En caso de duda sobre si incluir una sección, evaluar su utilidad para los usuarios
- Los breaking changes SIEMPRE deben documentarse con guías de migración
- El changelog debe ser exhaustivo pero conciso
- Mantener el inglés como idioma estándar para todos los release notes
- El release notes es la primera impresión de los usuarios sobre la nueva versión
- Cada release notes debe ser standalone (entendible sin leer releases anteriores)
- Priorizar claridad y utilidad sobre exhaustividad técnica
