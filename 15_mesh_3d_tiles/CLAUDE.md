# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a test project for 3D mesh tile processing using vcpkg for dependency management. It's part of the larger `vcpkg_junk` repository (https://github.com/yyk99/vcpkg_junk) which contains multiple vcpkg-related experimental projects.

## Build System

The project uses CMake with vcpkg for dependency management. Multiple CMake presets are configured for different development environments in `CMakeUserPresets.json`:
- `default` - Uses VCPKG_ROOT at g:\opt\vcpkg
- `windows-developmentyk` - Uses VCPKG_ROOT at c:\opt\vcpkg
- `cherry` - Uses VCPKG_ROOT at e:\opt\vcpkg
- `kestrel` - Linux build using Unix Makefiles with VCPKG_ROOT at /home/yyk/src/vcpkg

### Build Commands

**Windows:**
```bash
cmake --preset default
cmake --build build-vs2022 --config Debug
# or for release:
cmake --build build-vs2022 --config Release
```

You can also use named build presets:
```bash
cmake --preset default
cmake --build --preset default-debug
# or:
cmake --build --preset default-release
```

**Linux:**
```bash
cmake --preset kestrel
cmake --build build-linux
```

### Running Tests

Tests are built using GoogleTest and are enabled by default via the `BUILD_TESTING` option:

```bash
# After building, run tests with:
ctest --test-dir build-vs2022 -C Debug
# or on Linux:
ctest --test-dir build-linux
```

The test executable is `test_15` and is located in `build-vs2022/bin/Debug/` (Windows) or `build-linux/bin/` (Linux).

## Dependencies

Managed via vcpkg through `vcpkg.json`:
- **fmt** - Formatting library
- **assimp** - Asset import library for 3D model loading
- **gtest** - Google Test framework (conditional, only when BUILD_TESTING is ON)

## Project Structure

This is a single-file test project:
- `test_mesh_3d_tiles.cpp` - Main test file containing GoogleTest tests
- `CMakeLists.txt` - Build configuration with conditional test support
- `vcpkg.json` - Dependency manifest with test feature flag
- `CMakePresets.json` - Base preset using Visual Studio 2022 generator
- `CMakeUserPresets.json` - User-specific presets for different machines (not committed to upstream)

## Git Workflow

- Main branch: `master`
- Current working branch: `devel/yurik42/15_mesh_3d_tiles`
- This is a fork: origin points to yurik42/vcpkg_junk, upstream to yyk99/vcpkg_junk

## Environment Variables

The `VCPKG_ROOT` environment variable must be set and should point to your vcpkg installation. This is configured per preset in `CMakeUserPresets.json`.
