#include "FrameBuffer.hpp"
#include "../Vulkan/VulkanFrameBuffer.hpp"

namespace ApertureIO {

FrameBuffer* FrameBuffer::CreateFrameBuffer(Device* pDevice, Context* pContext)
{
    // if the context was passed that we treat this as the swap chain framebuffer
    switch(pContext->getRendererAPI())
    {
        case eVulkan:
            VulkanFrameBuffer* framebuffer = new VulkanFrameBuffer(pDevice, pContext);
            framebuffer->isSwapChainTarget = true;
            return framebuffer;
    }
};

} // End ApertureIO namespace

