#include "repositories/IncidentRepository.h"
#include "services/IncidentService.h"
#include "ui/ConsoleApp.h"
#include <filesystem>
#include <iostream>

int main() {
    try {
        tp::IncidentRepository repository(std::filesystem::current_path() / "Data");
        tp::IncidentService service(std::move(repository));
        tp::ConsoleApp app(service); app.run(); return 0;
    } catch (const std::exception& error) {
        std::cerr << "ERROR DE INICIO: " << error.what() << '\n'; return 1;
    }
}

