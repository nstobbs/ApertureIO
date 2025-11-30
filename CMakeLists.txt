# Root CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

project(ApertureIO)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(ApertureIO)
add_subdirectory(EditorApplication)
add_subdirectory(TestApplication)
add_subdirectory(ThirdParty)