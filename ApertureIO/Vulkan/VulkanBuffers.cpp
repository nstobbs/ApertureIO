#include "VulkanBuffers.hpp"
#include "VulkanCommand.hpp"

namespace Aio {

VulkanBuffer::VulkanBuffer(BufferCreateInfo* createInfo)
{
    _pDevice = dynamic_cast<VulkanDevice*>(createInfo->device);
    size_t size = static_cast<uint32_t>(createInfo->layout.GetStride() * createInfo->count);
    _size = static_cast<uint32_t>(size);
    _type = createInfo->type;

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

   if (createInfo->type == BufferType::Vertex || BufferType::Index)
   {
        /* If it's a Vertex or Index then the data pointer is copied from the CPU */
        VK_ASSERT(vmaCopyMemoryToAllocation(allocator, createInfo->data, allocation, 0, size), VK_SUCCESS, "Copy From Host to Buffer");
        /* Create A Device Only Buffer and Copy theSlow Buffer Over*/
        VkBufferCreateInfo extraBufferInfo{};
        extraBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        extraBufferInfo.size = size;

        if (createInfo->type == BufferType::Vertex)
        {
            extraBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }
        else 
        {
            extraBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }
        
        
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
        /* When using UniformBuffers the data pointer is used for mapping .*/
        _buffer = buffer;
        _allocation = allocation;
        vmaMapMemory(_pDevice->GetVmaAllocator(), _allocation, &createInfo->data);
        _pData = createInfo->data;
        _handle = storeBufferHandle();
   }

   dynamic_cast<Buffer*>(this)->SetBufferLayout(createInfo->layout);
};

void VulkanBuffer::UploadToDevice(void* data)
{
    memcpy(_pData, data, static_cast<size_t>(_size));
};

// TODO: Useble for Uniform Buffers and Storage Buffers
BufferHandle VulkanBuffer::storeBufferHandle()
{
    BufferHandle handle = _pDevice->CreateUniformBufferHandle();
    
    // Create the Write Descriptor Set.
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = _buffer;
    bufferInfo.range = VK_WHOLE_SIZE;
    bufferInfo.offset = 0;

    VkWriteDescriptorSet writeInfo{};
    writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.dstSet = _pDevice->GetBindlessDescriptorSet();
    writeInfo.dstBinding = 0; // UniformBuffers at 0;
    writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeInfo.descriptorCount = 1;
    writeInfo.dstArrayElement = handle;
    writeInfo.pBufferInfo = &bufferInfo;
    
    vkUpdateDescriptorSets(_pDevice->GetVkDevice(), 1, &writeInfo, 0, nullptr);
    return handle;
};

void VulkanBuffer::rebuildBuffer()
{
    vmaDestroyBuffer(_pDevice->GetVmaAllocator(), _buffer, _allocation);
};


void VulkanBuffer::Bind(RenderContext& renderContext)
{
    switch(_type)
    {
        case BufferType::Vertex:
            renderContext._VertexBuffer = dynamic_cast<Buffer*>(this);
            break;
        case BufferType::Index:
            renderContext._IndexBuffer = dynamic_cast<Buffer*>(this);
        case BufferType::Uniform:
            break;
    };
};

void VulkanBuffer::Unbind()
{

};

VulkanBuffer::~VulkanBuffer()
{
    vmaDestroyBuffer(_pDevice->GetVmaAllocator(), _buffer, _allocation);
};

VkBuffer VulkanBuffer::GetBuffer()
{
    return _buffer;
};

} // End Aio namespace