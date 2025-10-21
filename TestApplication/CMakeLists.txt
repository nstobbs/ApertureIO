find_package(glfw3 REQUIRED) # TODO: remove this when no longer needed
find_package(Qt6 REQUIRED COMPONENTS Gui Core)

add_subdirectory(TestApplication)
add_subdirectory(Window)