find_package(volk REQUIRED)
find_package(glm REQUIRED)
find_package(shaderc REQUIRED)
find_package(efsw REQUIRED)
find_package(Boost REQUIRED)

add_subdirectory(Common)
add_subdirectory(Base)
add_subdirectory(Vulkan)