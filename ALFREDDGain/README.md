# ALFREDD Gain

Plugin VST3 de ganancia (-24 dB a +24 dB) con bypass y medidores de entrada/salida.
C++17 + JUCE 8.0.6 + CMake. Windows x64. Mono y estéreo.

> **Estado:** el código fuente NO ha sido compilado ni probado por quien lo escribió
> (el entorno donde se generó no tenía compilador de Windows, CMake ni acceso a Internet).
> Los pasos de abajo son la primera compilación real; si algo falla, el mensaje de error es el dato útil.

## Compilar (opción A: en tu PC)
Requisitos: Visual Studio 2022 (carga "Desarrollo para el escritorio con C++"), CMake ≥ 3.22, Git.

    build.bat

El primer `cmake` descarga JUCE (necesita Internet). Resultado:

    build\ALFREDDGain_artefacts\Release\VST3\ALFREDD Gain.vst3

## Compilar (opción B: GitHub Actions)
Sube la carpeta a un repositorio de GitHub. El workflow `.github/workflows/build-windows.yml`
compila en `windows-latest` y publica el `.vst3` como artefacto descargable.

## Instalar
Copia la carpeta `ALFREDD Gain.vst3` completa (es un bundle, no un solo archivo) a:

    C:\Program Files\Common Files\VST3\

- **Ableton Live:** Preferencias → Plug-ins → activar "Use VST3 Plug-in System Folders" → Rescan.
- **FL Studio:** Options → Manage plugins → Find installed plugins (Verify plugins).

## Detalles
- Parámetros: `gain` (dB, automatizable, defecto 0 dB) y `bypass` (automatizable).
- Estado guardado con `AudioProcessorValueTreeState` (XML en el proyecto del DAW).
- Cambios de ganancia/bypass suavizados en 20 ms (sin clics).
- Runtime de MSVC enlazado estáticamente (no requiere VC++ Redistributable).

## Instalador (.exe)
`installer.iss` (Inno Setup 6) genera `ALFREDD-Gain-Setup.exe`, que copia el plugin a
`C:\Program Files\Common Files\VST3\`. El workflow de GitHub Actions lo crea automáticamente
y lo publica como artefacto "ALFREDD-Gain-Installer". En local: compila primero y luego `iscc installer.iss`.
