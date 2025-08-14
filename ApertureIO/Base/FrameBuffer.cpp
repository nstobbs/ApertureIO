#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"

namespace Aio {

std::shared_ptr<FrameBuffer> FrameBuffer::CreateFrameBuffer(const FrameBufferCreateInfo& createInfo)
{
    switch(createInfo.pDevice->getRendererAPI())
    {
        case RendererAPI::eVulkan:
            VulkanFrameBuffer* framebuffer = new VulkanFrameBuffer(createInfo);
            return framebuffer;
    }
};

void FrameBuffer::setName(std::string name)
{
    _name = name;
};

} // End Aio namespace

