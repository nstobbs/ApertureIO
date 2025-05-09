#include "Buffers.hpp"

#include "../Vulkan/VulkanBuffers.hpp"

#include <iostream>

namespace ApertureIO {

Buffer* Buffer::CreateBuffer(BufferCreateInfo* createInfo)
{
    auto API = createInfo->context->getRendererAPI();
    switch(API)
    {
        case eVulkan:
            return new VulkanBuffer(createInfo->device, createInfo->data,
                                    createInfo->layout, createInfo->count,
                                    createInfo->hostAccess);
    }
};

}

