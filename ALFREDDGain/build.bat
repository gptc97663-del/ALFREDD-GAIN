@echo off
REM Requisitos: Visual Studio 2022 (carga de trabajo "Desarrollo para el escritorio con C++"), CMake 3.22+ y Git.
REM Ejecutar desde "x64 Native Tools Command Prompt for VS 2022" o una consola normal.
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 || goto :error
cmake --build build --config Release --parallel || goto :error
echo.
echo Plugin compilado en:
echo   build\ALFREDDGain_artefacts\Release\VST3\ALFREDD Gain.vst3
exit /b 0
:error
echo ERROR: la compilacion fallo.
exit /b 1
