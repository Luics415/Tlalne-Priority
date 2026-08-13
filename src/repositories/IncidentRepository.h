#pragma once
#include "models/Incident.h"
#include <filesystem>
#include <vector>

namespace tp {
class IncidentRepository {
public:
    explicit IncidentRepository(std::filesystem::path dataDirectory);
    std::vector<Incident> load() const;
    void save(const std::vector<Incident>& incidents) const;
    const std::filesystem::path& dataDirectory() const { return dataDirectory_; }

private:
    std::filesystem::path dataDirectory_;
    std::filesystem::path incidentsPath_;
    std::filesystem::path historyPath_;
};
}

