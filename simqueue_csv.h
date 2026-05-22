// ════════════════════════════════════════════════════════════════════
//  simqueue_csv.h  —  Exporta el estado de simulación a CSV
//
//  Columnas de PS:
//    - Siempre: FinServ, Estado
//    - Si descanso==true: IniDescanso, FinDescanso, CantDescansos
//        · Activo==true  → FinDescanso queda en blanco
//        · Activo==false → IniDescanso queda en blanco
//    - Si descanso==false: ninguna columna de descanso
//
//  Columnas de Cola:
//    - Siempre: CantCola
//    - Si desercion==true: Desertados_<cola>
//    - Total_Desertores global siempre aparece
//
//  Como descanso/desercion no cambian tras la inicialización,
//  la cabecera es fija durante toda la simulación.
// ════════════════════════════════════════════════════════════════════
#pragma once
#include <fstream>
#include <string>

static std::string sqcsv_fmtTime(unsigned int s) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%03d:%02d:%02d",
             s / 3600, (s % 3600) / 60, s % 60);
    return buf;
}

namespace SimCSV {

static const char* CSV_FILE    = "sim_data.csv";
static bool        _headerWritten = false;

// ── Cabecera (fija tras la primera llamada) ──────────────────────
static std::string buildHeader() {
    std::string h = "Hora_actual";

    for (size_t i = 0; i < direccionFC.size(); i++)
        h += ",ProxLlegada_FC" + std::to_string(i + 1);

    for (size_t i = 0; i < direccionDeColas.size(); i++) {
        Cola* c = direccionDeColas[i];
        h += ",CantCola_" + c->nombre;
        if (c->desercion)
            h += ",Desertados_" + c->nombre;
    }

    for (size_t i = 0; i < direccionZS.size(); i++) {
        std::string n = direccionZS[i]->nombre;
        h += ",Estado_" + n;
        h += ",FinZS_" + n;
    }

    for (size_t i = 0; i < direccionPS.size(); i++) {
        PuestoServicio* ps = direccionPS[i];
        std::string n = ps->nombre;
        h += ",FinServ_" + n;
        h += ",Estado_" + n;
        if (ps->descanso) {
            h += ",IniDescanso_" + n;
            h += ",FinDescanso_" + n;
            h += ",CantDescansos_" + n;
        }
    }

    h += ",Total_Atendidos,Total_Desertores";
    return h;
}

// ── Fila de datos ────────────────────────────────────────────────
static std::string buildRow(unsigned int tiempo) {
    std::string r = sqcsv_fmtTime(tiempo);

    for (size_t i = 0; i < direccionFC.size(); i++)
        r += "," + sqcsv_fmtTime(direccionFC[i]->t_ProximaLlegada);

    for (size_t i = 0; i < direccionDeColas.size(); i++) {
        Cola* c = direccionDeColas[i];
        r += "," + std::to_string((int)c->clientes.size());
        if (c->desercion)
            r += "," + std::to_string(c->clientesDesertados);
    }

    for (size_t i = 0; i < direccionZS.size(); i++) {
        ZonaSeguridad* zs = direccionZS[i];
        r += std::string(",") + (zs->ocupado ? "OCUP" : "LIBRE");
        r += "," + (zs->ocupado ? sqcsv_fmtTime(zs->t_salida) : std::string("-"));
    }

    for (size_t i = 0; i < direccionPS.size(); i++) {
        PuestoServicio* ps = direccionPS[i];

        r += "," + (ps->ocupado ? sqcsv_fmtTime(ps->t_FinDeServicio) : std::string("-"));

        std::string estado;
        if      (!ps->Activo && ps->descanso) estado = "DESCANSO";
        else if (ps->ocupado)                 estado = "OCUPADO";
        else if (ps->Activo)                  estado = "LIBRE";
        else                                  estado = "INACTIVO";
        r += "," + estado;

        if (ps->descanso) {
            // IniDescanso: solo si está en descanso ahora (Activo==false)
            r += std::string(",") + (!ps->Activo ? sqcsv_fmtTime(ps->t_InicioDescanso) : "");
            // FinDescanso: solo si ya volvió del descanso (Activo==true)
            r += std::string(",") + ( ps->Activo ? sqcsv_fmtTime(ps->t_FinDescanso)   : "");
            r += "," + std::to_string(ps->cantidadDescansos);
        }
    }

    r += "," + std::to_string(contGlobal);
    r += "," + std::to_string(contDesertoresGlobal);
    return r;
}

// ── API ──────────────────────────────────────────────────────────

inline void init(const char* path = nullptr) {
    if (path) CSV_FILE = path;
    std::ofstream f(CSV_FILE, std::ios::trunc);
    _headerWritten = false;
}

inline void escribirFila(unsigned int tiempo) {
    if (!_headerWritten) {
        std::ofstream f(CSV_FILE, std::ios::trunc);
        f << buildHeader() << "\n";
        _headerWritten = true;
    }
    std::ofstream f(CSV_FILE, std::ios::app);
    f << buildRow(tiempo) << "\n";
}

} // namespace SimCSV

inline void mostrarEvento(unsigned int tiempo) { SimCSV::escribirFila(tiempo); }
inline void mostrarEvento() {}
