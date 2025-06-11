add_library(Vulkan STATIC 
        VulkanBuffers.cpp
        VulkanCommand.cpp
        VulkanCommon.cpp
        VulkanContext.cpp
        VulkanDevice.cpp
        VulkanFrameBuffer.cpp
        VulkanShader.cpp
)

target_include_directories(Vulkan PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Vulkan
    volk::volk
    shaderc::shaderc
    vk-bootstrap::vk-bootstrap
    GPUOpen::VulkanMemoryAllocator

    Aio::Common
    Aio::Base
)

add_library(Aio::Vulkan ALIAS Vulkan)