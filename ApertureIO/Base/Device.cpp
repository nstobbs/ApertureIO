#include "Device.hpp"
#include "../Vulkan/VulkanDevice.hpp"

namespace Aio {

 Device* Device::CreateDevice(Context* context)
 {
    switch(context->getRendererAPI())
    {
        case eVulkan:
            VulkanDevice* device = new VulkanDevice(context);
            dynamic_cast<Device*>(device)->pCommand = Command::CreateCommand(context, dynamic_cast<Device*>(device));
            return device;
    }
 };


/* TODO: These functions should fail when we 
try and go above our device limits. */
BufferHandle Device::CreateUniformBufferHandle()
{
    BufferHandle handle = static_cast<BufferHandle>(_uniformBufferHandles.size());
    _uniformBufferHandles.push_back(handle);
    return handle;
};

BufferHandle Device::CreateStorageBufferHandle()
{
    BufferHandle handle = _storageBufferHandles.size();
    _storageBufferHandles.push_back(handle);
    return handle;
};

TextureHandle Device::CreateTextureHandle()
{
    TextureHandle handle = _textureHandles.size();
    _textureHandles.push_back(handle);
    return handle;
};

} // End Aio namespace