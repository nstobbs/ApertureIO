#include "ApertureIO/Buffers.hpp"
#include "ApertureIO/VulkanBuffers.hpp"

#include <iostream>

namespace Aio {

Buffer* Buffer::CreateBuffer(BufferCreateInfo* createInfo)
{
    auto API = createInfo->context->getRendererAPI();
    switch(API)
    {
        case eVulkan:
            return new VulkanBuffer(createInfo);
    }
};

BufferLayout Buffer::GetBufferLayout()
{
    return _layout;
};

void Buffer::SetBufferLayout(BufferLayout layout)
{
    _layout = layout;
};

}

