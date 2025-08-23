#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"

namespace Aio {

UniquePtr<FrameBuffer> FrameBuffer::CreateFrameBuffer(const FrameBufferCreateInfo& createInfo)
{
    switch(createInfo.pContext->getRendererAPI())
    {
        case RendererAPI::eVulkan:
            return std::make_unique<VulkanFrameBuffer>(createInfo);
    }
};

} // End Aio namespace

