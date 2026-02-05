# Building The Collider Plugin

## Quick Start

### macOS
```bash
# 1. Install CMake (if needed)
brew install cmake

# 2. Clone the repository
git clone http://[repo]/physicscollidervst
cd physicscollidervst

# 3. Get JUCE
git clone https://github.com/juce-framework/JUCE.git JUCE

# 4. Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel 4

# 5. Find your plugins
open build/TheCollider_artefacts/Release/

# Output:
# - VST3: TheCollider_artefacts/Release/VST3/TheCollider.vst3
# - AU: TheCollider_artefacts/Release/AU/TheCollider.component
# - Standalone: TheCollider_artefacts/Release/Standalone/TheCollider.app
```

### Windows
```cmd
# 1. Install CMake & Visual Studio Build Tools

# 2. Clone the repository
git clone http://[repo]/physicscollidervst
cd physicscollidervst

# 3. Get JUCE
git clone https://github.com/juce-framework/JUCE.git JUCE

# 4. Build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel 4

# 5. Find your plugins
# VST3: build\TheCollider_artefacts\Release\VST3\TheCollider.vst3
```

### Linux (Standalone only)
```bash
# 1. Install dependencies
sudo apt-get install build-essential cmake git

# 2. Clone the repository
git clone http://[repo]/physicscollidervst
cd physicscollidervst

# 3. Get JUCE
git clone https://github.com/juce-framework/JUCE.git JUCE

# 4. Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel 4

# 5. Run standalone
./TheCollider_artefacts/Release/Standalone/TheCollider
```

---

## Requirements

| Platform | Requirements |
|----------|--------------|
| **macOS** | Xcode 13+, CMake 3.21+, JUCE 8 |
| **Windows** | Visual Studio 2019+, CMake 3.21+, JUCE 8 |
| **Linux** | GCC 9+, CMake 3.21+, JUCE 8 (Standalone only) |

---

## Installation

### macOS
```bash
# VST3
cp -r build/TheCollider_artefacts/Release/VST3/TheCollider.vst3 \
  ~/Library/Audio/Plug-Ins/VST3/

# AU (recommended)
cp -r build/TheCollider_artefacts/Release/AU/TheCollider.component \
  ~/Library/Audio/Plug-Ins/Components/
```

### Windows
```cmd
# VST3
xcopy build\TheCollider_artefacts\Release\VST3\TheCollider.vst3 ^
  "%APPDATA%\VST3\" /Y
```

### Linux
```bash
# VST3 (if available)
mkdir -p ~/.vst3
cp -r build/TheCollider_artefacts/Release/VST3/TheCollider.vst3 ~/.vst3/
```

---

## Troubleshooting

### "JUCE not found"
Make sure JUCE is cloned into the project root:
```bash
git clone https://github.com/juce-framework/JUCE.git JUCE
```

### CMake configuration fails
Update CMake to 3.21+:
```bash
# macOS
brew install cmake

# Ubuntu
sudo apt-get install cmake
```

### Build fails on Windows
Ensure Visual Studio Build Tools are installed with C++ support

### AU plugin not building on macOS
Requires Xcode command line tools:
```bash
xcode-select --install
```

---

## Automated Builds with GitHub Actions

The project includes GitHub Actions workflows that automatically build VST3/AU plugins for macOS and Windows. Binaries are available as artifacts on every push!

**To use:**
1. Push changes to `claude/collider-synth-plugin-kTeW9`
2. GitHub Actions builds automatically
3. Download prebuilt VST3/AU from the workflow artifacts or latest release

---

## Build Configuration

The CMakeLists.txt is configured to produce:
- **VST3**: Universal plugin format (macOS/Windows)
- **AU**: macOS Audio Unit (macOS only)
- **Standalone**: Executable audio application (all platforms)

All formats are built by default. To limit builds, modify CMakeLists.txt:
```cmake
# Keep only VST3
FORMATS VST3
```

---

## Performance Notes

Build times:
- **macOS**: ~3-5 minutes (first build with JUCE)
- **Windows**: ~5-7 minutes
- **Linux**: ~2-3 minutes (Standalone only)

Debug builds are faster but larger. Use `-DCMAKE_BUILD_TYPE=Release` for optimized, smaller binaries.

---

## Next Steps

After building:
1. Copy plugin to your DAW's plugin folder
2. Rescan plugins in your DAW
3. Open The Collider and select a factory preset
4. Play MIDI notes and adjust parameters in real-time!

For parameter reference, see `IMPLEMENTATION_COMPLETE.md`
