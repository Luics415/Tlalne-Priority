#include "repositories/IncidentRepository.h"
#include "models/IncidentStatus.h"
#include "models/IncidentType.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace tp {
namespace {
std::string escapeCsv(const std::string& value) {
    std::string result = value;
    std::size_t pos = 0;
    while ((pos = result.find('"', pos)) != std::string::npos) { result.insert(pos, 1, '"'); pos += 2; }
    return '"' + result + '"';
}

std::vector<std::string> parseCsv(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (ch == '"') {
            if (quoted && i + 1 < line.size() && line[i + 1] == '"') { field += '"'; ++i; }
            else quoted = !quoted;
        } else if (ch == ',' && !quoted) { fields.push_back(field); field.clear(); }
        else field += ch;
    }
    fields.push_back(field);
    return fields;
}

std::string timeToString(std::chrono::system_clock::time_point value) {
    const auto raw = std::chrono::system_clock::to_time_t(value);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &raw);
#else
    localtime_r(&raw, &tm);
#endif
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

std::chrono::system_clock::time_point stringToTime(const std::string& value) {
    std::tm tm{};
    std::istringstream input(value);
    input >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (input.fail()) throw std::runtime_error("FECHA INVALIDA EN CSV: " + value);
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}
}

IncidentRepository::IncidentRepository(std::filesystem::path dataDirectory)
    : dataDirectory_(std::move(dataDirectory)),
      incidentsPath_(dataDirectory_ / "incidencias.csv"),
      historyPath_(dataDirectory_ / "historial.csv") {}

std::vector<Incident> IncidentRepository::load() const {
    std::vector<Incident> incidents;
    std::unordered_map<std::string, std::size_t> index;
    if (std::ifstream input{incidentsPath_}; input) {
        std::string line; std::getline(input, line);
        while (std::getline(input, line)) {
            if (line.empty()) continue;
            const auto f = parseCsv(line);
            if (f.size() != 10) throw std::runtime_error("REGISTRO DE INCIDENCIA CSV DANADO.");
            auto type = incidentTypeFromString(f[1]); auto status = incidentStatusFromString(f[8]);
            if (!type || !status) throw std::runtime_error("TIPO O ESTADO INVALIDO EN CSV.");
            Incident item{f[0], *type, f[2], f[3], std::stoi(f[4]), std::stoi(f[5]),
                stringToTime(f[6]), std::stoi(f[7]), f[9], *status, {}};
            index[item.folio] = incidents.size(); incidents.push_back(std::move(item));
        }
    }
    if (std::ifstream input{historyPath_}; input) {
        std::string line; std::getline(input, line);
        while (std::getline(input, line)) {
            if (line.empty()) continue;
            const auto f = parseCsv(line);
            if (f.size() != 4) throw std::runtime_error("REGISTRO DE HISTORIAL CSV DANADO.");
            if (auto it = index.find(f[0]); it != index.end())
                incidents[it->second].history.push_back({f[0], stringToTime(f[1]), f[2], f[3]});
        }
    }
    return incidents;
}

void IncidentRepository::save(const std::vector<Incident>& incidents) const {
    std::filesystem::create_directories(dataDirectory_);
    const auto incidentsTmp = incidentsPath_.string() + ".tmp";
    const auto historyTmp = historyPath_.string() + ".tmp";
    {
        std::ofstream out(incidentsTmp, std::ios::trunc);
        if (!out) throw std::runtime_error("NO SE PUDO GUARDAR INCIDENCIAS.CSV.");
        out << "FOLIO,TIPO,ZONA,DESCRIPCION,RIESGO,REPORTES,CREACION,PRIORIDAD,ESTADO,CLASIFICACION\n";
        for (const auto& i : incidents)
            out << escapeCsv(i.folio) << ',' << escapeCsv(toString(i.type)) << ',' << escapeCsv(i.zone) << ','
                << escapeCsv(i.description) << ',' << i.risk << ',' << i.citizenReports << ','
                << escapeCsv(timeToString(i.createdAt)) << ',' << i.priority << ',' << escapeCsv(toString(i.status))
                << ',' << escapeCsv(i.priorityClass) << '\n';
    }
    {
        std::ofstream out(historyTmp, std::ios::trunc);
        if (!out) throw std::runtime_error("NO SE PUDO GUARDAR HISTORIAL.CSV.");
        out << "FOLIO,FECHA_HORA,EVENTO,NOTA\n";
        for (const auto& i : incidents) for (const auto& h : i.history)
            out << escapeCsv(h.folio) << ',' << escapeCsv(timeToString(h.timestamp)) << ','
                << escapeCsv(h.event) << ',' << escapeCsv(h.note) << '\n';
    }
    std::error_code ec;
    std::filesystem::remove(incidentsPath_, ec); ec.clear();
    std::filesystem::rename(incidentsTmp, incidentsPath_, ec);
    if (ec) throw std::runtime_error("NO SE PUDO REEMPLAZAR INCIDENCIAS.CSV.");
    std::filesystem::remove(historyPath_, ec); ec.clear();
    std::filesystem::rename(historyTmp, historyPath_, ec);
    if (ec) throw std::runtime_error("NO SE PUDO REEMPLAZAR HISTORIAL.CSV.");
}
}

