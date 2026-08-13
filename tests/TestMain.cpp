#include "repositories/IncidentRepository.h"
#include "services/IncidentService.h"
#include "services/PriorityCalculator.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main() {
    const auto testDir = fs::temp_directory_path() / "tlalne_priority_tests";
    std::error_code ec; fs::remove_all(testDir, ec); fs::create_directories(testDir);
    try {
        tp::IncidentService service{tp::IncidentRepository(testDir)};
        const auto low = service.create({tp::IncidentType::Garbage, "zona centro", "bolsas acumuladas", 1, 1});
        assert(low.folio.rfind("TP-", 0) == 0);
        assert(low.zone == "ZONA CENTRO" && low.description == "BOLSAS ACUMULADAS");
        assert(low.priority >= 0 && low.priority <= 100);
        bool rejectedRisk = false;
        try { service.create({tp::IncidentType::Pothole, "X", "Y", 6, 0}); } catch (const std::invalid_argument&) { rejectedRisk = true; }
        assert(rejectedRisk);

        const auto high = service.create({tp::IncidentType::TrafficLight, "CENTRO", "NO FUNCIONA", 5, 10});
        const auto queue = service.activeByPriority();
        assert(queue.size() == 2 && queue.front().folio == high.folio);
        assert(service.findByFolio(low.folio).has_value());

        const auto edited = service.edit(low.folio, {tp::IncidentType::WaterLeak, "san juan", "fuga grande", 5, 20}, "dato corregido");
        assert(edited.zone == "SAN JUAN" && edited.priority > low.priority && edited.history.size() == 2);

        bool invalidTransition = false;
        try { service.changeStatus(high.folio, tp::IncidentStatus::Finished, "SALTO"); } catch (const std::invalid_argument&) { invalidTransition = true; }
        assert(invalidTransition);
        service.changeStatus(high.folio, tp::IncidentStatus::Assigned, "CUADRILLA ASIGNADA");
        service.changeStatus(high.folio, tp::IncidentStatus::InProgress, "TRABAJO INICIADO");
        service.changeStatus(high.folio, tp::IncidentStatus::Finished, "RESUELTO");
        const auto active = service.activeByPriority();
        assert(std::none_of(active.begin(), active.end(), [&](const tp::Incident& i) { return i.folio == high.folio; }));
        assert(service.finished().size() == 1);

        tp::IncidentService reloaded{tp::IncidentRepository(testDir)};
        const auto restored = reloaded.findByFolio(high.folio);
        assert(restored && restored->status == tp::IncidentStatus::Finished && restored->history.size() == 4);
        assert(fs::exists(testDir / "incidencias.csv") && fs::exists(testDir / "historial.csv"));

        fs::remove_all(testDir, ec);
        std::cout << "TODAS LAS PRUEBAS PASARON.\n"; return 0;
    } catch (const std::exception& error) {
        std::cerr << "PRUEBA FALLIDA: " << error.what() << '\n'; return 1;
    }
}
