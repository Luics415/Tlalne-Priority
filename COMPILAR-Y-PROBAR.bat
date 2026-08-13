@echo off
setlocal
cd /d "%~dp0"
cmake -S . -B build
if errorlevel 1 goto error
cmake --build build --config Release
if errorlevel 1 goto error
ctest --test-dir build -C Release --output-on-failure
if errorlevel 1 goto error
echo.
echo Compilacion y pruebas completadas correctamente.
pause
exit /b 0
:error
echo.
echo No se pudo completar. Verifica que CMake y Visual Studio C++ esten instalados.
pause
exit /b 1

