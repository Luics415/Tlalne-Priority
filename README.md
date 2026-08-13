# Tlalne Priority

Aplicacion academica de consola para priorizar incidencias urbanas. Esta basada en un escenario educativo relacionado con Tlalnepantla y **no es un sistema oficial ni representa al Ayuntamiento**.

En el paquete para Windows, ejecuta `EJECUTAR-TLALNE-PRIORITY.bat`. Los CSV se guardan dentro de `Aplicacion/Data/`.

## Compilar en Windows

Requiere CMake 3.20 o posterior y un compilador con C++20 (Visual Studio 2022 o MinGW reciente).

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Ejecuta `TlalnePriority.exe` desde la carpeta generada por CMake. La aplicacion crea y recupera `Data/incidencias.csv` y `Data/historial.csv` en su carpeta de trabajo.

## Formula de prioridad

`riesgo * 10 + min(20, reportes * 2) + min(15, dias de antiguedad) + peso del tipo`, limitado a 0-100. Los pesos son: semaforo 18, fuga de agua 17, alumbrado 12, bache 11, basura 8, senalizacion 7 y otro 5.

Clasificacion: 0-30 BAJA, 31-55 MEDIA, 56-75 ALTA y 76-100 CRITICA. La prioridad se recalcula al consultar, filtrar o editar.
