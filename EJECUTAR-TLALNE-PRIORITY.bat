@echo off
cd /d "%~dp0Aplicacion"
if not exist "TlalnePriority.exe" (
  echo No se encontro Aplicacion\TlalnePriority.exe.
  echo Compila el proyecto o vuelve a extraer el ZIP completo.
  pause
  exit /b 1
)
TlalnePriority.exe

