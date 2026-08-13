#pragma once
#include <optional>
#include <string>

namespace tp {
enum class IncidentType { Pothole, StreetLight, WaterLeak, TrafficLight, Garbage, Signage, Other };

inline std::string toString(IncidentType type) {
    switch (type) {
        case IncidentType::Pothole: return "BACHE";
        case IncidentType::StreetLight: return "ALUMBRADO PUBLICO";
        case IncidentType::WaterLeak: return "FUGA DE AGUA";
        case IncidentType::TrafficLight: return "SEMAFORO";
        case IncidentType::Garbage: return "BASURA";
        case IncidentType::Signage: return "SENALIZACION";
        default: return "OTRO";
    }
}

inline std::optional<IncidentType> incidentTypeFromString(const std::string& value) {
    if (value == "BACHE") return IncidentType::Pothole;
    if (value == "ALUMBRADO PUBLICO") return IncidentType::StreetLight;
    if (value == "FUGA DE AGUA") return IncidentType::WaterLeak;
    if (value == "SEMAFORO") return IncidentType::TrafficLight;
    if (value == "BASURA") return IncidentType::Garbage;
    if (value == "SENALIZACION") return IncidentType::Signage;
    if (value == "OTRO") return IncidentType::Other;
    return std::nullopt;
}
}

