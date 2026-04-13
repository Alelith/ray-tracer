# 🧠 Guía para generación y modificación de README.md con IA

Este proyecto utiliza una plantilla estándar (`README_TEMPLATE.md`) para generar o actualizar el archivo `README.md`.  
El objetivo de esta guía es indicar a los agentes de IA (como **GitHub Copilot**, **ChatGPT**, **Claude**, etc.) cómo usar correctamente la plantilla, qué secciones conservar o suprimir y qué tono o formato seguir.

---

## ⚙️ Reglas generales

1. **Usar siempre `README_TEMPLATE.md` como referencia base.**  
   Cada sección debe conservar su título, icono y jerarquía Markdown exactamente como aparece en la plantilla.

2. **Personalizar TODOS los campos entre corchetes `[ ... ]`** según la información real del proyecto.  
   - Reemplazar `[Project Name]` con el nombre real del proyecto
   - Reemplazar `[username]` con el username de GitHub del autor
   - Reemplazar `[repo_name]` con el nombre del repositorio
   - Completar badges con información real
   - NO dejar ningún placeholder sin reemplazar

3. **Eliminar secciones vacías o no aplicables.**  
   - Si una sección del template no aporta valor al proyecto actual, debe eliminarse por completo
   - Actualizar la **Table of Contents** eliminando enlaces a secciones suprimidas
   - Mantener coherencia entre el índice y el contenido

4. **Detectar automáticamente información del proyecto:**
   - Analizar archivos de configuración (`package.json`, `requirements.txt`, `pom.xml`, `Cargo.toml`, etc.)
   - Examinar la estructura de directorios para generar **Project Structure**
   - Identificar el archivo LICENSE para completar la sección **License**
   - Buscar tests para completar la sección **Tests**
   - Revisar archivos de configuración para la sección **Configuration**

5. **Mantener coherencia de estilo:**
   - Lenguaje: **Inglés técnico formal y claro**
   - Mantener TODOS los emojis e iconos (🚀, ⚙️, 🧩, etc.) tal como aparecen en la plantilla
   - Conservar los separadores `---` entre secciones
   - Usar formato Markdown correcto para código, enlaces y listas
   - NO agregar secciones que no estén en la plantilla sin consultarlo primero

6. **Información del autor:**
   - Si los datos del autor están en la plantilla (Lilith Estévez Boeta), mantenerlos tal cual
   - Si se requiere cambiar el autor, reemplazar con los datos proporcionados

---

## 🧩 Estructura general esperada

El README final debe seguir el orden y estilo de las siguientes secciones (todas definidas en `README_TEMPLATE.md`):

| Sección | Descripción | Obligatoria | Notas |
|----------|--------------|--------------|--------|
| **Title and Badges** | Nombre del proyecto y etiquetas de estado, lenguaje, framework, licencia, última actualización | ✅ Sí | Personalizar badges con datos reales. Detectar licencia del proyecto. |
| **🧠 Description** | Breve resumen del propósito, objetivos y alcance del proyecto | ✅ Sí | 3–6 líneas máximo. Claro y conciso. |
| **🧩 Table of Contents** | Índice dinámico de secciones del README | ✅ Sí | Debe actualizarse eliminando enlaces a secciones no incluidas. |
| **🌟 Features** | Lista de funcionalidades principales y futuras | ✅ Sí | Usar checkbox format. Marcar [x] completadas, [ ] futuras. |
| **⚙️ Technologies Used** | Lista de tecnologías, frameworks, herramientas, etc. | ✅ Sí | Detectar automáticamente desde archivos de configuración. |
| **🏗️ Architecture** | Descripción o diagrama técnico del sistema | ⚙️ Opcional | Suprimir en proyectos simples sin arquitectura definida. Adaptar diagrama Mermaid al proyecto. |
| **📂 Project Structure** | Árbol de directorios y descripción de carpetas principales | ⚙️ Opcional | Generar automáticamente explorando el directorio del proyecto. |
| **📦 Installation** | Requisitos previos (Prerequisites) y pasos de instalación | ✅ Sí | Incluir versiones mínimas requeridas. Comandos específicos del proyecto. |
| **▶️ Execution** | Cómo ejecutar el proyecto (local y Docker si aplica) | ✅ Sí | Comandos reales y funcionales del proyecto. |
| **⚙️ Configuration** | Variables de entorno, archivos de config, etc. | ⚙️ Opcional | Buscar archivos `.env.example` o similares. Solo si el proyecto requiere configuración. |
| **🧩 Main Endpoints** | Tabla de endpoints REST/GraphQL/etc. | ⚙️ Opcional | Solo para proyectos con API. Documentar endpoints reales. |
| **🧪 Usage Examples** | Ejemplos de uso o comandos típicos con salida esperada | ✅ Sí | Ejemplos reales y funcionales. Incluir output esperado. |
| **📸 Screenshots / Demo** | Screenshots, GIFs o enlaces a demos | ⚙️ Opcional | Solo si existen assets visuales o el proyecto tiene UI. |
| **🧾 Tests** | Cómo ejecutar test unitarios o de integración | ⚙️ Opcional | Solo si el proyecto tiene tests. Detectar framework de testing. |
| **📖 Documentation** | Enlace a documentación adicional | ⚙️ Opcional | Solo si existe documentación externa (Doxygen, Sphinx, JSDoc, etc.). |
| **🚀 Deployment** | Instrucciones de deployment con pasos específicos | ⚙️ Opcional | Solo si el proyecto está desplegado o tiene instrucciones de deployment. |
| **🐞 Known Issues / ToDo** | Tabla de errores conocidos o tareas pendientes | ⚙️ Opcional | Buscar en Issues de GitHub o archivo TODO. |
| **�� Learnings & Future Improvements** | Reflexión técnica o aprendizajes | ⚙️ Opcional | Recomendado en proyectos personales o educativos. |
| **🤝 Contributing** | Guía de contribución al proyecto | ⚙️ Opcional | Mantener si el proyecto acepta contribuciones. Puede eliminarse en proyectos personales cerrados. |
| **🤝 Credits** | Fuentes, referencias, librerías, agradecimientos | ✅ Sí | Siempre incluir si se basó en otros proyectos o usa librerías de terceros. |
| **📜 License** | Licencia del proyecto con nombre específico | ✅ Sí | Detectar del archivo LICENSE. Indicar nombre completo (MIT, Apache 2.0, GPL-3.0, etc.). |
| **👩‍💻 Author** | Información del autor con enlaces a perfiles | ✅ Sí | Mantener datos de la plantilla o personalizar según se indique. |

---

## 🎯 Instrucciones específicas por sección

### **Title and Badges**
- Reemplazar `[Project Name]` con el nombre real del proyecto
- Actualizar badge de Status: `In Progress` o `Completed`
- Detectar lenguaje principal del proyecto
- Detectar framework principal (si aplica)
- Detectar tipo de licencia del archivo LICENSE
- Verificar que el username en el badge de GitHub sea correcto

### **🧠 Description**
- Máximo 3-6 líneas
- Responder: ¿Qué hace? ¿Para quién? ¿Qué problema resuelve?
- Tono técnico pero accesible

### **🧩 Table of Contents**
- **CRÍTICO:** Eliminar enlaces a secciones que no se incluyan en el README final
- Mantener orden de la plantilla
- Verificar que todos los enlaces funcionen correctamente

### **⚙️ Technologies Used**
- Detectar automáticamente desde:
  - `package.json` (Node.js/JavaScript)
  - `requirements.txt`, `setup.py`, `pyproject.toml` (Python)
  - `pom.xml`, `build.gradle` (Java)
  - `Cargo.toml` (Rust)
  - `go.mod` (Go)
  - Archivos de configuración del proyecto
- Organizar por categorías: Languages, Frameworks, Databases, Tools, Infrastructure

### **📂 Project Structure**
- Generar automáticamente explorando directorios
- Mostrar solo estructura relevante (no node_modules, .git, etc.)
- Incluir comentarios breves sobre carpetas principales

### **📦 Installation & ▶️ Execution**
- Comandos específicos y probados del proyecto
- Incluir versiones mínimas en Prerequisites
- Si tiene Docker, incluir instrucciones de Docker

### **🧪 Usage Examples**
- Ejemplos reales que funcionen
- Incluir output esperado
- Si es una librería, mostrar código de uso
- Si es una CLI, mostrar comandos con flags

### **📜 License**
- Detectar del archivo LICENSE
- Si no existe LICENSE, indicar "No license specified" o sugerir agregar una
- Incluir enlace al archivo LICENSE

### **👩‍💻 Author**
- Datos por defecto: Lilith Estévez Boeta
- Solo modificar si se indica explícitamente

---

## 🤖 Prompt recomendado para IA

Cuando quieras generar o actualizar un README, usa este prompt:

```
Genera/Actualiza el README.md de este proyecto usando README_TEMPLATE.md como plantilla base y siguiendo estrictamente las reglas de readme-guide.md.

Tareas específicas:
1. Analiza el proyecto completo (archivos, estructura, dependencias, configuración)
2. Reemplaza TODOS los placeholders [entre corchetes] con información real
3. Detecta automáticamente: lenguaje, framework, dependencias, licencia, estructura
4. Genera secciones con contenido real y útil
5. Elimina secciones no aplicables Y actualiza el índice en consecuencia
6. Mantén el formato, emojis y estilo exacto de la plantilla
7. Asegúrate de que todos los comandos y ejemplos funcionen
8. Verifica que los enlaces del índice apunten a secciones existentes

NO incluyas secciones vacías o con solo placeholders.
NO agregues secciones que no estén en la plantilla.
NO modifiques los datos del autor a menos que se indique.
```

---

## ✅ Checklist de validación

Antes de considerar el README completo, verificar:

- [ ] Título del proyecto actualizado (sin `[Project Name]`)
- [ ] Badges actualizados con información real
- [ ] Descripción clara y concisa (3-6 líneas)
- [ ] Table of Contents sincronizada con secciones presentes
- [ ] Sección Technologies Used con stack real del proyecto
- [ ] Comandos de instalación probados y funcionales
- [ ] Comandos de ejecución correctos
- [ ] Ejemplos de uso reales (no placeholders)
- [ ] Licencia correcta detectada del archivo LICENSE
- [ ] NO quedan placeholders `[...]` sin reemplazar
- [ ] Secciones no aplicables eliminadas del contenido Y del índice
- [ ] Formato Markdown correcto (enlaces, código, listas)
- [ ] Emojis preservados exactamente como en la plantilla
- [ ] Información del autor correcta

---

## 🚨 Errores comunes a evitar

1. ❌ Dejar placeholders `[...]` sin reemplazar
2. ❌ Incluir secciones vacías o con solo placeholders
3. ❌ Table of Contents desincronizada (enlaces a secciones inexistentes)
4. ❌ Comandos de instalación/ejecución genéricos que no funcionan
5. ❌ Licencia hardcodeada (debe detectarse del proyecto)
6. ❌ Modificar el estilo, emojis o estructura de la plantilla
7. ❌ Agregar secciones no definidas en la plantilla
8. ❌ Información del autor incorrecta o modificada sin autorización
9. ❌ Badges con información incorrecta o placeholder
10. ❌ Project Structure manual cuando puede generarse automáticamente

---

## 📝 Notas finales

- Esta guía y la plantilla están diseñadas para mantener consistencia entre todos los proyectos
- La IA debe priorizar la detección automática de información sobre suposiciones
- En caso de duda, es mejor omitir una sección opcional que incluir información incorrecta
- El README debe ser funcional, útil y profesional
- Mantener el inglés como idioma estándar para todos los README
