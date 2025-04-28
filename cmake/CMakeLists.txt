cmake_minimum_required(VERSION 3.10)

project(ApertureIO)
set(CXX_STANDARD_REQUIRED 20)

find_package(volk)
find_package(glfw3)

add_executable(ApertureIO ../src/main.cpp

                          ../src/App/Window.hpp
                          ../src/App/Window.cpp

                          ../src/Base/Context.hpp
                          ../src/Base/Context.cpp

                          ../src/Base/Device.hpp
                          ../src/Base/Device.cpp

                          ../src/Base/FrameBuffer.hpp
                          ../src/Base/FrameBuffer.cpp

                          ../src/Vulkan/VulkanCommon.hpp

                          ../src/Vulkan/VulkanContext.hpp
                          ../src/Vulkan/VulkanContext.cpp

                          ../src/Vulkan/VulkanDevice.hpp
                          ../src/Vulkan/VulkanDevice.cpp

                          ../src/Vulkan/VulkanFrameBuffer.hpp
                          ../src/Vulkan/VulkanFrameBuffer.cpp

                          ../deps/vk-bootstrap/src/VkBootstrap.h
                          ../deps/vk-bootstrap/src/VkBootstrap.cpp
                          ../deps/vk-bootstrap/src/VkBootstrapDispatch.h)
target_link_libraries(ApertureIO PRIVATE volk::volk glfw)