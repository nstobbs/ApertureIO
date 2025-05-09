#include "VulkanBuffers.hpp"

namespace ApertureIO {

VulkanBuffer::VulkanBuffer(Device* device, void* pData, BufferLayout layout, uint32_t vertexCount, bool hostAccess)
{
    _pDevice = dynamic_cast<VulkanDevice*>(device);
    size_t size = static_cast<uint32_t>(layout.GetStride() * vertexCount);
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; // TODO check if any other flags are needed here!
    
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; // TODO: check this flag as well.
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

    VmaAllocator allocator = _pDevice->getVmaAllocator();
    VkBuffer buffer;
    VmaAllocation allocation;
    vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    vmaCopyMemoryToAllocation(allocator, pData, _allocation, 0, size);

    /* Seems like we still need to manually move the buffer from the slow part of the gpu 
    and move it to the more performance side.
    Could add in the buffer create stuct if we need to access via the cpu with the buffer.
    */

   if (!hostAccess)
   {
        VkBufferCreateInfo extraBufferInfo{};
        extraBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        extraBufferInfo.size = size;
        extraBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        
        VmaAllocationCreateInfo extraAllocInfo{};
        extraAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
       // extraAllocInfo.flags = VMA_ALLOCATION_CREATE
       VkBuffer extraBuffer;
       VmaAllocation extraAllocation;
       vmaCreateBuffer(allocator, &extraBufferInfo, &extraAllocInfo, &extraBuffer, &extraAllocation, nullptr);
       /* TODO: Need commands buffers before we can copy the buffer across the gpu...*/
   };
};

void VulkanBuffer::Bind()
{

};

void VulkanBuffer::Unbind()
{

};

} // End ApertureIO namespace