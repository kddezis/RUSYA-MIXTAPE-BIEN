# RUSYA MIXTAPE MAMAGUEVO  (VST3)

Saturador de un solo knob (tanh asimétrico + oversampling 2x + DC blocker + compensación de volumen).
Estética rusa/soviética: Moscú de fondo, estrella roja, dorado, botella Glacial inclinándose con el knob.

## Compilar (necesitas CMake ≥ 3.22, git, y un compilador C++17)

### Windows (Visual Studio 2022)
    cmake -B build -G "Visual Studio 17 2022"
    cmake --build build --config Release
El VST3 se copia solo a `C:\Program Files\Common Files\VST3\`  (abre la terminal como administrador, o copia a mano desde `build\RusyaMixtape_artefacts\Release\VST3`).

### macOS
    cmake -B build -G Xcode
    cmake --build build --config Release
Se copia a `~/Library/Audio/Plug-Ins/VST3/`.

### Linux
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j
(Necesita las libs de JUCE: libasound2-dev, libfreetype-dev, libx11-dev, libxrandr-dev, libxinerama-dev, libxcursor-dev, libgl1-mesa-dev.)

La primera vez descarga JUCE 8 automáticamente (hace falta internet).

## Ajustes rápidos
- Cantidad de saturación: `Source/PluginProcessor.cpp` → `d * 40.0f` (dB máximos de pre-gain).
- Sesgo (armónicos pares): `0.12f * d`.
- Imágenes: carpeta `Assets/` (se incrustan en el binario via BinaryData).

## Sin instalar nada: compilar en la nube (GitHub Actions)
1. Crea un repo en github.com (privado está bien) y sube todo el contenido de esta carpeta (incluida `.github`).
2. Ve a la pestaña **Actions** → "Build VST3 (Windows)" (corre solo al subir; también con "Run workflow").
3. Cuando termine (~10 min), entra al run y descarga el artifact **RUSYA-MIXTAPE-MAMAGUEVO-VST3**.
4. Descomprime y copia la carpeta `RUSYA MIXTAPE MAMAGUEVO.vst3` a `C:\Program Files\Common Files\VST3\`.
5. En FL Studio: Options → Manage plugins → **Find installed plugins** (con "Verify plugins" activado).
