# Diseño e implementación de un lenguaje para la generación automática de currículum vitae (CV)

Proyecto académico que implementa un **lenguaje específico de dominio (DSL)** en C++ para el registro de estudiantes, ofertas laborales y la generación automática de currículums vitae **estandarizados y compatibles con sistemas ATS** (Applicant Tracking Systems), orientado a estudiantes y egresados de la **Universidad San Ignacio de Loyola (USIL)**.

El sistema incluye:

- Analizador léxico  
- Analizador sintáctico recursivo descendente  
- Analizador semántico con tablas de símbolos  
- Módulo de **traducción dirigida por la sintaxis (TDS)** que genera un **CV ATS en HTML** y salidas legibles en consola (`mostrar CV_...` y `mostrar PT_...`).  

---

## 1. Descripción del caso de estudio

La USIL, como institución privada de educación superior, ofrece servicios de empleabilidad y asesoría de CV para sus estudiantes y egresados. Sin embargo, los indicadores de empleabilidad en rankings internacionales muestran desafíos en la preparación de los postulantes para superar los filtros iniciales de selección automatizada.

En el contexto actual, aproximadamente el **99.7 % de las organizaciones** utiliza **ATS** para filtrar postulaciones, descartando de forma automática CVs que no cumplen requisitos de **estructura, formato o palabras clave**. Esto provoca que la mayoría de currículums nunca llegue a ser revisada por un reclutador humano.

Ante esta problemática, este proyecto propone un **lenguaje especializado** que:

- Estandariza la forma de registrar estudiantes y ofertas laborales.  
- Permite definir **experiencia, educación y habilidades** de manera estructurada.  
- Genera automáticamente un **CV ATS en HTML**, alineado a criterios formales y de palabras clave.  
- Integra reglas semánticas para validar la coherencia de los datos (fechas, correos, teléfonos, estado de ofertas, etc.).  

---

## 2. Objetivos del proyecto

### 2.1 Objetivo general

Diseñar e implementar un lenguaje especializado que permita el registro de estudiantes, ofertas laborales y la generación personalizada de currículums vitae estandarizados y compatibles con sistemas de reclutamiento automatizado (ATS), con el fin de mejorar la visibilidad laboral de los estudiantes y egresados de la USIL.

### 2.2 Objetivos específicos

1. **Identificar** los requerimientos funcionales y no funcionales necesarios para el diseño del lenguaje orientado al registro de estudiantes, oportunidades laborales y generación de CVs.
2. **Desarrollar** el diseño formal del lenguaje, incluyendo su sintaxis y semántica, para garantizar la compatibilidad con ATS.
3. **Implementar** un analizador léxico que permita la correcta interpretación de los tokens definidos en el lenguaje.
4. **Implementar** un analizador sintáctico recursivo descendente que valide la estructura de los registros y CVs generados con base en las reglas gramaticales del lenguaje.
5. **Implementar** un analizador semántico que asegure la coherencia y validez de los datos incluidos en los registros, CVs y postulaciones.
6. **Desarrollar** un módulo de traducción dirigida por la sintaxis que genere documentos finales en formato HTML, compatibles con ATS.
7. **Aplicar** el lenguaje en un caso práctico de generación de CVs para validar su efectividad en procesos de postulación laboral.

---

## 3. Arquitectura general del sistema

El sistema está implementado en un único archivo C++ que agrupa los siguientes módulos principales:

1. **Funciones auxiliares básicas**  
   - Manejo de cadenas (`trim`, `toLowerStr`).  
   - Validación de formatos numéricos (`esSoloDigitos`).  
   - Validación de fechas (`esFechaValida`, `fechaMenorOIgual`, `fechaActualSistema`).  

2. **Analizador léxico (`AnalizadorLexico`)**  
   - Tokeniza el contenido de `archivo.txt`.  
   - Reconoce palabras clave (`REGISTRAR`, `ESTUDIANTE`, `ASIGNAR`, `CREAR`, `EVALUAR`, `MOSTRAR`, etc.).  
   - Reconoce números, fechas, cadenas y símbolos (`{`, `}`, `:`, `,`, `;`).  
   - Soporta comentarios de línea `//` y de bloque `/* ... */`.

3. **Post-procesamiento de tokens (`postprocesarTokens`)**  
   - Re-clasifica secuencias como tokens especializados:  
     - `T_CODIGO`, `T_NOMBRE`, `T_CARRERA`, `T_CORREO`, `T_TELEFONO`,  
       `T_EMPRESA`, `T_PUESTO`, `T_FECHA`, `T_HABILIDAD`, `T_SUELDO`, `T_ESPECIALIZACION`.  
   - Agrupa valores multi-palabra (por ejemplo, nombres, carreras, puestos, habilidades).  

4. **Modelos de datos (tablas de símbolos)**  
   Definidos mediante `struct` y `map`:
   - `Estudiante`  
   - `Educacion`  
   - `Experiencia`  
   - `Oferta`  
   - `CV`  
   - `Postulacion`  

   Tablas globales:
   ```cpp
   map<string, Estudiante>   estudiantes;
   map<string, Educacion>    educaciones;
   map<string, Experiencia>  experiencias;
   map<string, Oferta>       ofertas;
   map<string, CV>           cvs;
   map<string, Postulacion>  postulaciones;
   ```

5. **Módulo semántico**  
   - Funciones que insertan y validan datos en las tablas:
     - `registrarEstudianteEnTabla`
     - `registrarOfertaEnTabla`
     - `registrarEducacionEnTabla`
     - `registrarExperienciaEnTabla`
     - `registrarPostulacion`
     - `crearCVVacio`
     - `evaluarCompatibilidadCVOferta`
     - `ofertaActiva`
   - Reglas de negocio y validaciones de consistencia (ver sección 7).

6. **Módulo de traducción dirigida por la sintaxis (TDS)**  
   - `generarCVHtml`: genera un CV ATS en formato HTML.  
   - `mostrarCV`: muestra el CV en formato texto plano, tipo CV ATS.  
   - `mostrarPostulacion`: muestra las ofertas asociadas a una postulacion `PT_XXXX`.

7. **Analizador sintáctico (`AnalizadorSintactico`)**  
   - Implementa un **parser recursivo descendente**.  
   - Cada producción de la gramática está asociada directamente a una función en C++ que ejecuta acciones semánticas.

---

## 4. Diseño del lenguaje

### 4.1 Entidades del dominio

El DSL modela explícitamente las siguientes entidades:

- **Estudiante**: código, nombre completo, carrera, correo institucional, teléfono.  
- **CV**: identificado por `CV_<codigo>`, agrupa educación y experiencia del estudiante.  
- **Educación**: institución, campo de estudio, fechas de inicio y fin.  
- **Experiencia**: empresa, puesto, habilidades y fechas.  
- **Oferta**: identificador `OF_XXX`, empresa, puesto, habilidades requeridas, sueldo y `fechamaxima`.  
- **Postulación**: identificador `PT_<codigo>`, agrupa ofertas asociadas a un candidato.

### 4.2 Sintaxis básica

#### 4.2.1 Registro de estudiante

```dsl
registrar estudiante {
    codigo: 2312227;
    nombre: Omar Morales;
    carrera: Ingeniería de Software;
    correo: omar@usil.pe;
    telefono: 942131311;
};
```

#### 4.2.2 Registro de oferta laboral

```dsl
registrar OF_001 {
    empresa: Tech Labs;
    puesto: Practicante de Ingeniería de Software;
    habilidades: Java, Selenium, Jira;
    sueldo: 1500;
    fechamaxima: 31/12/2025;
};
```

#### 4.2.3 Creación de CV asociado a un estudiante

```dsl
crear CV_2312227;
```

> Semántica: solo se creará el CV si el código `2312227` pertenece a un estudiante previamente registrado.

#### 4.2.4 Asignación de educación a un CV

```dsl
asignar ED_01 a CV_2312227 {
    institucion: USIL;
    campo: Ingeniería de Software;
    fechainicio: 03/03/2023;
    fechafin: 12/12/2027;
};
```

#### 4.2.5 Asignación de experiencia a un CV

```dsl
asignar EX_01 a CV_2312227 {
    empresa: Tech Labs;
    puesto: Practicante de Ingeniería;
    fechainicio: 12/12/2024;
    fechafin: 12/12/2025;
    habilidades: Java, Selenium, Jira;
};
```

#### 4.2.6 Evaluación de compatibilidad CV–Oferta

```dsl
evaluar CV_2312227 para OF_001;
```

- Calcula el porcentaje de coincidencia entre las habilidades del CV y las de la oferta.
- Si la coincidencia es **≥ 70 %**:
  - Genera el archivo `CV_2312227_ATS.html`.
  - Crea automáticamente una **postulación** con id `PT_2312227`.

#### 4.2.7 Asignación explícita de oferta a una postulación

```dsl
asignar OF_001 a PT_2312227;
```

> Solo se permite si:
> - La postulación `PT_2312227` existe.  
> - La oferta `OF_001` existe.  
> - La oferta está activa según su `fechamaxima`.

#### 4.2.8 Mostrar CV o postulaciones

```dsl
mostrar CV_2312227;
mostrar PT_2312227;
```

---

## 5. Analizador léxico

El analizador léxico (`AnalizadorLexico`) lee `archivo.txt` y produce una secuencia de `Token`.

Características:

- Soporta comentarios:
  ```cpp
  // comentario de una línea
  /* comentario
     de varias líneas */
  ```
- Reconoce:
  - **Palabras clave**: `REGISTRAR`, `ESTUDIANTE`, `ASIGNAR`, `CREAR`, `EVALUAR`, `PARA`, `MOSTRAR`, `A`.  
  - **Identificadores**: nombres de entidades (`ED_01`, `EX_01`, `OF_001`, `CV_2312227`, `PT_2312227`, textos Unicode).  
  - **Números y fechas**: secuencias con dígitos y `/`.  
  - **Cadenas entre comillas** (soporte adicional).  
  - **Símbolos de puntuación**: `{`, `}`, `:`, `,`, `;`.

Posteriormente, la función `postprocesarTokens` transforma secuencias genéricas en tokens especializados (`T_CODIGO`, `T_NOMBRE`, `T_FECHA`, `T_HABILIDAD`, etc.), facilitando la fase sintáctica y semántica.

---

## 6. Analizador sintáctico recursivo descendente

El analizador sintáctico (`AnalizadorSintactico`) trabaja sobre el vector de tokens postprocesados.

### 6.1 Estructura general

Producción principal:

```ebnf
programa  ::= { sentencia } FIN ;
sentencia ::= sentenciaRegistrar
            | sentenciaAsignar
            | sentenciaCrear
            | sentenciaEvaluar
            | sentenciaMostrar ;
```

Cada tipo de sentencia se implementa como método:

- `sentenciaRegistrar()`
- `sentenciaAsignar()`
- `sentenciaCrear()`
- `sentenciaEvaluar()`
- `sentenciaMostrar()`

Cada uno invoca funciones semánticas en puntos clave, por ejemplo:

- `registrarEstudianteEnTabla(e);`
- `registrarOfertaEnTabla(o);`
- `registrarEducacionEnTabla(ed, aplicarSemantica);`
- `registrarExperienciaEnTabla(ex, aplicarSemantica);`
- `evaluarCompatibilidadCVOferta(cvId, ofId);`
- `mostrarCV(id);`
- `mostrarPostulacion(id);`

---

## 7. Analizador semántico y tablas de símbolos

El proyecto implementa un **analizador semántico explícito**, responsable de validar reglas de negocio y coherencia interna de los datos.

### 7.1 Tablas de símbolos

- `estudiantes[codigo]`  
- `educaciones[id_edu]`  
- `experiencias[id_exp]`  
- `ofertas[id_oferta]`  
- `cvs[id_cv]`  
- `postulaciones[id_pt]`  

Estas estructuras se actualizan en las funciones:

- `registrarEstudianteEnTabla`
- `registrarOfertaEnTabla`
- `registrarEducacionEnTabla`
- `registrarExperienciaEnTabla`
- `registrarPostulacion`
- `crearCVVacio`

### 7.2 Reglas semánticas principales

1. **Código de estudiante**  
   - Debe tener exactamente **7 dígitos** (`esCodigoEstudianteValido`).  
   - Si no cumple, se muestra un error.

2. **Correo institucional**  
   - Debe terminar en `@usil.pe`.  
   - Si no cumple, se lanza una advertencia.

3. **Teléfono celular**  
   - Debe tener **9 dígitos** y comenzar en `9`.  
   - Si no cumple, se lanza una advertencia.

4. **Creación de CV (`crear CV_XXXX;`)**  
   - Extrae el código numérico desde `CV_XXXX`.  
   - Verifica que el estudiante exista en la tabla `estudiantes`.  
   - Si no existe, se muestra un error y el CV no se crea.

5. **Asignación de educación / experiencia (`asignar ... a CV_XXXX { ... };`)**
   - Verifica que el CV está asociado a un estudiante registrado.
   - Valida formato de fechas (`esFechaValida`).
   - Valida **rango de fechas**:
     - `fechainicio <= fechafin` mediante `fechaMenorOIgual`.  
     - Si **fechainicio > fechafin**, se muestra un error y la educación/experiencia no se registra.

6. **Ofertas laborales y estado activo (`ofertaActiva`)**  
   - Cada oferta tiene un campo opcional `fechamaxima`.  
   - La función:
     ```cpp
     bool ofertaActiva(const Oferta &o) {
         if (o.fechamaxima.empty() || !esFechaValida(o.fechamaxima)) return true;
         string hoy = fechaActualSistema();
         if (!esFechaValida(hoy)) return true;
         return fechaMenorOIgual(hoy, o.fechamaxima); // hoy <= fechamaxima
     }
     ```
   - Se valida el estado de la oferta en:
     - `evaluarCompatibilidadCVOferta`
     - `registrarPostulacion`
   - Si la oferta ya no está activa, se imprime un mensaje de error y no se continúa con la operación.

7. **Evaluación de compatibilidad CV–Oferta**
   - Se construye el conjunto de habilidades del CV a partir de las experiencias asignadas.
   - Se calcula:
     ```text
     coincidencias = |habilidades_CV ∩ habilidades_oferta|
     porcentaje = coincidencias / |habilidades_oferta| * 100
     ```
   - Si `porcentaje >= 70 %`:
     - Se genera el archivo `CV_XXXX_ATS.html`.
     - Se crea automáticamente una **postulación** `PT_<codigo_estudiante>` si aún no existe.

8. **Registro de postulaciones manuales**
   - La sentencia:
     ```dsl
     asignar OF_001 a PT_2312227;
     ```
     valida:
     - Que la postulación `PT_2312227` exista.
     - Que la oferta `OF_001` exista.
     - Que la oferta esté activa según `fechamaxima`.
   - Si todo es correcto, se agrega la oferta a la lista `postulaciones[ptId].ofertas`.

---

## 8. Traducción dirigida por la sintaxis (TDS)

La **traducción dirigida por la sintaxis** se realiza en dos niveles de salida:

### 8.1 Generación de CV ATS en HTML (`generarCVHtml`)

Cuando `evaluar CV_XXXX para OF_YYY;` alcanza un porcentaje ≥ 70 %, se genera un archivo:

```text
CV_XXXX_ATS.html
```

Contenido del HTML:

- `<h1>` con el nombre del estudiante.  
- Bloque de **contacto** con correo institucional y teléfono.  
- Sección **EDUCACION** con líneas:
  ```text
  Institución | Campo | fechainicio - fechafin
  ```
- Sección **EXPERIENCIA** con líneas:
  ```text
  Empresa | Puesto | fechainicio - fechafin
  ```
- Sección **HABILIDADES** como lista `<ul><li>...</li></ul>` con habilidades únicas recopiladas del CV.

Este formato está pensado para:

- Ser legible por humanos.  
- Mantener una estructura clara y limpia (títulos, listas, secciones), propia de un CV ATS.

### 8.2 Salida de CV en consola (`mostrar CV_XXXX;`)

La sentencia:

```dsl
mostrar CV_2312227;
```

Produce una salida en consola con el siguiente esquema:

```text
<Nombre del estudiante>
<correo> | <telefono>
EDUCACION
<Institucion> | <Campo> | <FechaInicio> - <FechaFin>

EXPERIENCIA
- <Empresa> | <Puesto> | <FechaInicio> - <FechaFin>

HABILIDADES
- <Habilidad 1>
- <Habilidad 2>
...
```

### 8.3 Salida de postulaciones (`mostrar PT_XXXX;`)

La sentencia:

```dsl
mostrar PT_2312227;
```

Muestra:

```text
Postulaciones 
Ofertas asignadas:
- OF_001 | <Empresa> | <Puesto>
- OF_002 | <Empresa> | <Puesto>
...
```

---

## 9. Ejemplo de uso completo (archivo.txt)

```dsl
registrar estudiante {
    codigo: 2312227;
    nombre: Omar Morales;
    carrera: Ingeniería de Software;
    correo: omar@usil.pe;
    telefono: 942131311;
};

registrar OF_001 {
    empresa: Tech Labs;
    puesto: Practicante de Ingeniería de Software;
    habilidades: Java, Selenium, Jira;
    sueldo: 1500;
    fechamaxima: 31/12/2025;
};

crear CV_2312227;

asignar ED_01 a CV_2312227 {
    institucion: USIL;
    campo: Ingeniería de Software;
    fechainicio: 03/03/2023;
    fechafin: 12/12/2027;
};

asignar EX_01 a CV_2312227 {
    empresa: Tech Labs;
    puesto: Practicante de Ingeniería;
    fechainicio: 12/12/2024;
    fechafin: 12/12/2025;
    habilidades: Java, Selenium, Jira;
};

evaluar CV_2312227 para OF_001;

mostrar CV_2312227;
mostrar PT_2312227;
```

---

## 10. Compilación y ejecución

### 10.1 Requisitos

- Compilador C++ con soporte para C++17 o superior (por ejemplo `g++`).  
- Archivo fuente (por ejemplo `main.cpp`) con el código del DSL.  
- Archivo de entrada `archivo.txt` con las sentencias del lenguaje.

### 10.2 Compilar

```bash
g++ main.cpp -std=c++17 -O2 -o dsl_ats
```

### 10.3 Ejecutar

```bash
./dsl_ats
```

El programa:

1. Lee el contenido de `archivo.txt`.  
2. Realiza el análisis léxico, sintáctico y semántico.  
3. Muestra en consola los tokens generados.  
4. Ejecuta las sentencias del DSL.  
5. Genera, si corresponde, el archivo `CV_XXXX_ATS.html` en el mismo directorio.

---

## 11. Estructura sugerida del repositorio

Aunque el proyecto puede residir en un único archivo, se recomienda organizarlo así:

```text
.
├── src/
│   └── main.cpp          # Implementación del DSL
├── examples/
│   └── archivo.txt       # Casos de prueba y ejemplos de uso
├── docs/
│   └── informe.pdf       # Documento del trabajo académico
├── README.md             # Este archivo, en formato Markdown
└── .gitignore
```

---

## 12. Limitaciones y trabajo futuro

- Las fechas se validan por formato y orden, pero no se manejan aún calendarios complejos (años bisiestos, etc.) más allá de la estructura `dd/mm/yyyy`.
- El conjunto de habilidades depende de coincidencias literales (sensibles a escritura y espacios).
- El formato HTML generado está pensado como CV ATS básico; puede ampliarse a:
  - Exportación a otros formatos (PDF vía herramientas externas).  
  - Secciones adicionales (logros, proyectos, certificaciones).  
  - Traducción automática de secciones al inglés.

Posibles extensiones:

- Integración con un **frontend web** para que el estudiante genere su archivo DSL desde formularios.  
- Implementación de un **intérprete interactivo** con consola de comandos.  
- Soporte de **múltiples plantillas HTML** para distintos tipos de cargo (junior, trainee, analista, etc.).  
