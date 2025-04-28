#include "VulkanFrameBuffer.hpp"

namespace ApertureIO {

VulkanFrameBuffer::VulkanFrameBuffer(Device* device, Context* context)
{
    // create with swap chain
    _pDevice = dynamic_cast<VulkanDevice*>(device);
    _pContext = dynamic_cast<VulkanContext*>(context);
    
};


VulkanFrameBuffer::VulkanFrameBuffer(Device* device)
{
    // create without swap chain
    _pDevice = dynamic_cast<VulkanDevice*>(device);
};

bool VulkanFrameBuffer::init()
{
    std::cout << ":0\n";
    return false;
};
} // End ApertureIO namespace