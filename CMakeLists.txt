cmake_minimum_required(VERSION 3.20)
project(TlalnePriority VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TP_SOURCES
    src/models/Incident.cpp
    src/models/HistoryEntry.cpp
    src/services/PriorityCalculator.cpp
    src/services/IncidentService.cpp
    src/repositories/IncidentRepository.cpp
)

add_library(tlalne_core ${TP_SOURCES})
target_include_directories(tlalne_core PUBLIC src)
if(MSVC)
  target_compile_options(tlalne_core PRIVATE /W4 /permissive- /utf-8)
else()
  target_compile_options(tlalne_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(TlalnePriority src/main.cpp src/ui/ConsoleApp.cpp)
target_link_libraries(TlalnePriority PRIVATE tlalne_core)

include(CTest)
if(BUILD_TESTING)
  add_executable(TlalnePriorityTests tests/TestMain.cpp)
  target_link_libraries(TlalnePriorityTests PRIVATE tlalne_core)
  add_test(NAME TlalnePriorityTests COMMAND TlalnePriorityTests)
endif()

