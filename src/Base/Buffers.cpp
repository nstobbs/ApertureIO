#include "Buffers.hpp"

#include "../Vulkan/VulkanBuffers.hpp"

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

}

