cmake_minimum_required(VERSION 3.10)

project(ApertureIO)
set(CXX_STANDARD_REQUIRED 20)

find_package(Vulkan REQUIRED FATAL_ERROR)
find_package(volk REQUIRED CONFIG)
find_package(vk-bootstrap REQUIRED CONFIG)
find_package(vulkan-memory-allocator REQUIRED CONFIG)

add_executable(ApertureIO ../src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan
                                      volk::volk_headers
                                      vk-bootstrap::vk-bootstrap
                                      vulkan-memory-allocator::vulkan-memory-allocator)