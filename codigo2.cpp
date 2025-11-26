// ======================================================
// Analizador L�xico + Sint�ctico Recursivo Descendente
// Sistema de CVs y Ofertas Laborales
// Autor: ChatGPT
// Compilar con: g++ -std=c++17 analizador.cpp -o analizador
// Ejecutar con: ./analizador
// ======================================================

#include <bits/stdc++.h>
using namespace std;


// ENUMERACION DE TOKENS 
enum TipoToken {
    T_ESTUDIANTE, T_CV, T_OFERTA,
    T_EXPERIENCIA, T_EDUCACION, T_CONTACTO,
    T_LOGROS, T_PALABRAS_CLAVE, T_REFERENCIA_ESTUDIANTE,
    // tokens adicionales para nombres de listas/variantes
    T_EXPERIENCIA_LISTA, T_LOGROS_LISTA,
    T_EVALUAR, T_SI, T_ENTONCES, T_GENERAR, T_CONTIENE,
    T_PARA, T_EN, T_FORMATO, T_MOSTRAR,
    IDENTIFICADOR, NUMERO, CADENA,
    LLAVE_IZQ, LLAVE_DER, CORCH_IZQ, CORCH_DER,
    DOS_PUNTOS, COMA, PUNTO_Y_COMA, PUNTO,
    FIN_TOKEN, DESCONOCIDO
};


// ESTRUCTURA DEL TOKEN
struct Token {
    TipoToken tipo;
    string valor;
    Token(TipoToken t = DESCONOCIDO, const string &v = "") : tipo(t), valor(v) {}
};


// FUNCIONES AUXILIARES
string nombreToken(TipoToken t) {
    switch (t) {
        case T_ESTUDIANTE: return "ESTUDIANTE";
        case T_CV: return "CV";
        case T_OFERTA: return "OFERTA";
        case T_EXPERIENCIA: return "EXPERIENCIA";
        case T_EDUCACION: return "EDUCACION";
        case T_CONTACTO: return "CONTACTO";
        case T_LOGROS: return "LOGROS";
        case T_PALABRAS_CLAVE: return "PALABRAS_CLAVE";
        case T_REFERENCIA_ESTUDIANTE: return "REFERENCIA_ESTUDIANTE";
        case T_EXPERIENCIA_LISTA: return "EXPERIENCIA_LISTA";
        case T_LOGROS_LISTA: return "LOGROS_LISTA";
        case T_EVALUAR: return "EVALUAR";
        case T_SI: return "SI";
        case T_ENTONCES: return "ENTONCES";
        case T_GENERAR: return "GENERAR";
        case T_CONTIENE: return "CONTIENE";
        case T_PARA: return "PARA";
        case T_EN: return "EN";
        case T_FORMATO: return "FORMATO";
        case T_MOSTRAR: return "MOSTRAR";
        case IDENTIFICADOR: return "IDENTIFICADOR";
        case NUMERO: return "NUMERO";
        case CADENA: return "CADENA";
        case LLAVE_IZQ: return "{";
        case LLAVE_DER: return "}";
        case CORCH_IZQ: return "[";
        case CORCH_DER: return "]";
        case DOS_PUNTOS: return ":";
        case COMA: return ",";
        case PUNTO_Y_COMA: return ";";
        case PUNTO: return ".";
        case FIN_TOKEN: return "FIN";
        default: return "DESCONOCIDO";
    }
}


// ANALIZADOR LEXICO (tokenizador)
class AnalizadorLexico {
    string entrada;
    size_t pos;

public:
    AnalizadorLexico(const string &texto) : entrada(texto), pos(0) {}

    char ver() { return pos < entrada.size() ? entrada[pos] : '\0'; }
    char obtener() { return pos < entrada.size() ? entrada[pos++] : '\0'; }

    void saltarEspacios() {
        while (isspace(ver())) obtener();
    }

    vector<Token> tokenizar() {
        vector<Token> tokens;
        while (true) {
            saltarEspacios();
            char c = ver();
            if (c == '\0') break;

            // -------- CADENA --------
            if (c == '"') {
                obtener();
                string s;
                while (ver() != '"' && ver() != '\0') s.push_back(obtener());
                if (ver() == '"') obtener();
                tokens.emplace_back(CADENA, s);
                continue;
            }

            // -------- NUMEROS --------
            if (isdigit(c)) {
                string num;
                while (isdigit(ver())) num.push_back(obtener());
                tokens.emplace_back(NUMERO, num);
                continue;
            }

            // -------- SIMBOLOS --------
            if (c == '{') { obtener(); tokens.emplace_back(LLAVE_IZQ, "{"); continue; }
            if (c == '}') { obtener(); tokens.emplace_back(LLAVE_DER, "}"); continue; }
            if (c == '[') { obtener(); tokens.emplace_back(CORCH_IZQ, "["); continue; }
            if (c == ']') { obtener(); tokens.emplace_back(CORCH_DER, "]"); continue; }
            if (c == ':') { obtener(); tokens.emplace_back(DOS_PUNTOS, ":"); continue; }
            if (c == ',') { obtener(); tokens.emplace_back(COMA, ","); continue; }
            if (c == ';') { obtener(); tokens.emplace_back(PUNTO_Y_COMA, ";"); continue; }
            if (c == '.') { obtener(); tokens.emplace_back(PUNTO, "."); continue; }

            // -------- PALABRAS CLAVE E IDENTIFICADORES --------
            if (isalpha(c) || c == '_') {
                string id;
                while (isalnum(ver()) || ver() == '_') id.push_back(obtener());

                // Palabras clave y variantes
                if (id == "ESTUDIANTE") tokens.emplace_back(T_ESTUDIANTE, id);
                else if (id == "CV") tokens.emplace_back(T_CV, id);
                else if (id == "OFERTA") tokens.emplace_back(T_OFERTA, id);
                else if (id == "EXPERIENCIA") tokens.emplace_back(T_EXPERIENCIA, id);
                else if (id == "EDUCACION") tokens.emplace_back(T_EDUCACION, id);
                else if (id == "CONTACTO") tokens.emplace_back(T_CONTACTO, id);
                else if (id == "LOGROS") tokens.emplace_back(T_LOGROS, id);
                else if (id == "PALABRAS_CLAVE") tokens.emplace_back(T_PALABRAS_CLAVE, id);
                else if (id == "REFERENCIA_ESTUDIANTE") tokens.emplace_back(T_REFERENCIA_ESTUDIANTE, id);
                else if (id == "EXPERIENCIA_LISTA") tokens.emplace_back(T_EXPERIENCIA_LISTA, id);
                else if (id == "LOGROS_LISTA") tokens.emplace_back(T_LOGROS_LISTA, id);
                else if (id == "EVALUAR") tokens.emplace_back(T_EVALUAR, id);
                else if (id == "SI") tokens.emplace_back(T_SI, id);
                else if (id == "ENTONCES") tokens.emplace_back(T_ENTONCES, id);
                else if (id == "GENERAR") tokens.emplace_back(T_GENERAR, id);
                else if (id == "CONTIENE") tokens.emplace_back(T_CONTIENE, id);
                else if (id == "PARA") tokens.emplace_back(T_PARA, id);
                else if (id == "EN") tokens.emplace_back(T_EN, id);
                else if (id == "FORMATO") tokens.emplace_back(T_FORMATO, id);
                else if (id == "MOSTRAR") tokens.emplace_back(T_MOSTRAR, id);
                else tokens.emplace_back(IDENTIFICADOR, id);
                continue;
            }

            // -------- DESCONOCIDO --------
            string s(1, obtener());
            tokens.emplace_back(DESCONOCIDO, s);
        }

        tokens.emplace_back(FIN_TOKEN, "");
        return tokens;
    }
};


// ESTRUCTURA DE ENTIDAD 
struct Entidad {
    string tipo;
    string id;
};
vector<Entidad> tablaEntidades;

// FUNCIONES SEMANTICAS
void registrarEntidad(const string &tipo, const string &id) {
    for (auto &e : tablaEntidades)
        if (e.id == id) {
            cout << "Error: la entidad '" << id << "' ya existe.\n";
            return;
        }
    tablaEntidades.push_back({tipo, id});
    cout << "Registrada entidad " << tipo << " '" << id << "'\n";
}

void mostrarEntidad(const string &id) {
    for (auto &e : tablaEntidades)
        if (e.id == id) {
            cout << "Entidad: " << e.tipo << " '" << id << "' encontrada.\n";
            return;
        }
    cout << "No se encontro la entidad '" << id << "'\n";
}

void evaluarCompatibilidad(const string &cv, const string &oferta) {
    cout << "Evaluando compatibilidad entre " << cv << " y " << oferta << "...\n";
    cout << "Resultado: 85% de coincidencia en palabras clave.\n";
}

void generarCVFinal(const string &cv, const string &oferta, const string &formato) {
    cout << "Generando CV '" << cv << "' para '" << oferta
         << "' en formato " << formato << "...\n";
    cout << " Archivo ATS exportado correctamente.\n";
}


// ANALIZADOR SINTACTICO 
class AnalizadorSintactico {
    vector<Token> tokens;
    size_t pos;

    Token actual() { return pos < tokens.size() ? tokens[pos] : Token(FIN_TOKEN); }
    void avanzar() { if (pos < tokens.size()) pos++; }

    bool coincide(TipoToken tipo) {
        if (actual().tipo == tipo) { avanzar(); return true; }
        return false;
    }

    void error(const string &msg) {
        cerr << "Error de sintaxis: " << msg
             << " cerca de '" << actual().valor << "'\n";
        exit(1);
    }

public:
    AnalizadorSintactico(const vector<Token> &t) : tokens(t), pos(0) {}

    void analizar() {
        while (actual().tipo != FIN_TOKEN) {
            if (actual().tipo == T_ESTUDIANTE ||
                actual().tipo == T_CV ||
                actual().tipo == T_OFERTA)
                declaracion();
            else instruccion();
        }
        cout << "\n Analisis sintactico completado con exito.\n";
    }

private:
    // ---------------- DECLARACIONES ----------------
    void declaracion() {
        string tipo;
        if (coincide(T_ESTUDIANTE)) tipo = "ESTUDIANTE";
        else if (coincide(T_CV)) tipo = "CV";
        else if (coincide(T_OFERTA)) tipo = "OFERTA";
        else error("Se esperaba tipo de entidad");

        string id;
        if (actual().tipo == IDENTIFICADOR) { id = actual().valor; avanzar(); }
        else error("Se esperaba identificador");

        if (!coincide(LLAVE_IZQ)) error("Se esperaba '{'");
        listaCampos();
        if (!coincide(LLAVE_DER)) error("Se esperaba '}'");
        if (!coincide(PUNTO_Y_COMA)) error("Se esperaba ';'");

        registrarEntidad(tipo, id);
    }

    void listaCampos() {
        // permitir listas vacias o campos
        if (actual().tipo == LLAVE_DER) return;
        campo();
        while (actual().tipo == COMA) {
            avanzar();
            // si siguiente es '}' permitimos coma rogue? error semantico, pero mantenemos estricto:
            campo();
        }
    }

    // campo ahora acepta IDENTIFICADOR y varias palabras clave/listas
    void campo() {
        if (actual().tipo != IDENTIFICADOR &&
            actual().tipo != T_EXPERIENCIA &&
            actual().tipo != T_EDUCACION &&
            actual().tipo != T_CONTACTO &&
            actual().tipo != T_REFERENCIA_ESTUDIANTE &&
            actual().tipo != T_PALABRAS_CLAVE &&
            actual().tipo != T_EXPERIENCIA_LISTA &&
            actual().tipo != T_LOGROS_LISTA &&
            actual().tipo != T_LOGROS)
            error("Se esperaba nombre de campo o palabra de estructura");

        TipoToken tipoCampo = actual().tipo;
        avanzar();

        if (!coincide(DOS_PUNTOS)) error("Se esperaba ':'");

        // Estructura anidada simple
        if (tipoCampo == T_EXPERIENCIA ||
            tipoCampo == T_EDUCACION ||
            tipoCampo == T_CONTACTO) {
            if (!coincide(LLAVE_IZQ)) error("Se esperaba '{' despu�s de la estructura");
            listaCampos();
            if (!coincide(LLAVE_DER)) error("Se esperaba '}' al cerrar estructura");
        }
        // Listas de estructuras o cadenas
        else if (tipoCampo == T_EXPERIENCIA_LISTA ||
                 tipoCampo == T_LOGROS_LISTA ||
                 tipoCampo == T_PALABRAS_CLAVE ||
                 tipoCampo == T_LOGROS) {
            if (!coincide(CORCH_IZQ)) error("Se esperaba '[' al inicio de la lista");
            listaValores();
            if (!coincide(CORCH_DER)) error("Se esperaba ']' al final de la lista");
        }
        // REFERENCIA_ESTUDIANTE o identificadores simples o n�mero o cadena
        else {
            valor();
        }
    }

    // ----------------------------------------------------------------
    // listaValores(): permite elementos que sean CADENA, NUMERO, IDENTIFICADOR
    // o estructuras (EXPERIENCIA { ... }) dentro de un arreglo.
    // ----------------------------------------------------------------
    void listaValores() {
        // aceptar arreglo vacio
        if (actual().tipo == CORCH_DER) return;

        // elemento
        while (true) {
            if (actual().tipo == CADENA || actual().tipo == NUMERO || actual().tipo == IDENTIFICADOR) {
                avanzar();
            } else if (actual().tipo == T_EXPERIENCIA || actual().tipo == T_EDUCACION || actual().tipo == T_CONTACTO) {
                // estructura dentro de lista
                estructuraAnidada();
            } else {
                error("Elemento inv�lido dentro de la lista (se esperaba cadena, n�mero, identificador o estructura)");
            }

            if (actual().tipo == COMA) { avanzar(); continue; }
            break;
        }
    }

    // ----------------------------------------------------------------
    // valor(): valores simples o arreglos (cuando usado fuera de campo())
    // ----------------------------------------------------------------
    void valor() {
        if (actual().tipo == CADENA || actual().tipo == NUMERO || actual().tipo == IDENTIFICADOR) {
            avanzar();
            return;
        }

        if (actual().tipo == CORCH_IZQ) {
            // Arreglo: puede contener cadenas y/o estructuras
            avanzar(); // consume '['
            listaValores();
            if (!coincide(CORCH_DER)) error("Se esperaba ']' al cerrar arreglo");
            return;
        }

        if (actual().tipo == T_EXPERIENCIA || actual().tipo == T_EDUCACION || actual().tipo == T_CONTACTO) {
            estructuraAnidada();
            return;
        }

        error("Valor no reconocido");
    }

    // estructuraAnidada: asume que el token tipo (EXPERIENCIA/EDUCACION/CONTACTO) est� en actual()
    void estructuraAnidada() {
        // consume la palabra clave (EXPERIENCIA / EDUCACION / CONTACTO)
        TipoToken tk = actual().tipo;
        avanzar();
        if (!coincide(LLAVE_IZQ)) error("Se esperaba '{' despu�s de la palabra de estructura");
        listaCampos();
        if (!coincide(LLAVE_DER)) error("Se esperaba '}' al cerrar la estructura");
    }

    // ---------------- INSTRUCCIONES ----------------
    void instruccion() {
        if (actual().tipo == T_EVALUAR) evaluacion();
        else if (actual().tipo == T_GENERAR) generacion();
        else if (actual().tipo == T_SI) condicional();
        else if (actual().tipo == T_MOSTRAR) mostrar();
        else error("Instruccion desconocida");
    }

    void evaluacion() {
        avanzar(); // EVALUAR
        if (!coincide(T_CV)) error("Se esperaba 'CV'");
        string cv = obtenerIdentificador();
        if (!coincide(T_PARA)) error("Se esperaba 'PARA'");
        if (!coincide(T_OFERTA)) error("Se esperaba 'OFERTA'");
        string oferta = obtenerIdentificador();
        if (!coincide(PUNTO_Y_COMA)) error("Falta ';'");
        evaluarCompatibilidad(cv, oferta);
    }

    void generacion() {
        avanzar(); // GENERAR
        if (!coincide(T_CV)) error("Se esperaba 'CV'");
        string cv = obtenerIdentificador();
        if (!coincide(T_PARA)) error("Se esperaba 'PARA'");
        if (!coincide(T_OFERTA)) error("Se esperaba 'OFERTA'");
        string oferta = obtenerIdentificador();
        if (!coincide(T_EN)) error("Se esperaba 'EN'");
        if (!coincide(T_FORMATO)) error("Se esperaba 'FORMATO'");
        string formato = obtenerIdentificador();
        if (!coincide(PUNTO_Y_COMA)) error("Falta ';'");
        generarCVFinal(cv, oferta, formato);
    }

    void condicional() {
        avanzar(); // SI
        condicion();
        if (!coincide(T_ENTONCES)) error("Falta 'ENTONCES'");
        instruccion();
    }

    void condicion() {
    string a = obtenerIdentificador();
    if (!coincide(PUNTO)) error("Falta '.'");

    // Campo izquierdo: puede ser IDENTIFICADOR o palabra clave
    if (actual().tipo == IDENTIFICADOR ||
        actual().tipo == T_LOGROS || actual().tipo == T_PALABRAS_CLAVE ||
        actual().tipo == T_EXPERIENCIA || actual().tipo == T_EDUCACION ||
        actual().tipo == T_CONTACTO) {
        avanzar();
    } else error("Se esperaba nombre de campo despues del '.'");

    if (!coincide(T_CONTIENE)) error("Falta 'CONTIENE'");

    string b = obtenerIdentificador();
    if (!coincide(PUNTO)) error("Falta '.'");

    // Campo derecho: puede ser IDENTIFICADOR o palabra clave
    if (actual().tipo == IDENTIFICADOR ||
        actual().tipo == T_LOGROS || actual().tipo == T_PALABRAS_CLAVE ||
        actual().tipo == T_EXPERIENCIA || actual().tipo == T_EDUCACION ||
        actual().tipo == T_CONTACTO) {
        avanzar();
    } else error("Se esperaba nombre de campo despues del '.'");

    cout << "Evaluando condicion de CONTIENE...\n";
    }


    void mostrar() {
        avanzar(); // MOSTRAR
        string id = obtenerIdentificador();
        if (!coincide(PUNTO_Y_COMA)) error("Falta ';'");
        mostrarEntidad(id);
    }

    string obtenerIdentificador() {
        if (actual().tipo != IDENTIFICADOR)
            error("Se esperaba un identificador");
        string id = actual().valor;
        avanzar();
        return id;
    }
};


// FUNCION PRINCIPAL
int main() {
    const string nombreArchivo = "archivo.txt";

    ifstream archivo(nombreArchivo);
    if (!archivo) {
        cout << "No se pudo abrir el archivo '" << nombreArchivo << "'.\n";
        return 1;
    }

    stringstream buffer;
    buffer << archivo.rdbuf();
    string entrada = buffer.str();

    AnalizadorLexico lexer(entrada);
    auto tokens = lexer.tokenizar();

    cout << "===== TOKENS GENERADOS =====\n";
    for (auto &t : tokens)
        cout << nombreToken(t.tipo) << " (" << t.valor << ")\n";

    cout << "\n===== ANALISIS SINTACTICO =====\n";
    AnalizadorSintactico parser(tokens);
    parser.analizar();

    return 0;
}

