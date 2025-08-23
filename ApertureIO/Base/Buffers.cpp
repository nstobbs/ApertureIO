#include "ApertureIO/Buffers.hpp"
#include "ApertureIO/VulkanBuffers.hpp"

#include <iostream>

namespace Aio {

UniquePtr<Buffer> Buffer::CreateBuffer(BufferCreateInfo& createInfo)
{
    auto API = createInfo.context->getRendererAPI();
    switch(API)
    {
        case RendererAPI::eVulkan:
            return std::make_unique<VulkanBuffer>(createInfo);
    }
};

BufferLayout Buffer::GetBufferLayout()
{
    return _layout;
};

uint32_t Buffer::Count()
{
    return _count;
}

void Buffer::SetBufferLayout(BufferLayout layout)
{
    _layout = layout;
};

}

