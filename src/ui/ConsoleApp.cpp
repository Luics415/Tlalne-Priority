#include "ui/ConsoleApp.h"
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

namespace tp {
namespace {
std::string formatTime(std::chrono::system_clock::time_point value) {
    const auto raw = std::chrono::system_clock::to_time_t(value); std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &raw);
#else
    localtime_r(&raw, &tm);
#endif
    std::ostringstream out; out << std::put_time(&tm, "%d/%m/%Y %H:%M"); return out.str();
}
}

void ConsoleApp::run() {
#ifdef _WIN32
    std::system("chcp 65001 > nul");
#endif
    bool running = true;
    while (running) {
        std::cout << "\n==============================================\n"
                  << "              TLALNE PRIORITY\n"
                  << "  PROYECTO ACADEMICO - NO ES SISTEMA OFICIAL\n"
                  << "==============================================\n"
                  << "1. Registrar incidencia\n2. Ver incidencias prioritarias activas\n"
                  << "3. Buscar incidencia por folio\n4. Editar / corregir incidencia\n"
                  << "5. Cambiar estado\n6. Ver historial especifico por folio\n"
                  << "7. Ver incidencias por zona\n8. Ver incidencias finalizadas\n0. Salir\n";
        try {
            const int option = readInt("Selecciona una opcion: ", 0, 8);
            switch (option) {
                case 1: registerIncident(); break; case 2: listActive(); break; case 3: search(); break;
                case 4: edit(); break; case 5: changeStatus(); break; case 6: history(); break;
                case 7: byZone(); break; case 8: listFinished(); break; default: running = false;
            }
            if (running) pause();
        } catch (const std::exception& error) {
            if (std::cin.eof()) {
                std::cout << "\nENTRADA FINALIZADA. CERRANDO LA APLICACION.\n";
                running = false;
            } else {
                std::cout << "\nERROR: " << error.what() << '\n';
                pause();
            }
        }
    }
    std::cout << "DATOS GUARDADOS. HASTA PRONTO.\n";
}

std::string ConsoleApp::readRequired(const std::string& prompt) {
    std::cout << prompt; std::string value; if (!std::getline(std::cin, value)) throw std::runtime_error("ENTRADA FINALIZADA.");
    value = IncidentService::normalize(value); if (value.empty()) throw std::invalid_argument("EL DATO ES OBLIGATORIO."); return value;
}
int ConsoleApp::readInt(const std::string& prompt, int min, int max) {
    const auto text = readRequired(prompt); std::size_t used = 0; int value{};
    try { value = std::stoi(text, &used); } catch (...) { throw std::invalid_argument("INGRESA UN NUMERO VALIDO."); }
    if (used != text.size() || value < min || value > max) throw std::invalid_argument("VALOR FUERA DEL RANGO PERMITIDO.");
    return value;
}
IncidentType ConsoleApp::readType() {
    std::cout << "1. BACHE\n2. ALUMBRADO PUBLICO\n3. FUGA DE AGUA\n4. SEMAFORO\n5. BASURA\n6. SENALIZACION\n7. OTRO\n";
    return static_cast<IncidentType>(readInt("Tipo: ", 1, 7) - 1);
}
void ConsoleApp::pause() { std::cout << "\nPresiona ENTER para continuar..."; std::string value; std::getline(std::cin, value); }

void ConsoleApp::registerIncident() {
    std::cout << "\n=== REGISTRAR INCIDENCIA ===\n"; IncidentDraft draft{readType(), readRequired("Zona: "), readRequired("Descripcion: "),
        readInt("Nivel de riesgo (1-5): ", 1, 5), readInt("Reportes ciudadanos (0-1000000): ", 0, 1000000)};
    auto item = service_.create(draft); std::cout << "\nINCIDENCIA REGISTRADA CON FOLIO " << item.folio << "\n"; printIncident(item);
}
void ConsoleApp::listActive() { std::cout << "\n=== COLA PRIORITARIA ACTIVA ===\n"; printList(service_.activeByPriority()); }
void ConsoleApp::search() { const auto item = service_.findByFolio(readRequired("Folio: ")); if (!item) std::cout << "NO SE ENCONTRO EL FOLIO.\n"; else printIncident(*item); }
void ConsoleApp::edit() {
    const auto folio = readRequired("Folio a corregir: "); auto current = service_.findByFolio(folio);
    if (!current) { std::cout << "NO SE ENCONTRO EL FOLIO.\n"; return; }
    printIncident(*current); std::cout << "\nCAPTURA LOS DATOS CORREGIDOS COMPLETOS:\n";
    IncidentDraft draft{readType(), readRequired("Zona: "), readRequired("Descripcion: "), readInt("Riesgo (1-5): ", 1, 5),
        readInt("Reportes ciudadanos: ", 0, 1000000)};
    printIncident(service_.edit(folio, draft, readRequired("Motivo de correccion: ")));
}
void ConsoleApp::changeStatus() {
    const auto folio = readRequired("Folio: "); auto current = service_.findByFolio(folio);
    if (!current) { std::cout << "NO SE ENCONTRO EL FOLIO.\n"; return; }
    auto next = nextStatus(current->status); if (!next) { std::cout << "LA INCIDENCIA YA ESTA FINALIZADA.\n"; return; }
    std::cout << "Estado actual: " << toString(current->status) << "\nSiguiente estado permitido: " << toString(*next) << '\n';
    const auto confirm = readRequired("Confirmar cambio (S/N): "); if (confirm != "S") { std::cout << "CAMBIO CANCELADO.\n"; return; }
    printIncident(service_.changeStatus(folio, *next, readRequired("Nota del cambio: ")));
}
void ConsoleApp::history() {
    auto item = service_.findByFolio(readRequired("Folio: ")); if (!item) { std::cout << "NO SE ENCONTRO EL FOLIO.\n"; return; }
    std::cout << "\n=== HISTORIAL " << item->folio << " ===\n";
    for (const auto& h : item->history) std::cout << formatTime(h.timestamp) << " | " << h.event << " | " << h.note << '\n';
}
void ConsoleApp::byZone() { std::cout << "\n=== ACTIVAS POR ZONA ===\n"; printList(service_.activeByZone(readRequired("Zona: "))); }
void ConsoleApp::listFinished() { std::cout << "\n=== INCIDENCIAS FINALIZADAS ===\n"; printList(service_.finished()); }

void ConsoleApp::printIncident(const Incident& i) {
    std::cout << "----------------------------------------------\n" << i.folio << " | " << toString(i.type) << " | " << i.zone
              << "\n" << i.description << "\nRIESGO: " << i.risk << " | REPORTES: " << i.citizenReports
              << " | CREADA: " << formatTime(i.createdAt) << "\nPRIORIDAD: " << i.priority << "/100 [" << i.priorityClass
              << "] | ESTADO: " << toString(i.status) << '\n';
}
void ConsoleApp::printList(const std::vector<Incident>& items) {
    if (items.empty()) { std::cout << "NO HAY INCIDENCIAS PARA MOSTRAR.\n"; return; }
    constexpr std::size_t pageSize = 20; const auto count = std::min(items.size(), pageSize);
    for (std::size_t i = 0; i < count; ++i) printIncident(items[i]);
    if (items.size() > pageSize) std::cout << "MOSTRANDO 20 DE " << items.size() << ". USA BUSQUEDA O FILTRO POR ZONA.\n";
}
}
