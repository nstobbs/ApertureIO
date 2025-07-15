#pragma once

#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Buffers.hpp"
#include "ApertureIO/BufferLayout.hpp"
#include "ApertureIO/RenderContext.hpp"

#include <utility>

namespace Aio {

//TODO: Think of a better name for this.
struct BufferBlock
{
    VkBuffer vkBufferHandle;
    VmaAllocation vmaAllocationHandle;
};

class VulkanBuffer : public Buffer
{
    public:
    ~VulkanBuffer();
    VulkanBuffer(BufferCreateInfo* createInfo);

    void UploadToDevice(void* data);
    
    /* Might need to function to rebuild the buffer */
    void rebuildBuffer(); //destory and rebuild the buffer.

    void Bind(RenderContext& renderContext) override;
    void Unbind() override;

    VkBuffer GetBuffer();

    private:
    BufferBlock createVkBuffer(size_t size, VkBufferUsageFlags usage); //TODO: not sure if using pairs are a great idea here.

    BufferHandle storeBufferHandle();
    BufferHandle _handle;
    
    VulkanDevice* _pDevice;

    BufferType _type;
    BufferLayout _layout;

    VkBuffer _buffer;
    VmaAllocation _allocation;

    uint32_t _size;
    void* _pData;
};
}; // End of Aio Namespace