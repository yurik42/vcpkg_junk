# vcpkg Overlay Ports

This directory contains custom vcpkg port overlays that modify the default port configurations.

## assimp

The assimp overlay enables Draco support by setting `ASSIMP_BUILD_DRACO=ON` and patching the CMakeLists.txt to use vcpkg's draco package.

### Changes from default port:
1. Added `-DASSIMP_BUILD_DRACO=ON` to the CMake configuration options (portfile.cmake:41)
2. Patched `CMakeLists.txt` to use vcpkg's draco via `find_package(draco CONFIG)` instead of building from bundled sources (portfile.cmake:12-15)

### Technical Details:
The standard assimp port removes the bundled `contrib/draco` directory to avoid conflicts. By default, assimp expects to either:
- Build draco from `contrib/draco` (which vcpkg removes), or
- Use Hunter package manager (which conflicts with vcpkg)

This overlay adds a patch that checks for `VCPKG_TOOLCHAIN` and uses `find_package(draco CONFIG)` to link against vcpkg's draco package.

### Verification:
After building, you can verify Draco support is enabled by checking:
```bash
grep "ASSIMP_BUILD_DRACO" c:/opt/vcpkg/buildtrees/assimp/config-x64-windows-rel-CMakeCache.txt.log
# Should show: ASSIMP_BUILD_DRACO:BOOL=ON
```

### Usage:
When configuring your project with CMake, specify the overlay directory:
```bash
cmake -B build-vs2022 -S . -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_TOOLCHAIN_FILE=c:/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_OVERLAY_PORTS=./vcpkg-overlays
```

This will cause vcpkg to use the modified assimp port with Draco support instead of the default one.
