#include "services/IncidentService.h"
#include "services/PriorityCalculator.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <queue>
#include <sstream>
#include <stdexcept>

namespace tp {
namespace {
std::string datePart(std::chrono::system_clock::time_point value) {
    const auto raw = std::chrono::system_clock::to_time_t(value); std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &raw);
#else
    localtime_r(&raw, &tm);
#endif
    std::ostringstream out; out << std::put_time(&tm, "%Y%m%d"); return out.str();
}
}

IncidentService::IncidentService(IncidentRepository repository)
    : repository_(std::move(repository)), incidents_(repository_.load()) { rebuildIndex(); recalculateAll(); }

std::string IncidentService::normalize(std::string value) {
    auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    auto last = value.find_last_not_of(" \t\r\n"); value = value.substr(first, last - first + 1);
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

void IncidentService::validate(const IncidentDraft& draft) {
    if (normalize(draft.zone).empty()) throw std::invalid_argument("LA ZONA ES OBLIGATORIA.");
    if (normalize(draft.description).empty()) throw std::invalid_argument("LA DESCRIPCION ES OBLIGATORIA.");
    if (draft.risk < 1 || draft.risk > 5) throw std::invalid_argument("EL RIESGO DEBE ESTAR ENTRE 1 Y 5.");
    if (draft.citizenReports < 0) throw std::invalid_argument("LOS REPORTES NO PUEDEN SER NEGATIVOS.");
}

void IncidentService::rebuildIndex() { index_.clear(); for (std::size_t i = 0; i < incidents_.size(); ++i) index_[incidents_[i].folio] = i; }
void IncidentService::recalculate(Incident& i) const { i.priority = PriorityCalculator::calculate(i); i.priorityClass = PriorityCalculator::classify(i.priority); }
void IncidentService::recalculateAll() { for (auto& i : incidents_) recalculate(i); }
void IncidentService::persist() { repository_.save(incidents_); }

std::string IncidentService::generateFolio(std::chrono::system_clock::time_point now) const {
    const auto day = datePart(now); int sequence = 0;
    const std::string prefix = "TP-" + day + "-";
    for (const auto& i : incidents_) if (i.folio.rfind(prefix, 0) == 0) {
        try { sequence = std::max(sequence, std::stoi(i.folio.substr(prefix.size()))); } catch (...) {}
    }
    std::ostringstream out; out << prefix << std::setw(3) << std::setfill('0') << sequence + 1; return out.str();
}

Incident& IncidentService::require(const std::string& folio) {
    const auto key = normalize(folio); auto it = index_.find(key);
    if (it == index_.end()) throw std::out_of_range("NO EXISTE UNA INCIDENCIA CON ESE FOLIO.");
    return incidents_[it->second];
}

Incident IncidentService::create(const IncidentDraft& draft, std::chrono::system_clock::time_point now) {
    validate(draft);
    Incident i; i.folio = generateFolio(now); i.type = draft.type; i.zone = normalize(draft.zone);
    i.description = normalize(draft.description); i.risk = draft.risk; i.citizenReports = draft.citizenReports;
    i.createdAt = now; i.status = IncidentStatus::Reported;
    i.history.push_back({i.folio, now, "INCIDENCIA REGISTRADA", "REGISTRO INICIAL"}); recalculate(i);
    incidents_.push_back(i); rebuildIndex(); persist(); return i;
}

std::optional<Incident> IncidentService::findByFolio(const std::string& folio) {
    recalculateAll(); auto it = index_.find(normalize(folio));
    if (it == index_.end()) return std::nullopt;
    return incidents_[it->second];
}

std::vector<Incident> IncidentService::activeByPriority() {
    recalculateAll();
    auto less = [](const Incident* a, const Incident* b) { if (a->priority != b->priority) return a->priority < b->priority; return a->createdAt > b->createdAt; };
    std::priority_queue<const Incident*, std::vector<const Incident*>, decltype(less)> queue(less);
    for (const auto& i : incidents_) if (i.status != IncidentStatus::Finished) queue.push(&i);
    std::vector<Incident> result; while (!queue.empty()) { result.push_back(*queue.top()); queue.pop(); } return result;
}

std::vector<Incident> IncidentService::activeByZone(const std::string& zone) {
    recalculateAll(); std::vector<Incident> result; const auto key = normalize(zone);
    for (const auto& i : incidents_) if (i.status != IncidentStatus::Finished && i.zone == key) result.push_back(i);
    std::sort(result.begin(), result.end(), [](const Incident& a, const Incident& b) { return a.priority > b.priority; }); return result;
}

std::vector<Incident> IncidentService::finished() {
    recalculateAll(); std::vector<Incident> result;
    std::copy_if(incidents_.begin(), incidents_.end(), std::back_inserter(result), [](const Incident& i) { return i.status == IncidentStatus::Finished; });
    std::sort(result.begin(), result.end(), [](const Incident& a, const Incident& b) { return a.createdAt > b.createdAt; }); return result;
}

Incident IncidentService::edit(const std::string& folio, const IncidentDraft& values, const std::string& note) {
    validate(values); auto& i = require(folio); i.type = values.type; i.zone = normalize(values.zone);
    i.description = normalize(values.description); i.risk = values.risk; i.citizenReports = values.citizenReports;
    recalculate(i); i.history.push_back({i.folio, std::chrono::system_clock::now(), "CORRECCION DE INCIDENCIA", normalize(note)});
    persist(); return i;
}

Incident IncidentService::changeStatus(const std::string& folio, IncidentStatus newStatus, const std::string& note) {
    auto& i = require(folio); const auto expected = nextStatus(i.status);
    if (!expected || *expected != newStatus) throw std::invalid_argument("TRANSICION INVALIDA. SOLO SE PERMITE EL SIGUIENTE ESTADO DEL FLUJO.");
    i.status = newStatus; i.history.push_back({i.folio, std::chrono::system_clock::now(), "CAMBIO DE ESTADO A " + toString(newStatus), normalize(note)});
    persist(); return i;
}
}
