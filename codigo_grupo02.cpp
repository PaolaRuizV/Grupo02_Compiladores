#include <bits/stdc++.h>
using namespace std;

// ================= FUNCIONES AUXILIARES BASICAS =================

bool terminaCon(const string &s, const string &suf) {
    if (s.size() < suf.size()) return false;
    return s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

bool empiezaCon(const string &s, const string &pref) {
    if (s.size() < pref.size()) return false;
    return s.compare(0, pref.size(), pref) == 0;
}

string trim(const string &s) {
    size_t i = 0, j = s.size();
    while (i < j && isspace((unsigned char)s[i])) i++;
    while (j > i && isspace((unsigned char)s[j - 1])) j--;
    return s.substr(i, j - i);
}

bool esSoloDigitos(const string &s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit((unsigned char)c)) return false;
    return true;
}

// fecha: dd/mm/yyyy
bool esFechaValida(const string &f) {
    if (f.size() != 10) return false;
    if (!isdigit(f[0]) || !isdigit(f[1]) ||
        f[2] != '/' ||
        !isdigit(f[3]) || !isdigit(f[4]) ||
        f[5] != '/' ||
        !isdigit(f[6]) || !isdigit(f[7]) ||
        !isdigit(f[8]) || !isdigit(f[9])) return false;
    return true;
}

// compara fechas dd/mm/yyyy: devuelve true si f1 <= f2
bool fechaMenorOIgual(const string &f1, const string &f2) {
    if (!esFechaValida(f1) || !esFechaValida(f2)) return true; // ya se aviso antes

    int d1 = stoi(f1.substr(0,2));
    int m1 = stoi(f1.substr(3,2));
    int y1 = stoi(f1.substr(6,4));

    int d2 = stoi(f2.substr(0,2));
    int m2 = stoi(f2.substr(3,2));
    int y2 = stoi(f2.substr(6,4));

    if (y1 != y2) return y1 < y2;
    if (m1 != m2) return m1 < m2;
    return d1 <= d2;
}

// fecha actual del sistema en formato dd/mm/yyyy
string fechaActualSistema() {
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    int d = lt->tm_mday;
    int m = lt->tm_mon + 1;
    int y = lt->tm_year + 1900;
    char buf[11];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", d, m, y);
    return string(buf);
}

string toLowerStr(string s) {
    for (auto &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ===================== TOKENS =========================

enum TipoToken {
    // Palabras clave
    T_REGISTRAR, T_ESTUDIANTE, T_ASIGNAR, T_A,
    T_CREAR, T_EVALUAR, T_PARA, T_MOSTRAR,

    // Tokens especificos de valor
    T_CODIGO, T_NOMBRE, T_CARRERA, T_CORREO, T_TELEFONO,
    T_EMPRESA, T_PUESTO, T_FECHA, T_HABILIDAD, T_SUELDO,
    T_ESPECIALIZACION,

    // Genericos
    IDENTIFICADOR, NUMERO, CADENA,
    LLAVE_IZQ, LLAVE_DER,
    DOS_PUNTOS, COMA, PUNTO_Y_COMA,

    FIN_TOKEN, DESCONOCIDO
};

struct Token {
    TipoToken tipo;
    string valor;
    Token(TipoToken t = DESCONOCIDO, const string &v = "") : tipo(t), valor(v) {}
};

string nombreToken(TipoToken t, const string &valor = "") {
    switch (t) {
        case T_REGISTRAR:   return "REGISTRAR";
        case T_ESTUDIANTE:  return "ESTUDIANTE";
        case T_ASIGNAR:     return "ASIGNAR";
        case T_A:           return "A";
        case T_CREAR:       return "CREAR";
        case T_EVALUAR:     return "EVALUAR";
        case T_PARA:        return "PARA";
        case T_MOSTRAR:     return "MOSTRAR";

        case T_CODIGO:          return "CODIGO";
        case T_NOMBRE:          return "NOMBRE";
        case T_CARRERA:         return "CARRERA";
        case T_CORREO:          return "CORREO";
        case T_TELEFONO:        return "TELEFONO";
        case T_EMPRESA:         return "EMPRESA";
        case T_PUESTO:          return "PUESTO";
        case T_FECHA:           return "FECHA";
        case T_HABILIDAD:       return "HABILIDAD";
        case T_SUELDO:          return "SUELDO";
        case T_ESPECIALIZACION: return "ESPECIALIZACION";

        case LLAVE_IZQ:     return "{";
        case LLAVE_DER:     return "}";
        case DOS_PUNTOS:    return ":";
        case COMA:          return ",";
        case PUNTO_Y_COMA:  return ";";
        case NUMERO:        return "NUMERO";
        case CADENA:        return "CADENA";
        case FIN_TOKEN:     return "FIN";

        case IDENTIFICADOR: {
            string vlow = toLowerStr(valor);
            if (vlow == "codigo")      return "CAMPO_CODIGO";
            if (vlow == "nombre")      return "CAMPO_NOMBRE";
            if (vlow == "carrera")     return "CAMPO_CARRERA";
            if (vlow == "correo")      return "CAMPO_CORREO";
            if (vlow == "telefono")    return "CAMPO_TELEFONO";
            if (vlow == "institucion") return "CAMPO_INSTITUCION";
            if (vlow == "campo")       return "CAMPO_CAMPO";
            if (vlow == "fechainicio") return "CAMPO_FECHAINICIO";
            if (vlow == "fechafin")    return "CAMPO_FECHAFIN";
            if (vlow == "empresa")     return "CAMPO_EMPRESA";
            if (vlow == "puesto")      return "CAMPO_PUESTO";
            if (vlow == "habilidades") return "CAMPO_HABILIDADES";
            if (vlow == "sueldo")      return "CAMPO_SUELDO";
            if (vlow == "fechamaxima") return "CAMPO_FECHAMAXIMA";
            return "IDENTIFICADOR";
        }

        default:
            return "DESCONOCIDO";
    }
}

// ================== ANALIZADOR LEXICO =================

class AnalizadorLexico {
    string entrada;
    size_t pos;

    char ver() const { return pos < entrada.size() ? entrada[pos] : '\0'; }
    char ver2() const { return (pos + 1 < entrada.size()) ? entrada[pos + 1] : '\0'; }
    char obtener() { return pos < entrada.size() ? entrada[pos++] : '\0'; }

    void saltarEspaciosYComentarios() {
        while (true) {
            while (isspace((unsigned char)ver())) obtener();

            if (ver() == '/' && ver2() == '/') {
                obtener(); obtener();
                while (ver() != '\n' && ver() != '\0') obtener();
                continue;
            }
            if (ver() == '/' && ver2() == '*') {
                obtener(); obtener();
                while (true) {
                    if (ver() == '\0') break;
                    if (ver() == '*' && ver2() == '/') {
                        obtener(); obtener();
                        break;
                    }
                    obtener();
                }
                continue;
            }
            break;
        }
    }

public:
    AnalizadorLexico(const string &texto) : entrada(texto), pos(0) {}

    vector<Token> tokenizar() {
        vector<Token> tokens;

        while (true) {
            saltarEspaciosYComentarios();
            char c = ver();
            if (c == '\0') break;

            // CADENA entre comillas (por si acaso)
            if (c == '"') {
                obtener();
                string s;
                while (ver() != '"' && ver() != '\0') s.push_back(obtener());
                if (ver() == '"') obtener();
                tokens.emplace_back(CADENA, s);
                continue;
            }

            // NUMEROS y FECHAS (permitimos '/')
            if (isdigit((unsigned char)c)) {
                string num;
                while (isdigit((unsigned char)ver()) || ver() == '/') {
                    num.push_back(obtener());
                }
                tokens.emplace_back(NUMERO, num);
                continue;
            }

            // SIMBOLOS
            if (c == '{') { obtener(); tokens.emplace_back(LLAVE_IZQ, "{"); continue; }
            if (c == '}') { obtener(); tokens.emplace_back(LLAVE_DER, "}"); continue; }
            if (c == ':') { obtener(); tokens.emplace_back(DOS_PUNTOS, ":"); continue; }
            if (c == ',') { obtener(); tokens.emplace_back(COMA, ","); continue; }
            if (c == ';') { obtener(); tokens.emplace_back(PUNTO_Y_COMA, ";"); continue; }

            // IDENTIFICADORES / PALABRAS CLAVE
            if (isalpha((unsigned char)c) || c == '_' || (unsigned char)c >= 128) {
                string id;
                while (isalnum((unsigned char)ver()) ||
                       ver() == '_' ||
                       ver() == '@' ||
                       ver() == '.' ||
                       (unsigned char)ver() >= 128) {
                    id.push_back(obtener());
                }

                string up = id;
                for (auto &ch : up) ch = toupper((unsigned char)ch);

                if (up == "REGISTRAR")         tokens.emplace_back(T_REGISTRAR, id);
                else if (up == "ESTUDIANTE")   tokens.emplace_back(T_ESTUDIANTE, id);
                else if (up == "ASIGNAR")      tokens.emplace_back(T_ASIGNAR, id);
                else if (up == "A")            tokens.emplace_back(T_A, id);
                else if (up == "CREAR")        tokens.emplace_back(T_CREAR, id);
                else if (up == "EVALUAR")      tokens.emplace_back(T_EVALUAR, id);
                else if (up == "PARA")         tokens.emplace_back(T_PARA, id);
                else if (up == "MOSTRAR")      tokens.emplace_back(T_MOSTRAR, id);
                else                           tokens.emplace_back(IDENTIFICADOR, id);
                continue;
            }

            string s(1, obtener());
            tokens.emplace_back(DESCONOCIDO, s);
        }

        tokens.emplace_back(FIN_TOKEN, "");
        return tokens;
    }
};

// ============= POST-PROCESO DE TOKENS (TIPOS ESPECIALES) ============

vector<Token> postprocesarTokens(const vector<Token> &in) {
    vector<Token> out;
    size_t n = in.size();

    auto tieneDosPuntos = [&](size_t idx) {
        return (idx + 1 < n && in[idx + 1].tipo == DOS_PUNTOS);
    };

    auto esFechaLex = [&](const string &s) {
        return esFechaValida(s);
    };

    for (size_t i = 0; i < n; ++i) {
        const Token &t = in[i];

        if (t.tipo == IDENTIFICADOR) {
            string vlow = toLowerStr(t.valor);

            // CODIGO
            if (vlow == "codigo" && tieneDosPuntos(i) &&
                i + 2 < n && in[i + 2].tipo == NUMERO &&
                esSoloDigitos(in[i + 2].valor) && in[i + 2].valor.size() == 7) {

                out.push_back(t);
                out.push_back(in[i + 1]);
                Token val = in[i + 2];
                out.emplace_back(T_CODIGO, val.valor);
                i = i + 2;
                continue;
            }

            // NOMBRE
            if (vlow == "nombre" && tieneDosPuntos(i)) {
                out.push_back(t);
                out.push_back(in[i + 1]);
                size_t k = i + 2;
                string acc;
                while (k < n && in[k].tipo != PUNTO_Y_COMA) {
                    if (!acc.empty()) acc += ' ';
                    acc += in[k].valor;
                    k++;
                }
                if (!acc.empty()) {
                    out.emplace_back(T_NOMBRE, acc);
                    i = k - 1;
                    continue;
                }
            }

            // CARRERA
            if (vlow == "carrera" && tieneDosPuntos(i)) {
                out.push_back(t);
                out.push_back(in[i + 1]);
                size_t k = i + 2;
                string acc;
                while (k < n && in[k].tipo != PUNTO_Y_COMA) {
                    if (!acc.empty()) acc += ' ';
                    acc += in[k].valor;
                    k++;
                }
                if (!acc.empty()) {
                    out.emplace_back(T_CARRERA, acc);
                    i = k - 1;
                    continue;
                }
            }

            // CORREO
            if (vlow == "correo" && tieneDosPuntos(i) &&
                i + 2 < n && in[i + 2].tipo == IDENTIFICADOR &&
                in[i + 2].valor.find('@') != string::npos) {

                out.push_back(t);
                out.push_back(in[i + 1]);
                Token val = in[i + 2];
                out.emplace_back(T_CORREO, val.valor);
                i = i + 2;
                continue;
            }

            // TELEFONO
            if (vlow == "telefono" && tieneDosPuntos(i) &&
                i + 2 < n && in[i + 2].tipo == NUMERO &&
                esSoloDigitos(in[i + 2].valor) &&
                in[i + 2].valor.size() == 9) {

                out.push_back(t);
                out.push_back(in[i + 1]);
                Token val = in[i + 2];
                out.emplace_back(T_TELEFONO, val.valor);
                i = i + 2;
                continue;
            }

            // EMPRESA
            if (vlow == "empresa" && tieneDosPuntos(i)) {
                out.push_back(t);
                out.push_back(in[i + 1]);
                size_t k = i + 2;
                string acc;
                while (k < n && in[k].tipo != PUNTO_Y_COMA) {
                    if (!acc.empty()) acc += ' ';
                    acc += in[k].valor;
                    k++;
                }
                if (!acc.empty()) {
                    out.emplace_back(T_EMPRESA, acc);
                    i = k - 1;
                    continue;
                }
            }

            // PUESTO
            if (vlow == "puesto" && tieneDosPuntos(i)) {
                out.push_back(t);
                out.push_back(in[i + 1]);
                size_t k = i + 2;
                string acc;
                while (k < n && in[k].tipo != PUNTO_Y_COMA) {
                    if (!acc.empty()) acc += ' ';
                    acc += in[k].valor;
                    k++;
                }
                if (!acc.empty()) {
                    out.emplace_back(T_PUESTO, acc);
                    i = k - 1;
                    continue;
                }
            }

            // ESPECIALIZACION (valor de campo:)
            if (vlow == "campo" && tieneDosPuntos(i)) {
                out.push_back(t);
                out.push_back(in[i + 1]);
                size_t k = i + 2;
                string acc;
                while (k < n && in[k].tipo != PUNTO_Y_COMA) {
                    if (!acc.empty()) acc += ' ';
                    acc += in[k].valor;
                    k++;
                }
                if (!acc.empty()) {
                    out.emplace_back(T_ESPECIALIZACION, acc);
                    i = k - 1;
                    continue;
                }
            }

            // FECHAS
            if ((vlow == "fechainicio" || vlow == "fechafin" || vlow == "fechamaxima") &&
                tieneDosPuntos(i) &&
                i + 2 < n && in[i + 2].tipo == NUMERO &&
                esFechaLex(in[i + 2].valor)) {

                out.push_back(t);
                out.push_back(in[i + 1]);
                Token val = in[i + 2];
                out.emplace_back(T_FECHA, val.valor);
                i = i + 2;
                continue;
            }

            // HABILIDADES
            if (vlow == "habilidades" && tieneDosPuntos(i)) {
                out.push_back(t);
                out.push_back(in[i + 1]);
                size_t k = i + 2;

                while (k < n && in[k].tipo != PUNTO_Y_COMA) {
                    if (in[k].tipo == COMA) {
                        out.push_back(in[k]);
                        k++;
                        continue;
                    }
                    string cur;
                    while (k < n &&
                           in[k].tipo != COMA &&
                           in[k].tipo != PUNTO_Y_COMA) {
                        if (!cur.empty()) cur += ' ';
                        cur += in[k].valor;
                        k++;
                    }
                    cur = trim(cur);
                    if (!cur.empty()) {
                        out.emplace_back(T_HABILIDAD, cur);
                    }
                }
                i = k - 1;
                continue;
            }

            // SUELDO
            if (vlow == "sueldo" && tieneDosPuntos(i) &&
                i + 2 < n && in[i + 2].tipo == NUMERO &&
                esSoloDigitos(in[i + 2].valor)) {

                out.push_back(t);
                out.push_back(in[i + 1]);
                Token val = in[i + 2];
                out.emplace_back(T_SUELDO, val.valor);
                i = i + 2;
                continue;
            }
        }

        out.push_back(t);
    }

    return out;
}

// ================== MODELOS DE DATOS ===================

struct Estudiante {
    string codigo;
    string nombre;
    string carrera;
    string correo;
    string telefono;
};

struct Educacion {
    string id;
    string cvId;
    string institucion;
    string campo;
    string fechainicio;
    string fechafin;
};

struct Experiencia {
    string id;
    string cvId;
    string empresa;
    string puesto;
    string fechainicio;
    string fechafin;
    vector<string> habilidades;
};

struct Oferta {
    string id;
    string empresa;
    string puesto;
    vector<string> habilidades;
    int sueldo = 0;
    string fechamaxima;
};

struct CV {
    string id;
    vector<string> educaciones;
    vector<string> experiencias;
};

struct Postulacion {
    string id;
    vector<string> ofertas;
};

map<string, Estudiante> estudiantes;
map<string, Educacion> educaciones;
map<string, Experiencia> experiencias;
map<string, Oferta> ofertas;
map<string, CV> cvs;
map<string, Postulacion> postulaciones;

// ================= FUNCIONES DE APOYO EXTRA ==================

bool esCodigoEstudianteValido(const string &codigo) {
    return codigo.size() == 7 && esSoloDigitos(codigo);
}

bool esTelefonoValido(const string &t) {
    return t.size() == 9 && esSoloDigitos(t) && t[0] == '9';
}

string extraerCodigoDesdeCV(const string &cvId) {
    if (!empiezaCon(cvId, "CV_")) return "";
    return cvId.substr(3);
}

string construirPTDesdeCV(const string &cvId) {
    string cod = extraerCodigoDesdeCV(cvId);
    if (cod.empty()) return "";
    return "PT_" + cod;
}

bool ofertaActiva(const Oferta &o) {
    if (o.fechamaxima.empty() || !esFechaValida(o.fechamaxima)) return true;
    string hoy = fechaActualSistema();
    if (!esFechaValida(hoy)) return true; // fallback
    return fechaMenorOIgual(hoy, o.fechamaxima); // hoy <= fechamaxima
}

// =================== FUNCIONES SEMANTICAS ==============

void registrarEstudianteEnTabla(Estudiante e) {
    if (!esCodigoEstudianteValido(e.codigo)) {
        cout << "Error: codigo de estudiante '" << e.codigo
             << "' invalido (debe tener 7 digitos).\n";
    }

    string correoTrim = trim(e.correo);
    string correoLower = correoTrim;
    for (auto &ch : correoLower) ch = (char)tolower((unsigned char)ch);
    if (!terminaCon(correoLower, "@usil.pe")) {
        cout << "Advertencia: correo '" << e.correo
             << "' no termina en @usil.pe\n";
    } else {
        e.correo = correoTrim;
    }

    if (!esTelefonoValido(e.telefono)) {
        cout << "Advertencia: telefono '" << e.telefono
             << "' invalido (debe tener 9 digitos y empezar en 9).\n";
    }

    estudiantes[e.codigo] = e;
    cout << "Estudiante " << e.codigo << " registrado.\n";
}

void registrarOfertaEnTabla(const Oferta &o) {
    if (!empiezaCon(o.id, "OF_")) {
        cout << "Advertencia: identificador de oferta '" << o.id
             << "' no empieza con 'OF_'.\n";
    }
    ofertas[o.id] = o;
    cout << "Oferta " << o.id << " registrada para puesto '" << o.puesto << "'.\n";
}

void registrarEducacionEnTabla(const Educacion &ed, bool aplicar) {
    if (!aplicar) return;
    educaciones[ed.id] = ed;
    cvs[ed.cvId].id = ed.cvId;
    cvs[ed.cvId].educaciones.push_back(ed.id);
    cout << "Educacion " << ed.id << " asignada a " << ed.cvId << ".\n";
}

void registrarExperienciaEnTabla(const Experiencia &ex, bool aplicar) {
    if (!aplicar) return;
    experiencias[ex.id] = ex;
    cvs[ex.cvId].id = ex.cvId;
    cvs[ex.cvId].experiencias.push_back(ex.id);
    cout << "Experiencia " << ex.id << " asignada a " << ex.cvId << ".\n";
}

void registrarPostulacion(const string &ofId, const string &ptId) {
    if (!postulaciones.count(ptId)) {
        cout << "Error: la postulacion '" << ptId
             << "' no existe. Solo se crea automaticamente al generar el CV ATS.\n";
        return;
    }
    if (!ofertas.count(ofId)) {
        cout << "Error: la oferta '" << ofId << "' no existe.\n";
        return;
    }
    const Oferta &o = ofertas[ofId];
    if (!ofertaActiva(o)) {
        cout << "Error: la oferta " << ofId
             << " ya no esta activa (fechamaxima " << o.fechamaxima << ").\n";
        return;
    }
    postulaciones[ptId].ofertas.push_back(ofId);
    cout << "Oferta " << ofId << " asignada a postulacion " << ptId << ".\n";
}

void crearCVVacio(const string &cvId) {
    if (!empiezaCon(cvId, "CV_")) {
        cout << "Advertencia: identificador de CV '" << cvId
             << "' no empieza con 'CV_'.\n";
    }
    string codigo = extraerCodigoDesdeCV(cvId);
    if (codigo.empty() || !estudiantes.count(codigo)) {
        cout << "Error: no se puede crear " << cvId
             << " porque el estudiante con codigo '" << codigo
             << "' no ha sido registrado.\n";
        return;
    }
    cvs[cvId].id = cvId;
    cout << "CV " << cvId << " creado con campos vacios.\n";
}

// ------- HTML ATS ---------

void generarCVHtml(const string &cvId, const string &ofId) {
    string nombreArchivo = cvId + string("_ATS.html");
    ofstream out(nombreArchivo);
    if (!out) {
        cout << "No se pudo generar el archivo HTML " << nombreArchivo << ".\n";
        return;
    }

    // Obtener datos del estudiante
    string cod = extraerCodigoDesdeCV(cvId);
    const Estudiante* est = nullptr;
    auto itEst = estudiantes.find(cod);
    if (itEst != estudiantes.end()) {
        est = &itEst->second;
    }

    string nombreEst = est ? est->nombre : cvId;
    string correoEst = est ? est->correo : "";
    string telEst    = est ? est->telefono : "";

    // Construir set de habilidades totales del CV
    set<string> habCV;
    if (cvs.count(cvId)) {
        auto &cv = cvs[cvId];
        for (const auto &exId : cv.experiencias) {
            auto it = experiencias.find(exId);
            if (it == experiencias.end()) continue;
            for (const auto &h : it->second.habilidades) {
                habCV.insert(h);
            }
        }
    }

    // Escribimos HTML
    out << "<!DOCTYPE html>\n";
    out << "<html lang=\"es\">\n";
    out << "<head>\n";
    out << "<meta charset=\"UTF-8\">\n";
    out << "<title>CV - " << nombreEst << "</title>\n";
    out << "<style>\n";
    out << "    body {\n";
    out << "        font-family: Arial, sans-serif;\n";
    out << "        margin: 40px;\n";
    out << "        color: #222;\n";
    out << "    }\n\n";
    out << "    h1 {\n";
    out << "        text-align: center;\n";
    out << "        font-size: 32px;\n";
    out << "        margin-bottom: 5px;\n";
    out << "    }\n\n";
    out << "    .contacto {\n";
    out << "        text-align: center;\n";
    out << "        font-size: 16px;\n";
    out << "        margin-bottom: 30px;\n";
    out << "    }\n\n";
    out << "    .section-title {\n";
    out << "        text-align: center;\n";
    out << "        font-weight: bold;\n";
    out << "        font-size: 18px;\n";
    out << "        margin-top: 40px;\n";
    out << "        margin-bottom: 10px;\n";
    out << "    }\n\n";
    out << "    hr {\n";
    out << "        border: none;\n";
    out << "        border-top: 1px solid #888;\n";
    out << "        margin-bottom: 20px;\n";
    out << "        width: 100%;\n";
    out << "    }\n\n";
    out << "    .subtitulo {\n";
    out << "        font-weight: bold;\n";
    out << "        margin-bottom: 5px;\n";
    out << "    }\n\n";
    out << "    ul {\n";
    out << "        margin-top: 5px;\n";
    out << "    }\n";
    out << "</style>\n";
    out << "</head>\n";
    out << "<body>\n\n";

    // Header
    out << "<h1>" << nombreEst << "</h1>\n";
    out << "<div class=\"contacto\">\n";
    if (!correoEst.empty()) {
        out << "    " << correoEst;
        if (!telEst.empty()) out << " | " << telEst;
        out << "\n";
    } else if (!telEst.empty()) {
        out << "    " << telEst << "\n";
    }
    out << "</div>\n\n";

    // EDUCACION
    out << "<div class=\"section-title\">EDUCACION</div>\n";
    out << "<hr>\n\n";

    if (cvs.count(cvId) && !cvs[cvId].educaciones.empty()) {
        for (const auto &edId : cvs[cvId].educaciones) {
            auto itEd = educaciones.find(edId);
            if (itEd == educaciones.end()) continue;
            auto &ed = itEd->second;

            out << "<div class=\"subtitulo\">" << ed.institucion << "</div>\n";
            out << "<p>" << ed.campo << " | " << ed.fechainicio
                << " - " << (ed.fechafin.empty() ? "Actual" : ed.fechafin) << "</p>\n\n";
        }
    } else {
        out << "<p>None</p>\n\n";
    }

    // EXPERIENCIA
    out << "<div class=\"section-title\">EXPERIENCIA</div>\n";
    out << "<hr>\n\n";

    if (cvs.count(cvId) && !cvs[cvId].experiencias.empty()) {
        for (const auto &exId : cvs[cvId].experiencias) {
            auto itEx = experiencias.find(exId);
            if (itEx == experiencias.end()) continue;
            auto &ex = itEx->second;

            out << "<div class=\"subtitulo\">" << ex.empresa << "</div>\n";
            out << "<p>" << ex.puesto << " | " << ex.fechainicio
                << " - " << (ex.fechafin.empty() ? "Actual" : ex.fechafin) << "</p>\n\n";
        }
    } else {
        out << "<p>None</p>\n\n";
    }

    // HABILIDADES
    out << "<div class=\"section-title\">HABILIDADES</div>\n";
    out << "<hr>\n\n";

    out << "<ul>\n";
    if (!habCV.empty()) {
        for (const auto &h : habCV) {
            out << "    <li>" << h << "</li>\n";
        }
    }
    out << "</ul>\n\n";

    out << "</body>\n</html>\n";

    out.close();
    cout << "CV ATS HTML generado: " << nombreArchivo << "\n";
}

void evaluarCompatibilidadCVOferta(const string &cvId, const string &ofId) {
    if (!cvs.count(cvId)) {
        cout << "No existe CV " << cvId << ".\n";
        return;
    }
    if (!ofertas.count(ofId)) {
        cout << "No existe oferta " << ofId << ".\n";
        return;
    }

    auto &of = ofertas[ofId];
    if (!ofertaActiva(of)) {
        cout << "Error: la oferta " << ofId
             << " ya no esta activa (fechamaxima " << of.fechamaxima << ").\n";
        return;
    }

    string cod = extraerCodigoDesdeCV(cvId);
    if (cod.empty() || !estudiantes.count(cod)) {
        cout << "Error: no se puede evaluar " << cvId
             << " porque el estudiante con codigo '" << cod
             << "' no ha sido registrado.\n";
        return;
    }

    auto &cv = cvs[cvId];

    set<string> habOferta(of.habilidades.begin(), of.habilidades.end());
    set<string> habCV;

    for (const auto &exId : cv.experiencias) {
        auto it = experiencias.find(exId);
        if (it == experiencias.end()) continue;
        for (const auto &h : it->second.habilidades) {
            habCV.insert(h);
        }
    }

    if (habOferta.empty()) {
        cout << "La oferta " << ofId << " no tiene habilidades definidas.\n";
        return;
    }

    int coincidencias = 0;
    for (const auto &h : habOferta) {
        if (habCV.count(h)) coincidencias++;
    }

    double porcentaje = (100.0 * coincidencias) / habOferta.size();
    cout << "Coincidencia de habilidades entre " << cvId << " y " << ofId
         << ": " << coincidencias << "/" << habOferta.size()
         << " (" << porcentaje << "%)\n";

    if (porcentaje >= 70.0) {
        cout << "Porcentaje >= 70%. Se genera CV ATS HTML.\n";
        generarCVHtml(cvId, ofId);

        string ptId = construirPTDesdeCV(cvId);
        if (!ptId.empty()) {
            if (!postulaciones.count(ptId)) {
                Postulacion p;
                p.id = ptId;
                postulaciones[ptId] = p;
                cout << "Postulacion " << ptId
                     << " creada automaticamente para el estudiante "
                     << cod << ".\n";
            }
        }
    } else {
        cout << "Porcentaje < 70%. No se genera CV ATS HTML.\n";
    }
}

// ------ mostrar CV en consola + mostrar PT -------

void mostrarCV(const string &cvId) {
    if (!cvs.count(cvId)) {
        cout << "CV no encontrado.\n";
        return;
    }

    string cod = extraerCodigoDesdeCV(cvId);
    const Estudiante* est = nullptr;
    auto itEst = estudiantes.find(cod);
    if (itEst != estudiantes.end()) {
        est = &itEst->second;
    }

    auto &cv = cvs[cvId];

    cout << (est ? est->nombre : cvId) << "\n";

    if (est) {
        bool printed = false;
        if (!est->correo.empty()) {
            cout << est->correo;
            printed = true;
        }
        if (!est->telefono.empty()) {
            if (printed) cout << " | ";
            cout << est->telefono;
            printed = true;
        }
        if (printed) cout << "\n";
    }
    cout << "EDUCACION\n";

    if (cv.educaciones.empty()) {
        cout << "No se ha registrado educacion\n\n";
    } else {
        for (auto &edId : cv.educaciones) {
            auto itEd = educaciones.find(edId);
            if (itEd == educaciones.end()) continue;
            auto &ed = itEd->second;
            cout << ed.institucion << " | " << ed.campo
                 << " | " << ed.fechainicio << " - "
                 << (ed.fechafin.empty() ? "Actual" : ed.fechafin) << "\n";
        }
        cout << "\n";
    }

    cout << "EXPERIENCIA\n";
    if (cv.experiencias.empty()) {
        cout << "No se ha registrado experiencia\n\n";
    } else {
        for (auto &exId : cv.experiencias) {
            auto itEx = experiencias.find(exId);
            if (itEx == experiencias.end()) continue;
            auto &ex = itEx->second;
            cout << "- " << ex.empresa << " | " << ex.puesto
                 << " | " << ex.fechainicio << " - "
                 << (ex.fechafin.empty() ? "Actual" : ex.fechafin) << "\n";
        }
        cout << "\n";
    }

    set<string> habCV;
    for (auto &exId : cv.experiencias) {
        auto itEx = experiencias.find(exId);
        if (itEx == experiencias.end()) continue;
        for (auto &h : itEx->second.habilidades) {
            habCV.insert(h);
        }
    }

    cout << "HABILIDADES\n";
    if (habCV.empty()) {
        cout << "No se han registrado habilidades\n";
    } else {
        for (auto &h : habCV) {
            cout << "- " << h << "\n";
        }
    }
}

void mostrarPostulacion(const string &ptId) {
    cout << "Postulaciones \n";

    auto it = postulaciones.find(ptId);
    if (it == postulaciones.end()) {
        cout << "No se ha encontrado la postulacion.\n";
        return;
    }

    auto &p = it->second;
    if (p.ofertas.empty()) {
        cout << "No se han asignado ofertas a esta postulacion.\n";
        return;
    }

    cout << "Ofertas asignadas:\n";
    for (const auto &ofId : p.ofertas) {
        cout << "- " << ofId;
        auto itOf = ofertas.find(ofId);
        if (itOf != ofertas.end()) {
            cout << " | " << itOf->second.empresa
                 << " | " << itOf->second.puesto;
        }
        cout << "\n";
    }
}

// ================= ANALIZADOR SINTACTICO ===============

class AnalizadorSintactico {
    vector<Token> tokens;
    size_t pos;

    Token actual() const { return pos < tokens.size() ? tokens[pos] : Token(FIN_TOKEN); }
    void avanzar() { if (pos < tokens.size()) pos++; }

    bool coincide(TipoToken t) {
        if (actual().tipo == t) { avanzar(); return true; }
        return false;
    }

    void consumir(TipoToken t, const string &msg) {
        if (!coincide(t)) error(msg);
    }

    [[noreturn]] void error(const string &msg) const {
        cerr << "Error de sintaxis: " << msg
             << " cerca de '" << actual().valor << "'\n";
        exit(1);
    }

    string obtenerIdentificador() {
        if (actual().tipo != IDENTIFICADOR)
            error("Se esperaba un identificador");
        string v = actual().valor;
        avanzar();
        return v;
    }

    string leerHastaPuntoYComa() {
        string res;
        while (actual().tipo != PUNTO_Y_COMA && actual().tipo != FIN_TOKEN) {
            if (!res.empty()) res += ' ';
            res += actual().valor;
            avanzar();
        }
        return trim(res);
    }

public:
    AnalizadorSintactico(const vector<Token> &t) : tokens(t), pos(0) {}

    void analizar() {
        while (actual().tipo != FIN_TOKEN) {
            sentencia();
        }
        cout << "\nAnalisis sintactico completado.\n";
    }

private:
    void sentencia() {
        if (actual().tipo == T_REGISTRAR) {
            sentenciaRegistrar();
        } else if (actual().tipo == T_ASIGNAR) {
            sentenciaAsignar();
        } else if (actual().tipo == T_CREAR) {
            sentenciaCrear();
        } else if (actual().tipo == T_EVALUAR) {
            sentenciaEvaluar();
        } else if (actual().tipo == T_MOSTRAR) {
            sentenciaMostrar();
        } else if (actual().tipo == FIN_TOKEN) {
            return;
        } else {
            error("Sentencia desconocida");
        }
    }

    void sentenciaRegistrar() {
        consumir(T_REGISTRAR, "Se esperaba 'registrar'");
        if (actual().tipo == T_ESTUDIANTE) {
            avanzar();
            registrarEstudiante();
        } else {
            string id = obtenerIdentificador();
            registrarOferta(id);
        }
    }

    void registrarEstudiante() {
        Estudiante e;
        consumir(LLAVE_IZQ, "Se esperaba '{' en registrar estudiante");

        while (!coincide(LLAVE_DER)) {
            string campo = obtenerIdentificador();
            consumir(DOS_PUNTOS, "Se esperaba ':'");

            if (campo == "codigo") {
                if (actual().tipo == T_CODIGO || actual().tipo == NUMERO) {
                    e.codigo = actual().valor;
                    avanzar();
                } else {
                    e.codigo = leerHastaPuntoYComa();
                }
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de codigo");
            } else if (campo == "nombre") {
                if (actual().tipo == T_NOMBRE) {
                    e.nombre = actual().valor;
                    avanzar();
                } else {
                    e.nombre = leerHastaPuntoYComa();
                }
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de nombre");
            } else if (campo == "carrera") {
                if (actual().tipo == T_CARRERA) {
                    e.carrera = actual().valor;
                    avanzar();
                } else {
                    e.carrera = leerHastaPuntoYComa();
                }
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de carrera");
            } else if (campo == "correo") {
                if (actual().tipo == T_CORREO) {
                    e.correo = actual().valor;
                    avanzar();
                } else {
                    e.correo = leerHastaPuntoYComa();
                }
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de correo");
            } else if (campo == "telefono") {
                if (actual().tipo == T_TELEFONO) {
                    e.telefono = actual().valor;
                    avanzar();
                } else {
                    e.telefono = leerHastaPuntoYComa();
                }
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de telefono");
            } else {
                (void)leerHastaPuntoYComa();
                consumir(PUNTO_Y_COMA, "Se esperaba ';' en campo desconocido");
            }
        }
        consumir(PUNTO_Y_COMA, "Se esperaba ';' al final del bloque estudiante");
        registrarEstudianteEnTabla(e);
    }

    void registrarOferta(const string &idOferta) {
        Oferta o;
        o.id = idOferta;

        consumir(LLAVE_IZQ, "Se esperaba '{' en registrar oferta");

        while (!coincide(LLAVE_DER)) {
            string campo = obtenerIdentificador();
            consumir(DOS_PUNTOS, "Se esperaba ':'");

            if (campo == "empresa") {
                if (actual().tipo == T_EMPRESA) {
                    o.empresa = actual().valor;
                    avanzar();
                } else {
                    o.empresa = leerHastaPuntoYComa();
                }
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de empresa");
            } else if (campo == "puesto") {
                if (actual().tipo == T_PUESTO) {
                    o.puesto = actual().valor;
                    avanzar();
                } else {
                    o.puesto = leerHastaPuntoYComa();
                }
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de puesto");
            } else if (campo == "habilidades") {
                vector<string> habs;
                while (true) {
                    if (actual().tipo == T_HABILIDAD) {
                        habs.push_back(actual().valor);
                        avanzar();
                    } else if (actual().tipo == IDENTIFICADOR) {
                        string cur = leerHastaPuntoYComa();
                        if (!cur.empty()) habs.push_back(cur);
                        break;
                    }

                    if (actual().tipo == COMA) {
                        avanzar();
                        continue;
                    }
                    if (actual().tipo == PUNTO_Y_COMA) break;
                }
                o.habilidades = habs;
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de habilidades");
            } else if (campo == "sueldo") {
                if (actual().tipo == T_SUELDO || actual().tipo == NUMERO) {
                    o.sueldo = stoi(actual().valor);
                    avanzar();
                } else {
                    string s = leerHastaPuntoYComa();
                    if (!s.empty()) o.sueldo = stoi(s);
                }
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de sueldo");
            } else if (campo == "fechamaxima") {
                string v;
                if (actual().tipo == T_FECHA) {
                    v = actual().valor;
                    avanzar();
                } else {
                    v = leerHastaPuntoYComa();
                }
                if (!esFechaValida(v) && !v.empty()) {
                    cout << "Advertencia: fechamaxima '" << v
                         << "' no cumple formato dd/mm/yyyy.\n";
                }
                o.fechamaxima = v;
                consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de fechamaxima");
            } else {
                (void)leerHastaPuntoYComa();
                consumir(PUNTO_Y_COMA, "Se esperaba ';' en campo desconocido de oferta");
            }
        }
        consumir(PUNTO_Y_COMA, "Se esperaba ';' al final del bloque oferta");
        registrarOfertaEnTabla(o);
    }

    void sentenciaAsignar() {
        consumir(T_ASIGNAR, "Se esperaba 'asignar'");
        string id1 = obtenerIdentificador();
        consumir(T_A, "Se esperaba 'a'");
        string id2 = obtenerIdentificador();

        if (actual().tipo == LLAVE_IZQ) {
            avanzar();

            bool esEdu = false, esExp = false;
            Educacion ed;
            Experiencia ex;
            ed.id = id1;
            ex.id = id1;
            ed.cvId = id2;
            ex.cvId = id2;

            bool aplicarSemantica = true;
            string cod = extraerCodigoDesdeCV(id2);
            if (cod.empty() || !estudiantes.count(cod)) {
                cout << "Error: no se puede asignar '" << id1 << "' a " << id2
                     << " porque el estudiante con codigo '" << cod
                     << "' no ha sido registrado.\n";
                aplicarSemantica = false;
            }

            while (!coincide(LLAVE_DER)) {
                string campo = obtenerIdentificador();
                consumir(DOS_PUNTOS, "Se esperaba ':'");

                if (campo == "institucion") {
                    esEdu = true;
                    ed.institucion = leerHastaPuntoYComa();
                    consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de institucion");
                } else if (campo == "campo") {
                    esEdu = true;
                    if (actual().tipo == T_ESPECIALIZACION) {
                        ed.campo = actual().valor;
                        avanzar();
                    } else {
                        ed.campo = leerHastaPuntoYComa();
                    }
                    consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de campo");
                } else if (campo == "empresa") {
                    esExp = true;
                    if (actual().tipo == T_EMPRESA) {
                        ex.empresa = actual().valor;
                        avanzar();
                    } else {
                        ex.empresa = leerHastaPuntoYComa();
                    }
                    consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de empresa");
                } else if (campo == "puesto") {
                    esExp = true;
                    if (actual().tipo == T_PUESTO) {
                        ex.puesto = actual().valor;
                        avanzar();
                    } else {
                        ex.puesto = leerHastaPuntoYComa();
                    }
                    consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de puesto");
                } else if (campo == "fechainicio" || campo == "fechafin") {
                    string v;
                    if (actual().tipo == T_FECHA) {
                        v = actual().valor;
                        avanzar();
                    } else {
                        v = leerHastaPuntoYComa();
                    }
                    if (!v.empty() && !esFechaValida(v)) {
                        cout << "Advertencia: " << campo << " '" << v
                             << "' no cumple formato dd/mm/yyyy.\n";
                    }
                    if (campo == "fechainicio") {
                        ed.fechainicio = v;
                        ex.fechainicio = v;
                    } else {
                        ed.fechafin = v;
                        ex.fechafin = v;
                    }
                    consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de fecha");
                } else if (campo == "habilidades") {
                    esExp = true;
                    vector<string> habs;
                    while (true) {
                        if (actual().tipo == T_HABILIDAD) {
                            habs.push_back(actual().valor);
                            avanzar();
                        } else if (actual().tipo == IDENTIFICADOR) {
                            string cur = leerHastaPuntoYComa();
                            if (!cur.empty()) habs.push_back(cur);
                            break;
                        }

                        if (actual().tipo == COMA) {
                            avanzar();
                            continue;
                        }
                        if (actual().tipo == PUNTO_Y_COMA) break;
                    }
                    ex.habilidades = habs;
                    consumir(PUNTO_Y_COMA, "Se esperaba ';' despues de habilidades");
                } else {
                    (void)leerHastaPuntoYComa();
                    consumir(PUNTO_Y_COMA, "Se esperaba ';' en campo desconocido de asignar");
                }
            }

            // Validacion de rangos de fechas (modo estricto)
            if (esEdu && !ed.fechainicio.empty() && !ed.fechafin.empty()) {
                if (!fechaMenorOIgual(ed.fechainicio, ed.fechafin)) {
                    cout << "Error: en EDU " << ed.id
                         << " fechainicio (" << ed.fechainicio
                         << ") es mayor que fechafin (" << ed.fechafin
                         << "). No se registrara la educacion.\n";
                    aplicarSemantica = false;
                }
            }
            if (esExp && !ex.fechainicio.empty() && !ex.fechafin.empty()) {
                if (!fechaMenorOIgual(ex.fechainicio, ex.fechafin)) {
                    cout << "Error: en EXP " << ex.id
                         << " fechainicio (" << ex.fechainicio
                         << ") es mayor que fechafin (" << ex.fechafin
                         << "). No se registrara la experiencia.\n";
                    aplicarSemantica = false;
                }
            }

            consumir(PUNTO_Y_COMA, "Se esperaba ';' al final del bloque asignar");

            if (esEdu && !esExp) {
                registrarEducacionEnTabla(ed, aplicarSemantica);
            } else if (esExp) {
                registrarExperienciaEnTabla(ex, aplicarSemantica);
            } else {
                cout << "Asignacion " << id1 << " a " << id2
                     << " sin poder determinar si es educacion o experiencia.\n";
            }

        } else if (actual().tipo == PUNTO_Y_COMA) {
            avanzar();
            registrarPostulacion(id1, id2);
        } else {
            error("Se esperaba '{' o ';' despues de 'asignar ... a ...'");
        }
    }

    void sentenciaCrear() {
        consumir(T_CREAR, "Se esperaba 'crear'");
        string cvId = obtenerIdentificador();
        consumir(PUNTO_Y_COMA, "Se esperaba ';' al final de 'crear'");
        crearCVVacio(cvId);
    }

    void sentenciaEvaluar() {
        consumir(T_EVALUAR, "Se esperaba 'evaluar'");
        string cvId = obtenerIdentificador();
        consumir(T_PARA, "Se esperaba 'para'");
        string ofId = obtenerIdentificador();
        consumir(PUNTO_Y_COMA, "Se esperaba ';' al final de 'evaluar'");
        evaluarCompatibilidadCVOferta(cvId, ofId);
    }

    void sentenciaMostrar() {
        consumir(T_MOSTRAR, "Se esperaba 'mostrar'");
        string id = obtenerIdentificador();
        consumir(PUNTO_Y_COMA, "Se esperaba ';' al final de 'mostrar'");

        if (empiezaCon(id, "CV_")) {
            mostrarCV(id);
        } else if (empiezaCon(id, "PT_")) {
            mostrarPostulacion(id);
        } else {
            cout << "No se reconoce el tipo de identificador '" << id
                 << "' para 'mostrar'. Usa CV_... o PT_...\n";
        }
    }
};

// ======================== MAIN ========================

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
    tokens = postprocesarTokens(tokens);

    cout << "===== TOKENS GENERADOS =====\n";
    for (auto &t : tokens) {
        cout << nombreToken(t.tipo, t.valor) << " (" << t.valor << ")\n";
    }

    cout << "\n===== ANALISIS SINTACTICO =====\n";
    AnalizadorSintactico parser(tokens);
    parser.analizar();

    return 0;
}
