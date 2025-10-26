## How to configure

Windows

    cmake -S . -B build-vs2022-x64 -DCMAKE_TOOLCHAIN_FILE=E:/opt/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_TESTING=ON

Linux (x64 or arm64)

    cmake -S . -B build-linux -DCMAKE_TOOLCHAIN_FILE=/home/yyk/src/vcpkg/scripts/buildsystems/vcpkg.cmake

or with clang

   cmake -S . -B build-linux -DCMAKE_TOOLCHAIN_FILE=/home/yyk/src/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
