cmake_minimum_required(VERSION 3.10)
project(14_vulkan_primer)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(vulkan_app vulkan_app.cpp)
target_link_libraries(vulkan_app Vulkan::Vulkan glfw)

add_executable(vulkan_triangle vulkan_triangle.cpp)
target_link_libraries(vulkan_triangle Vulkan::Vulkan glfw)

find_package(glm REQUIRED)

add_executable(vulkan_cube vulkan_cube.cpp)
target_link_libraries(vulkan_cube Vulkan::Vulkan glfw glm::glm)
