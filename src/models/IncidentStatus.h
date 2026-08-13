#pragma once
#include <optional>
#include <string>

namespace tp {
enum class IncidentStatus { Reported, Assigned, InProgress, Finished };

inline std::string toString(IncidentStatus status) {
    switch (status) {
        case IncidentStatus::Reported: return "REPORTADO";
        case IncidentStatus::Assigned: return "ASIGNADO";
        case IncidentStatus::InProgress: return "EN ATENCION";
        default: return "FINALIZADO";
    }
}

inline std::optional<IncidentStatus> incidentStatusFromString(const std::string& value) {
    if (value == "REPORTADO") return IncidentStatus::Reported;
    if (value == "ASIGNADO") return IncidentStatus::Assigned;
    if (value == "EN ATENCION") return IncidentStatus::InProgress;
    if (value == "FINALIZADO") return IncidentStatus::Finished;
    return std::nullopt;
}

inline std::optional<IncidentStatus> nextStatus(IncidentStatus status) {
    switch (status) {
        case IncidentStatus::Reported: return IncidentStatus::Assigned;
        case IncidentStatus::Assigned: return IncidentStatus::InProgress;
        case IncidentStatus::InProgress: return IncidentStatus::Finished;
        default: return std::nullopt;
    }
}
}

