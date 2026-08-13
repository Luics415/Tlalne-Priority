#pragma once
#include "repositories/IncidentRepository.h"
#include <optional>
#include <string>
#include <unordered_map>

namespace tp {
struct IncidentDraft { IncidentType type; std::string zone; std::string description; int risk; int citizenReports; };

class IncidentService {
public:
    explicit IncidentService(IncidentRepository repository);
    Incident create(const IncidentDraft& draft, std::chrono::system_clock::time_point now = std::chrono::system_clock::now());
    std::optional<Incident> findByFolio(const std::string& folio);
    std::vector<Incident> activeByPriority();
    std::vector<Incident> activeByZone(const std::string& zone);
    std::vector<Incident> finished();
    Incident edit(const std::string& folio, const IncidentDraft& values, const std::string& note = "CORRECCION DE DATOS");
    Incident changeStatus(const std::string& folio, IncidentStatus newStatus, const std::string& note);
    const std::vector<Incident>& all() const { return incidents_; }
    static std::string normalize(std::string value);

private:
    IncidentRepository repository_;
    std::vector<Incident> incidents_;
    std::unordered_map<std::string, std::size_t> index_;
    void rebuildIndex();
    void recalculate(Incident& incident) const;
    void recalculateAll();
    void persist();
    std::string generateFolio(std::chrono::system_clock::time_point now) const;
    Incident& require(const std::string& folio);
    static void validate(const IncidentDraft& draft);
};
}

