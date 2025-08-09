#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"

namespace Aio {

FrameBuffer* FrameBuffer::CreateFrameBuffer(Device* pDevice, Context* pContext)
{
    switch(pContext->getRendererAPI())
    {
        case RendererAPI::eVulkan:
            VulkanFrameBuffer* framebuffer = new VulkanFrameBuffer(pDevice, pContext);
            return framebuffer;
    }
};

void FrameBuffer::setName(std::string name)
{
    _name = name;
};

} // End Aio namespace

