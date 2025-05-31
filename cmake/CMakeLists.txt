cmake_minimum_required(VERSION 3.10)

project(ApertureIO)
set(CXX_STANDARD_REQUIRED 20)

find_package(volk REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(shaderc REQUIRED)
find_package(efsw REQUIRED)
find_package(Boost REQUIRED)

add_executable(ApertureIO ../src/main.cpp

                          ../src/ApertureIO.hpp

                          ../src/App/Window.hpp
                          ../src/App/Window.cpp

                          ../src/Common/Logger.hpp
                          ../src/Common/Logger.cpp

                          ../src/Common/FileIO.hpp
                          ../src/Common/FileIO.cpp

                          ../src/Base/Context.hpp
                          ../src/Base/Context.cpp

                          ../src/Base/Device.hpp
                          ../src/Base/Device.cpp

                          ../src/Base/FrameBuffer.hpp
                          ../src/Base/FrameBuffer.cpp

                          ../src/Base/Buffers.hpp
                          ../src/Base/Buffers.cpp

                          ../src/Base/BufferLayout.hpp
                          ../src/Base/BufferLayout.cpp

                          ../src/Base/Shader.hpp
                          ../src/Base/Shader.cpp
                        
                          ../src/Base/Command.hpp

                          ../src/Base/RenderContext.hpp
                          ../src/Base/RenderContext.cpp

                          ../src/Vulkan/VulkanCommon.hpp
                          ../src/Vulkan/VulkanCommon.cpp

                          ../src/Vulkan/VulkanContext.hpp
                          ../src/Vulkan/VulkanContext.cpp

                          ../src/Vulkan/VulkanDevice.hpp
                          ../src/Vulkan/VulkanDevice.cpp

                          ../src/Vulkan/VulkanFrameBuffer.hpp
                          ../src/Vulkan/VulkanFrameBuffer.cpp

                          ../src/Vulkan/VulkanBuffers.hpp
                          ../src/Vulkan/VulkanBuffers.cpp

                          ../src/Vulkan/VulkanShader.hpp
                          ../src/Vulkan/VulkanShader.cpp

                          ../src/Vulkan/VulkanCommand.hpp
                          ../src/Vulkan/VulkanCommand.cpp

                          ../deps/vk-bootstrap/src/VkBootstrap.h
                          ../deps/vk-bootstrap/src/VkBootstrap.cpp
                          ../deps/vk-bootstrap/src/VkBootstrapDispatch.h
                          ../deps/VulkanMemoryAllocator/include/vk_mem_alloc.h)
                          
target_link_libraries(ApertureIO PRIVATE volk::volk glfw glm::glm shaderc::shaderc efsw::efsw boost::boost)
