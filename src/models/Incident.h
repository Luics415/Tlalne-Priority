#pragma once
#include "models/HistoryEntry.h"
#include "models/IncidentStatus.h"
#include "models/IncidentType.h"
#include <chrono>
#include <string>
#include <vector>

namespace tp {
struct Incident {
    std::string folio;
    IncidentType type{IncidentType::Other};
    std::string zone;
    std::string description;
    int risk{1};
    int citizenReports{0};
    std::chrono::system_clock::time_point createdAt;
    int priority{0};
    std::string priorityClass{"BAJA"};
    IncidentStatus status{IncidentStatus::Reported};
    std::vector<HistoryEntry> history;
};
}

