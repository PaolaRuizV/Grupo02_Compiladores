# Diseño e implementación de un lenguaje para la generación automática de currículum vitae (CV)

## 📌 Descripción General
Este proyecto implementa un lenguaje especializado (DSL) en C++ para **registrar estudiantes, ofertas laborales, educación y experiencia, evaluar compatibilidad con ofertas y generar CVs compatibles con sistemas ATS (Applicant Tracking Systems)**.

El compilador realiza:
- Análisis léxico
- Análisis sintáctico recursivo descendente
- Análisis semántico
- Traducción dirigida por la sintaxis para generar CVs en HTML

Todo esto se enmarca en el caso de estudio de la Universidad San Ignacio de Loyola (USIL), orientado a mejorar la empleabilidad de sus estudiantes y egresados mediante CVs estandarizados y legibles por ATS.

---

## 🎯 Caso de Estudio
La USIL presenta desafíos en sus indicadores de empleabilidad según el QS World University Ranking 2025.  
En este contexto, el área de Empleabilidad y Alumni requiere herramientas que ayuden a los estudiantes a superar los filtros automatizados de reclutamiento.

El DSL propuesto permite:
- Registrar estudiantes con validación de datos.
- Registrar ofertas laborales con habilidades requeridas.
- Construir CVs basados en educación y experiencia declaradas.
- Evaluar la compatibilidad entre un CV y una oferta según habilidades.
- Generar automáticamente un **CV ATS en HTML** cuando la compatibilidad es adecuada.
- Visualizar en consola tanto el CV como las postulaciones generadas.

---

## 🎯 Objetivo General
Diseñar e implementar un lenguaje especializado que permita el **registro de estudiantes, ofertas laborales y la generación personalizada de CVs estandarizados y compatibles con ATS**, contribuyendo a mejorar la visibilidad laboral de los estudiantes y egresados USIL.

---

## 🎯 Objetivos Específicos
- Identificar los requerimientos funcionales y no funcionales del lenguaje.
- Definir la **sintaxis y semántica** formal del DSL.
- Implementar un **analizador léxico** que reconozca los tokens del lenguaje.
- Implementar un **analizador sintáctico recursivo descendente** que valide la estructura de las sentencias.
- Implementar un **analizador semántico** que garantice la coherencia de referencias y reglas de negocio.
- Implementar un módulo de **traducción dirigida por la sintaxis** para generar CVs en HTML compatibles con ATS.
- Aplicar el lenguaje en un caso práctico con datos de estudiantes y ofertas simuladas.

---

## 🧩 Características del Lenguaje (DSL)

El lenguaje soporta las siguientes construcciones principales:

### ✔ Registro de estudiantes
```txt
registrar estudiante {
    codigo: 2312227;
    nombre: Omar Morales;
    carrera: Ingeniería de Software;
    correo: omar@usil.pe;
    telefono: 942131311;
};
```

### ✔ Registro de ofertas laborales
```txt
registrar OF_001 {
    empresa: Tech Labs;
    puesto: Practicante de Ingeniería;
    habilidades: Java, Selenium, Jira;
    sueldo: 1500;
    fechamaxima: 03/04/2025;
};
```

### ✔ Asignación de educación a un CV
```txt
asignar ED_01 a CV_2312227 {
    institucion: USIL;
    campo: Ingeniería de Software;
    fechainicio: 03/03/2023;
    fechafin: 12/12/2027;
};
```

### ✔ Asignación de experiencia laboral a un CV
```txt
asignar EX_01 a CV_2312227 {
    empresa: Tech Labs;
    puesto: Practicante de Ingeniería;
    fechainicio: 12/12/2024;
    fechafin: 12/12/2025;
    habilidades: Java, Selenium, Jira;
};
```

### ✔ Evaluación CV–Oferta
```txt
evaluar CV_2312227 para OF_001;
```

Si el porcentaje de coincidencia de habilidades es **≥ 70%**, se genera automáticamente el archivo:
```txt
CV_2312227_ATS.html
```
y se crea una **postulación** interna con identificador:
```txt
PT_2312227
```

### ✔ Visualización de CV en consola
```txt
mostrar CV_2312227;
```
Muestra:
- Nombre del estudiante
- Datos de contacto (correo y teléfono)
- Sección EDUCACION
- Sección EXPERIENCIA
- Sección HABILIDADES (conjunto único obtenido de la experiencia)

### ✔ Visualización de postulaciones en consola
```txt
mostrar PT_2312227;
```
Muestra:
- Lista de ofertas asignadas a la postulación
- Empresa y puesto de cada oferta

---

## 🏗 Arquitectura del Compilador

El compilador está organizado en varios módulos:

### 🔹 1. Analizador Léxico (`AnalizadorLexico`)
Responsable de convertir el código fuente en una secuencia de tokens.  
Reconoce:
- Palabras clave: `REGISTRAR`, `ESTUDIANTE`, `ASIGNAR`, `A`, `CREAR`, `EVALUAR`, `PARA`, `MOSTRAR`
- Identificadores (`CV_XXXXXXX`, `OF_XXX`, `ED_XX`, `EX_XX`)
- Números y fechas (`dd/mm/yyyy`)
- Campos semánticos (`codigo`, `correo`, `telefono`, `empresa`, `puesto`, `habilidades`, etc.)
- Comentarios de línea `//` y de bloque `/* ... */`
- Símbolos: `{`, `}`, `:`, `,`, `;`

Tras el escaneo inicial, la función:
```cpp
vector<Token> postprocesarTokens(const vector<Token> &in);
```
refina los tokens y reconoce tipos especiales como `T_CODIGO`, `T_CORREO`, `T_FECHA`, `T_HABILIDAD`, etc.

---

### 🔹 2. Analizador Sintáctico (`AnalizadorSintactico`)
Implementado como **analizador recursivo descendente**, procesa la secuencia de tokens y reconoce las producciones del lenguaje.  
Funciones principales:

- `void analizar();`
- `void sentencia();`
- `void sentenciaRegistrar();`
- `void sentenciaAsignar();`
- `void sentenciaCrear();`
- `void sentenciaEvaluar();`
- `void sentenciaMostrar();`

Cada producción sintáctica invoca acciones semánticas asociadas, lo que constituye la **traducción dirigida por la sintaxis**.

---

### 🔹 3. Análisis Semántico
Se realiza mediante funciones que operan sobre estructuras de datos globales:

```cpp
map<string, Estudiante>   estudiantes;
map<string, Educacion>    educaciones;
map<string, Experiencia>  experiencias;
map<string, Oferta>       ofertas;
map<string, CV>           cvs;
map<string, Postulacion>  postulaciones;
```

Funciones clave:
- `void registrarEstudianteEnTabla(Estudiante e);`
- `void registrarOfertaEnTabla(const Oferta &o);`
- `void registrarEducacionEnTabla(const Educacion &ed, bool aplicar);`
- `void registrarExperienciaEnTabla(const Experiencia &ex, bool aplicar);`
- `void registrarPostulacion(const string &ofId, const string &ptId);`
- `void crearCVVacio(const string &cvId);`
- `void evaluarCompatibilidadCVOferta(const string &cvId, const string &ofId);`
- `void mostrarCV(const string &cvId);`
- `void mostrarPostulacion(const string &ptId);`

Entre las validaciones semánticas se incluyen:
- Código de estudiante de 7 dígitos.
- Correo institucional que termine en `@usil.pe`.
- Teléfono de 9 dígitos que empieza en `9`.
- Existencia del estudiante antes de crear un `CV_`.
- Existencia de ofertas antes de asignarlas a postulaciones.
- Validación de formato de fechas `dd/mm/yyyy`.
- Cálculo del porcentaje de coincidencia de habilidades entre CV y oferta.

---

### 🔹 4. Traducción Dirigida por la Sintaxis
La **traducción dirigida por la sintaxis (TDS)** se implementa integrando acciones semánticas dentro de las reglas del análisis sintáctico.

Ejemplos:
- Al terminar de procesar `registrar estudiante { ... };` se ejecuta:
  ```cpp
  registrarEstudianteEnTabla(e);
  ```
- Al procesar `asignar ED_01 a CV_XXXX { ... };` o `asignar EX_01 a CV_XXXX { ... };` se ejecuta:
  ```cpp
  registrarEducacionEnTabla(ed, aplicarSemantica);
  registrarExperienciaEnTabla(ex, aplicarSemantica);
  ```
- Al procesar `evaluar CV_XXXX para OF_YYY;` se ejecuta:
  ```cpp
  evaluarCompatibilidadCVOferta(cvId, ofId);
  ```
  y, en caso de compatibilidad suficiente, se llama a:
  ```cpp
  generarCVHtml(cvId, ofId);
  ```

De esta forma, el compilador **construye estructuras internas, valida reglas y genera la salida HTML** durante el propio análisis sintáctico.

---

## 📄 Salida ATS en HTML

Cuando la compatibilidad entre un CV y una oferta es mayor o igual al 70%, se genera un archivo HTML con la siguiente estructura general:

- Título: `CV - <Nombre del Estudiante>`
- Encabezado centrado con nombre y datos de contacto.
- Sección **EDUCACION** con institución, campo y periodo.
- Sección **EXPERIENCIA** con empresa, puesto y periodo.
- Sección **HABILIDADES** con una lista única de habilidades obtenidas de la experiencia.

Nombre del archivo generado:
```txt
CV_<CODIGO>_ATS.html
```

---

## ▶ Cómo Ejecutar

1. Crear un archivo de entrada llamado:
```txt
archivo.txt
```
con las sentencias del DSL.

2. Compilar el programa:
```bash
g++ main.cpp -o dsl_cv
```

3. Ejecutar:
```bash
./dsl_cv
```

4. Revisar:
   - La salida en consola (tokens, análisis sintáctico, mensajes semánticos, comandos `mostrar`).
   - Los archivos generados, por ejemplo:
     ```txt
     CV_2312227_ATS.html
     ```

---

## 📁 Estructura Sugerida del Proyecto
```txt
📦 DSL-CV-USIL
 ┣ 📜 main.cpp          # Código fuente del compilador/interpretador del DSL
 ┣ 📜 archivo.txt       # Código de entrada en el lenguaje DSL
 ┣ 📜 CV_XXXXXXX_ATS.html
 ┗ 📜 README.md o README.txt
```

---

## 🧱 Tecnologías Utilizadas
- C++ (STL: `<vector>`, `<map>`, `<set>`, `<sstream>`, etc.)
- Manejo de archivos (`fstream`)
- Generación de HTML básica
