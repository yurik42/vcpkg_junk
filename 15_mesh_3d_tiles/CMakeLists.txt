cmake_minimum_required(VERSION 3.22)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

project(15_mesh_3d_tiles)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


if(BUILD_TESTING)
     enable_testing()
     include(GoogleTest)
     find_package(GTest REQUIRED)
     find_package(assimp CONFIG REQUIRED)

     add_executable(test_15
         test_mesh_3d_tiles.cpp
     )

     target_link_libraries(
         test_15
         PRIVATE
         GTest::gtest
         GTest::gtest_main
         assimp::assimp
     )

     gtest_discover_tests(test_15)
     set_target_properties(test_15 PROPERTIES FOLDER "Tests")
 endif()
