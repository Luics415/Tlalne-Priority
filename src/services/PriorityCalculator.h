#pragma once
#include "models/Incident.h"
#include <chrono>
#include <string>

namespace tp {
class PriorityCalculator {
public:
    static int calculate(const Incident& incident,
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now());
    static std::string classify(int score);
    static int typeWeight(IncidentType type);
};
}

