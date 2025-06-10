add_library(Vulkan STATIC 
        VulkanBuffers.cpp
        VulkanCommand.cpp
        VulkanCommon.cpp
        VulkanContext.cpp
        VulkanDevice.cpp
        VulkanFrameBuffer.cpp
        VulkanShader.cpp
)

add_library(Aio::Vulkan ALIAS Vulkan)