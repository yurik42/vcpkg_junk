# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a CMake-based C++ project exploring spatial data structures and algorithms, primarily using Boost Geometry, PCL (Point Cloud Library), and fmt for formatting. The project consists of multiple command-line applications, each demonstrating different concepts:

- **cmd_app1**: Basic fmt library usage examples
- **cmd_boost_rtree_primer**: Boost R-tree spatial indexing primer
- **cmd_boost_rtree_traversal**: R-tree traversal algorithms
- **cmd_boost_rtree_view_example**: R-tree visualization examples
- **cmd_cesium_3d_tiles_pointcloud**: 3D tiles and point cloud processing
- **cmd_mem_allocate**: Memory allocation testing
- **cmd_octree_primer**: Basic octree data structure examples
- **cmd_pcl_octree_primer**: PCL octree implementation examples
- **cmd_rtree_primer**: Generic R-tree implementation primer
- **cmd_simple_boost_rstar_example**: Boost R*-tree examples
- **cmd_sombrero_elevation_generator**: Elevation data generation

## Build System

The project uses **vcpkg** in manifest mode for dependency management with Visual Studio integration.

### Build Commands

**Visual Studio (Windows):**
```bash
# Configure with vcpkg preset
cmake --preset vcpkg
# Build all targets
cmake --build build-vs2022
# Or open in Visual Studio
start 11_vs.sln
```

**Linux/GCC:**
```bash
# Manual configure with vcpkg
mkdir build-linux && cd build-linux
cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake ..
# Build
cmake --build .
```

### Running Individual Applications
```bash
# From build directory
./bin/cmd_app1
./bin/cmd_boost_rtree_primer
# etc.
```

## Dependencies

Main dependencies managed through `vcpkg.json`:
- **fmt**: String formatting library
- **pcl**: Point Cloud Library for 3D processing
- **stb**: Image processing utilities
- **boost-geometry**: Spatial algorithms and data structures
- **gtest**: Testing framework (optional, enabled with BUILD_TESTING)

## Architecture Notes

- Each `cmd_*` directory contains a self-contained application with its own CMakeLists.txt
- All applications use C++17 standard
- Build outputs go to `build-*/bin/` for executables and `build-*/lib/` for libraries  
- Project uses folder organization in IDE with "Apps 11_vs" folder grouping
- MSVC builds include `/utf-8` compiler flag for Unicode support

## Development Workflow

1. Make changes to source files in respective `cmd_*` directories
2. Build specific target: `cmake --build build-vs2022 --target <target_name>`
3. Run executable from build directory
4. For new applications, add subdirectory to root CMakeLists.txt and include in set_target_properties

## vcpkg Integration

Initial setup requires:
```bash
cd cmd_app1  # or any subdirectory with vcpkg usage
vcpkg integrate install
```

The project uses vcpkg baseline `6ecbbbdf31cba47aafa7cf6189b1e73e10ac61f8` for reproducible builds.