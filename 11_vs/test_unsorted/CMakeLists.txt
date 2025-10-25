cmake_minimum_required(VERSION 3.15)
project(test_unsorted)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(BUILD_TESTING)
    enable_testing()
    include(GoogleTest)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
    find_package(GTest REQUIRED)

    # Add your source files here
    add_executable(test_unsorted 
        main.cpp
        test_unsorted.h
        virtual_base_test.cpp
        virtual_base_test_plain.h
        virtual_base_test_virt.h
    )

    # Find Boost (adjust components as needed)
    # find_package(Boost REQUIRED COMPONENTS geometry)

    target_link_libraries(test_unsorted PRIVATE GTest::gtest)
    gtest_discover_tests(test_unsorted)
    set_target_properties(test_unsorted PROPERTIES FOLDER "Tests")

endif()
