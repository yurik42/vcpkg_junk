# Claude Code Configuration

This file contains configuration and commands for Claude Code to help with development tasks.

## Build Commands

### Windows (Visual Studio 2022)
```bash
# Configure and build with CMake (using overlay port for Draco support)
cmake -B build-vs2022 -S . -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_TOOLCHAIN_FILE=c:/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_OVERLAY_PORTS=./vcpkg-overlays

cmake --build build-vs2022 --config Release
```

**Note:** The `vcpkg-overlays/assimp` directory contains a modified assimp port with `ASSIMP_BUILD_DRACO=ON` enabled.

## Test Commands
```bash
# Run tests (if available)
cd build-vs2022
ctest --config Release
```

## Lint and Type Check Commands
```bash
# Add specific lint/format commands here when identified
```

## Project Structure
- `build-vs2022/` - Visual Studio 2022 build directory
- Source files and project configuration in root directory