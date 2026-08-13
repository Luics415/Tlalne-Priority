#include "services/PriorityCalculator.h"
#include <algorithm>

namespace tp {
int PriorityCalculator::typeWeight(IncidentType type) {
    switch (type) {
        case IncidentType::TrafficLight: return 18;
        case IncidentType::WaterLeak: return 17;
        case IncidentType::StreetLight: return 12;
        case IncidentType::Pothole: return 11;
        case IncidentType::Garbage: return 8;
        case IncidentType::Signage: return 7;
        default: return 5;
    }
}

int PriorityCalculator::calculate(const Incident& incident, std::chrono::system_clock::time_point now) {
    const auto ageHours = std::max<long long>(0,
        std::chrono::duration_cast<std::chrono::hours>(now - incident.createdAt).count());
    const int ageDays = static_cast<int>(ageHours / 24);
    // Riesgo: hasta 50; reportes: hasta 20; antiguedad: hasta 15; tipo: hasta 18.
    const int score = incident.risk * 10
        + std::min(20, incident.citizenReports * 2)
        + std::min(15, ageDays)
        + typeWeight(incident.type);
    return std::clamp(score, 0, 100);
}

std::string PriorityCalculator::classify(int score) {
    if (score <= 30) return "BAJA";
    if (score <= 55) return "MEDIA";
    if (score <= 75) return "ALTA";
    return "CRITICA";
}
}

