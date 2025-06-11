#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"

namespace Aio {

FrameBuffer* FrameBuffer::CreateFrameBuffer(Device* pDevice, Context* pContext)
{
    // if the context was passed that we treat this as the swap chain framebuffer
    switch(pContext->getRendererAPI())
    {
        case eVulkan:
            VulkanFrameBuffer* framebuffer = new VulkanFrameBuffer(pDevice, pContext);
            return framebuffer;
    }
};

void FrameBuffer::setName(char* name)
{
    _name = name;
};

} // End Aio namespace

