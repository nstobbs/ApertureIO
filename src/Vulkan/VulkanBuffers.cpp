#include "VulkanBuffers.hpp"
#include "VulkanCommand.hpp"

namespace Aio {

VulkanBuffer::VulkanBuffer(Device* device, void* pData, BufferLayout layout, uint32_t vertexCount, bool hostAccess)
{
    _pDevice = dynamic_cast<VulkanDevice*>(device);
    size_t size = static_cast<uint32_t>(layout.GetStride() * vertexCount);
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; //TODO: need to sort this out, as buffers can only be used as vertex buffers here.
    
    
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; // TODO: check this flag as well.
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

    VmaAllocator allocator = _pDevice->GetVmaAllocator();
    VkBuffer buffer;
    VmaAllocation allocation;
    VK_ASSERT(vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr), VK_SUCCESS, "Create Host + Device Buffer");
    VK_ASSERT(vmaCopyMemoryToAllocation(allocator, pData, allocation, 0, size), VK_SUCCESS, "Copy From Host to Buffer");

   if (!hostAccess)
   {
        /* Create A Device Only Buffer and Copy theSlow Buffer Over*/
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
       VK_ASSERT(vmaCreateBuffer(allocator, &extraBufferInfo, &extraAllocInfo, &extraBuffer, &extraAllocation, nullptr), VK_SUCCESS, "Create Device Only Buffer");
       
       VulkanCommand::CopyBuffer(_pDevice, buffer, extraBuffer, size);
       _buffer = extraBuffer;
       _allocation = extraAllocation;

       // free the uneeded buffer
       vmaDestroyBuffer(allocator, buffer, allocation);
   }
   else {
        _buffer = buffer;
        _allocation = allocation;
   }
};

void VulkanBuffer::Bind()
{

};

void VulkanBuffer::Unbind()
{

};

VulkanBuffer::~VulkanBuffer()
{
    vmaDestroyBuffer(_pDevice->GetVmaAllocator(), _buffer, _allocation);
};

} // End Aio namespace