# Diseño e implementación de un lenguaje para la generación automática de currículum vitae (CV)

## 📌 Descripción General
El presente proyecto implementa un lenguaje especializado (DSL) diseñado para **registrar estudiantes, ofertas laborales, experiencias, educaciones y generar CVs compatibles con sistemas ATS (Applicant Tracking Systems)**.  
Se desarrolla como solución a la problemática identificada en la Universidad San Ignacio de Loyola (USIL), donde los estudiantes y egresados presentan dificultades para superar los primeros filtros de los procesos de selección automatizados.

Actualmente:
- El **99.7%** de las organizaciones utiliza ATS para filtrar postulantes.
- Hasta el **75%** de los CVs son descartados automáticamente por errores de formato o ausencia de palabras clave.
- Menos de **1 de cada 10** currículums llega a un reclutador humano.

El objetivo de este proyecto es brindar una herramienta que **genere CVs estandarizados, estructurados y compatibles con ATS**, aumentando la visibilidad y competitividad laboral de los postulantes.

---

## 🎯 Caso de Estudio
La USIL enfrenta desafíos en los indicadores de empleabilidad (QS 2025: puntaje 3.7/100). Como parte del área de Empleabilidad y Alumni, se requiere fortalecer la capacidad de los estudiantes para superar filtros automatizados al postular a empleos.  
Este proyecto propone un lenguaje declarativo que permite:

- Registrar estudiantes con validación de campos y requisitos formales.
- Registrar ofertas laborales con habilidades requeridas.
- Asignar educación y experiencia a un CV.
- Evaluar compatibilidad estudiante–oferta.
- Generar CVs ATS en formato HTML.

La herramienta automatiza procesos que antes eran manuales, dispersos o dependientes de plataformas externas.

---

## 🎯 Objetivo General
Diseñar e implementar un lenguaje especializado que permita el **registro de estudiantes, ofertas laborales y generación personalizada de CVs estandarizados y compatibles con ATS**, contribuyendo a mejorar la visibilidad y empleabilidad de estudiantes y egresados USIL.

---

## 🎯 Objetivos Específicos
- Identificar requerimientos funcionales y no funcionales del lenguaje.
- Diseñar formalmente la **sintaxis y semántica** del DSL.
- Implementar un **analizador léxico** para reconocer tokens del lenguaje.
- Implementar un **analizador sintáctico recursivo descendente** que valide estructuras.
- Implementar un **analizador semántico** que garantice coherencia, referencias válidas y reglas de negocio.
- Implementar **traducción dirigida por la sintaxis** para generar CVs ATS en HTML.
- Validar el lenguaje mediante un **caso práctico con estudiantes USIL**.

---

## 🧩 Características del Lenguaje (DSL)
El lenguaje permite declarar y procesar:

### ✔ Estudiantes
```
registrar estudiante {
    codigo: 2312227;
    nombre: Omar Morales;
    carrera: Ingeniería de Software;
    correo: omar@usil.pe;
    telefono: 942131311;
};
```

### ✔ Ofertas Laborales
```
registrar OF_001 {
    empresa: Tech Labs;
    puesto: Practicante de Ingeniería;
    habilidades: Java, Selenium, Jira;
    sueldo: 1500;
    fechamaxima: 03/04/2025;
};
```

### ✔ Asignación de educación y experiencia
```
asignar ED_01 a CV_2312227 {
    institucion: USIL;
    campo: Ingeniería de Software;
    fechainicio: 03/03/2023;
    fechafin: 12/12/2027;
};
```

### ✔ Evaluación CV–Oferta
```
evaluar CV_2312227 para OF_001;
```

### ✔ Generación de CV ATS en HTML
Si la compatibilidad ≥ 70%, el sistema genera automáticamente:  
`CV_2312227_ATS.html`

---

## 🏗 Arquitectura del Compilador

El compilador está conformado por los siguientes módulos:

### 🔹 1. Analizador Léxico (Scanner)
Tokeniza la entrada, reconoce:
- Palabras clave: `registrar`, `asignar`, `evaluar`, etc.
- Fechas con formato `dd/mm/yyyy`.
- Identificadores `CV_XXXXXX`, `ED_XX`, `EX_XX`, `OF_XX`.
- Campos semánticos especiales: correo USIL, teléfono válido, habilidades, etc.

### 🔹 2. Analizador Sintáctico (Parser)
Implementado como **recursivo descendente**.  
Reconoce la gramática del DSL mediante funciones:

- `sentenciaRegistrar()`
- `sentenciaAsignar()`
- `sentenciaCrear()`
- `sentenciaEvaluar()`
- `sentenciaMostrar()`

### 🔹 3. Analizador Semántico
Valida reglas de negocio:
- Código de estudiante válido
- Correo institucional `@usil.pe`
- Teléfono válido
- Existencia del estudiante antes de crear CV
- Vinculación correcta de educación y experiencia
- Comparación de habilidades CV–oferta

### 🔹 4. Traducción Dirigida por la Sintaxis
Las acciones semánticas se ejecutan dentro de las reglas sintácticas, generando:
- Estructuras internas
- Validaciones semánticas
- CV ATS en HTML

---

## ▶ Cómo Ejecutar

1. Crear archivo de entrada `archivo.txt`
2. Compilar:
```
g++ main.cpp -o dsl_cv
```
3. Ejecutar:
```
./dsl_cv
```
4. Ver el archivo generado:
```
CV_<CODIGO>_ATS.html
```

---

## 📁 Estructura del Proyecto
```
📦 DSL-CV-USIL
 ┣ 📜 main.cpp
 ┣ 📜 archivo.txt
 ┣ 📜 CV_XXXXXXX_ATS.html
 ┗ 📜 README.txt
```

---

## 📄 Licencia
Proyecto de uso educativo bajo licencia MIT.
