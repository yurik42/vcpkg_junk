# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a collection of C++ experimental projects demonstrating vcpkg dependency management and various spatial computing, geometry, and 3D processing libraries. The repository is structured as a mono-repo with 15 independent subdirectories (01-15), each showcasing different vcpkg integration patterns and library usage.

**Key focus areas:**
- vcpkg manifest mode and feature flags
- Spatial data structures (R-trees, octrees)
- 3D mesh and point cloud processing (Assimp, PCL, VTK)
- Cesium 3D Tiles generation
- Geometry processing (CGAL, Boost.Geometry, PROJ)

## Build System

The project uses **CMake with vcpkg in manifest mode**. Dependencies are controlled through a root `vcpkg.json` with feature flags for each subdirectory.

### CMake Presets

Multiple presets are configured in `CMakeUserPresets.json` for different environments:

**Windows:**
- `default` - Uses `VCPKG_ROOT=g:\opt\vcpkg` (Visual Studio 2022)
- `windows-developmentyk` - Uses `VCPKG_ROOT=c:\opt\vcpkg`
- `cherry` - Uses `VCPKG_ROOT=e:\opt\vcpkg` (disables BUILD_09, BUILD_03)

**Linux:**
- `kestrel` - Uses `VCPKG_ROOT=/home/yyk/src/vcpkg` (Unix Makefiles)

### Build Commands

**Windows (Visual Studio 2022):**
```bash
# Configure
cmake --preset default
# or
cmake --preset windows-developmentyk

# Build all targets
cmake --build build-vs2022

# Build specific configuration
cmake --build build-vs2022 --config Release
cmake --build build-vs2022 --config Debug
```

**Linux:**
```bash
# Configure
cmake --preset kestrel

# Build
cmake --build build-linux
```

### Running Tests

Tests use GoogleTest and are enabled by default via `BUILD_TESTING` option:

```bash
# Windows
ctest --test-dir build-vs2022 -C Debug

# Linux
ctest --test-dir build-linux
```

### Building Individual Subdirectories

The root CMakeLists.txt has `BUILD_XX` options (BUILD_01 through BUILD_11) to control which subdirectories are built. These map to vcpkg manifest features:

```bash
# Disable specific projects
cmake --preset default -DBUILD_09=OFF -DBUILD_03=OFF
```

Some subdirectories (08_assimp_junk, 15_mesh_3d_tiles) can be built standalone:

```bash
cd 08_assimp_junk
cmake --preset default
cmake --build build-vs2022
```

## Project Structure

Subdirectories are numbered experiments (01-15):

- **01_boost_static** - Static Boost linking
- **02_cmake_toolchain** - CMake toolchain examples
- **03_vcpkg_json** - vcpkg.json manifest basics (VTK)
- **04_cgal_tbb** - CGAL with TBB threading
- **05_manifest_features** - Conditional vcpkg features
- **06_cgal_tbb_novcpkg** - CGAL/TBB without vcpkg
- **07_cmake_qt** - Qt integration
- **08_assimp_junk** - Assimp mesh processing, Draco compression, tinygltf, Cesium utilities
- **09_vtk_tutorial** - VTK examples, Voronoi diagrams
- **10_proj** - PROJ coordinate transformations (LV95 Swiss coordinates)
- **11_vs** - Visual Studio spatial data structures (R-trees, octrees, PCL)
- **12_cgal** - Additional CGAL examples
- **13_assimp_dynamic** - Dynamic Assimp linking
- **14_vulkan_primer** - Vulkan graphics basics
- **15_mesh_3d_tiles** - Cesium 3D mesh tiles (current work)

Each subdirectory typically contains:
- Standalone `CMakeLists.txt`
- Command-line applications prefixed `cmd_*`
- Test files prefixed `test_*`
- Optional `vcpkg.json` for standalone builds
- Some have their own `CLAUDE.md` with subdirectory-specific guidance

## Dependencies

Root `vcpkg.json` includes:
- **boost-program-options, boost-filesystem, boost-geometry, boost-polygon**
- **gtest** (when BUILD_TESTING=ON)

Feature-specific dependencies (see vcpkg.json):
- **VTK** (03, 09)
- **CGAL, TBB** (04, 06, 09)
- **Assimp, tinygltf, glm, json-c, eigen3, draco** (08, 13, 15)
- **PCL, fmt, stb** (11)
- **PROJ** (10)

## Environment Setup

**Required:**
- Set `VCPKG_ROOT` environment variable pointing to your vcpkg installation
- Configure appropriate preset in `CMakeUserPresets.json` for your machine

**Optional:**
- Qt installation (for 07_cmake_qt): Set `CMAKE_PREFIX_PATH` in preset

## Architecture Notes

- All executables output to `build-*/bin/`, libraries to `build-*/lib/`
- C++17 standard throughout
- Visual Studio builds use `/utf-8` flag for Unicode support
- Uses CMake folder organization (targets grouped by "Apps", "Tests")
- Some projects use vcpkg overlay ports (e.g., 08_assimp_junk/vcpkg-overlays for Draco-enabled Assimp)

## Development Patterns

**Adding a new subdirectory experiment:**
1. Create directory with CMakeLists.txt
2. Add option and feature flag to root CMakeLists.txt (lines 16-70)
3. Add `add_subdirectory()` call in root CMakeLists.txt (lines 81-115)
4. Add feature to root `vcpkg.json` with required dependencies
5. Update preset cache variables if needed (e.g., to disable by default)

**Running single test suite:**
```bash
# From build directory
./bin/test_15  # Linux
.\bin\Debug\test_15.exe  # Windows
```

**Testing with different vcpkg configurations:**
The project supports both manifest mode (default) and classic mode builds. Some presets use `VCPKG_INSTALLED_DIR` for shared installations.

## Git Workflow

- Main branch: `master`
- Feature branches: `devel/yurik42/*`
- Current branch: `devel/yurik42/15_mesh_3d_tiles`

## Special Considerations

- **08_assimp_junk**: Uses vcpkg overlay port to enable Draco support in Assimp (`ASSIMP_BUILD_DRACO=ON`)
- **09_vtk_tutorial**: VTK builds are large; disabled by default in some presets
- **03_vcpkg_json**: VTK dependency; also disabled in some presets
- **Windows long command lines**: Ninja response files enabled (`CMAKE_NINJA_FORCE_RESPONSE_FILE`)
- **MSVC debug symbols**: Some test targets explicitly add `/Zi` flag

## Typical Workflow

```bash
# 1. Configure with appropriate preset
cmake --preset default  # or kestrel for Linux

# 2. Build
cmake --build build-vs2022  # or build-linux

# 3. Run tests
ctest --test-dir build-vs2022 -C Debug

# 4. Run specific executable
./build-linux/bin/cmd_boost_rtree_primer
```
