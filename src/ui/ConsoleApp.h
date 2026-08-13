#pragma once
#include "services/IncidentService.h"

namespace tp {
class ConsoleApp {
public:
    explicit ConsoleApp(IncidentService& service) : service_(service) {}
    void run();
private:
    IncidentService& service_;
    void registerIncident(); void listActive(); void search(); void edit(); void changeStatus();
    void history(); void byZone(); void listFinished();
    static void printIncident(const Incident& item); static void printList(const std::vector<Incident>& items);
    static std::string readRequired(const std::string& prompt); static int readInt(const std::string& prompt, int min, int max);
    static IncidentType readType(); static void pause();
};
}

