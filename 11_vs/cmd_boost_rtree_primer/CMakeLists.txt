cmake_minimum_required(VERSION 3.15)
project(cmd_boost_rtree_primer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your source files here
add_executable(cmd_boost_rtree_primer 
    cmd_boost_rtree_primer.cpp)

# Find Boost (adjust components as needed)
find_package(Boost REQUIRED)

target_include_directories(cmd_boost_rtree_primer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(cmd_boost_rtree_primer PRIVATE ${Boost_LIBRARIES})
