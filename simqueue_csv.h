// ════════════════════════════════════════════════════════════════════
//  simqueue_csv.h  —  Exporta el estado de simulación a CSV
// ════════════════════════════════════════════════════════════════════
#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

static std::string sqcsv_fmtTime(unsigned int s) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%03d:%02d:%02d",
             s / 3600, (s % 3600) / 60, s % 60);
    return buf;
}

namespace SimCSV {

static const char* CSV_FILE      = "sim_data.csv";
static bool        _headerWritten = false;

// ── ¿Alguna cola tiene deserción? ───────────────────────────────
static bool hayDesercion() {
    for (auto c : direccionDeColas)
        if (c->desercion) return true;
    return false;
}

// ── Recolecta los 4 t_salidaCola más próximos a 'tiempo' ────────
//    Solo de colas con desercion==true.
//    Devuelve vector de hasta 4 valores, ordenado ascendente.
static std::vector<unsigned int> proxDeserciones(unsigned int tiempo) {
    std::vector<unsigned int> salidas;
    for (auto cola : direccionDeColas) {
        if (!cola->desercion) continue;
        for (auto cliente : cola->clientes)
            if (cliente->t_salidaCola > tiempo)
                salidas.push_back(cliente->t_salidaCola);
    }
    std::sort(salidas.begin(), salidas.end());
    if (salidas.size() > 4) salidas.resize(4);
    return salidas;
}

// ── Cabecera ─────────────────────────────────────────────────────
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

    // 4 columnas de próximas deserciones (solo si hay al menos 1 cola con desercion)
    if (hayDesercion()) {
        h += ",ProxDeser_1,ProxDeser_2,ProxDeser_3,ProxDeser_4";
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
        h += ",Ocupacion_" + n;
        if (ps->descanso) {
            h += ",Actividad_" + n;
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

    // 4 columnas de próximas deserciones
    if (hayDesercion()) {
        std::vector<unsigned int> pd = proxDeserciones(tiempo);
        for (int i = 0; i < 4; i++) {
            if (i < (int)pd.size())
                r += "," + sqcsv_fmtTime(pd[i]);
            else
                r += ",";           // blanco si hay menos de 4
        }
    }

    for (size_t i = 0; i < direccionZS.size(); i++) {
        ZonaSeguridad* zs = direccionZS[i];
        r += std::string(",") + (zs->ocupado ? "OCUP" : "LIBRE");
        r += "," + (zs->ocupado ? sqcsv_fmtTime(zs->t_salida) : std::string("-"));
    }

    for (size_t i = 0; i < direccionPS.size(); i++) {
        PuestoServicio* ps = direccionPS[i];

        r += "," + (ps->ocupado ? sqcsv_fmtTime(ps->t_FinDeServicio) : std::string("-"));
        r += std::string(",") + (ps->ocupado ? "OCUPADO" : "LIBRE");

        if (ps->descanso) {
            r += std::string(",") + (ps->Activo ? "ACTIVO" : "DESCANSANDO");
            r += std::string(",") + ( ps->Activo ? sqcsv_fmtTime(ps->t_InicioDescanso) : "");
            r += std::string(",") + (!ps->Activo ? sqcsv_fmtTime(ps->t_FinDescanso)    : "");
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
