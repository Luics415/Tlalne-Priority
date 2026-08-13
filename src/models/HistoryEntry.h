#pragma once
#include <chrono>
#include <string>

namespace tp {
struct HistoryEntry {
    std::string folio;
    std::chrono::system_clock::time_point timestamp;
    std::string event;
    std::string note;
};
}

