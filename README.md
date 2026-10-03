# Tlalne-Priority

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue?style=for-the-badge&logo=c%2B%2B" alt="C++20" />
  <img src="https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge&logo=cmake" alt="CMake" />
  <img src="https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows" alt="Platform" />
  <img src="https://img.shields.io/badge/License-MIT-green?style=for-the-badge" alt="License" />
</p>

Aplicación de consola en **C++20** diseñada para el registro, indexación, persistencia y **priorización algorítmica** de incidencias de servicios urbanos. 

> **Aviso:** Proyecto académico enfocado en estructuras de datos avanzadas y diseño de software en C++. Basado en un escenario educativo contextualizado en Tlalnepantla; no representa una dependencia oficial del Ayuntamiento.

---

## 🏛️ Arquitectura del Sistema

El sistema implementa separación de responsabilidades para desacoplar la interfaz de consola, la persistencia en almacenamiento secundario y la estructura de datos en memoria:

```text
Tlalne-Priority/
├── CMakeLists.txt              # Configuración de compilación CMake (C++20)
├── COMPILAR-Y-PROBAR.bat       # Script automatizado de compilación y pruebas
├── EJECUTAR-TLALNE-PRIORITY.bat# Lanzador directo de la aplicación
├── include/ o src/             # Implementación del núcleo
│   ├── Incidencia.hpp          # Modelo de dominio e invariantes de datos
│   ├── PriorityEngine.hpp      # Cola de prioridad basada en Max-Heap
│   ├── IndexManager.hpp        # Indexación por folio para consultas directas
│   └── CsvStorage.hpp          # Motor de serialización y persistencia en CSV
├── tests/                      # Suite de pruebas unitarias
└── Data/                       # Almacenamiento persistente
    ├── incidencias.csv         # Registro activo de incidencias
    └── historial.csv           # Bitácora de transiciones y resoluciones
```

---

## 🧮 Algoritmo de Priorización y Complejidad

Cada reporte urbano es evaluado mediante un modelo determinista ponderado:

$$\text{Prioridad} = \min\Big(100, \; (\text{riesgo} \times 10) + \min(20, \text{reportes} \times 2) + \min(15, \text{antigüedad}) + \text{peso}(\text{tipo})\Big)$$

### Pesos por Categoría de Servicio:
* **Semáforo:** $+18$
* **Fuga de agua:** $+17$
* **Alumbrado público:** $+12$
* **Bache:** $+11$
* **Basura:** $+8$
* **Señalización:** $+7$
* **Otros:** $+5$

### Clasificación Operativa:
* `0 - 30`: **BAJA**
* `31 - 55`: **MEDIA**
* `56 - 75`: **ALTA**
* `76 - 100`: **CRÍTICA** (Atención prioritaria inmediata)

### Complejidad Asintótica:
* **Inserción de reporte:** $\mathcal{O}(\log n)$ (inserción en Max-Heap binario).
* **Extracción de la mayor urgencia:** $\mathcal{O}(1)$ inspección de raíz, $\mathcal{O}(\log n)$ rebalanceo (*heapify*).
* **Búsqueda por folio:** $\mathcal{O}(1)$ promedio utilizando tabla hash de índices.
* **Persistencia CSV:** $\mathcal{O}(n)$ serialización secuencial con *buffering*.

---

## 🚀 Compilación y Ejecución

### Requisitos:
* Compilador compatible con **C++20** (MSVC en Visual Studio 2022, GCC 11+ o Clang 13+).
* **CMake 3.20** o superior.

### Compilar desde consola (PowerShell / CMD):
```powershell
# 1. Configurar directorio de compilación
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 2. Compilar binarios optimizados
cmake --build build --config Release

# 3. Ejecutar suite de pruebas unitarias
ctest --test-dir build -C Release --output-on-failure
```

### Ejecución directa:
Ejecuta el archivo generado `TlalnePriority.exe` o utiliza el script:
```cmd
EJECUTAR-TLALNE-PRIORITY.bat
```

---

## 📸 Evidencia Visual & Capturas

<p align="center">
  <img width="551" height="354" alt="Captura de pantalla de Tlalne-Priority" src="https://github.com/user-attachments/assets/6dd5bd5f-9ddc-43c1-b316-471bf306d03b" />
</p>

---

## 📄 Licencia

Este proyecto está bajo la Licencia [MIT](LICENSE). Libre para propósitos educativos, análisis algorítmico y extensión modular.

