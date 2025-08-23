#include "ApertureIO/VulkanTexture.hpp"
#include "ApertureIO/VulkanCommand.hpp"

#include "ApertureIO/Logger.hpp"

namespace Aio
{

VulkanTexture::VulkanTexture(const TextureCreateInfo& createInfo)
{
    _pDevice = dynamic_cast<VulkanDevice*>(createInfo.pDevice);
    _pContext = dynamic_cast<VulkanContext*>(createInfo.pContext);
    _filepath = createInfo.filePath;

    // Read the Source File
    readTextureSourceFile();
    VkDeviceSize imageSize = _width * _height * _channels;

    // Create VulkanBuffer
    VkBuffer buffer;
    VmaAllocation bufferAllocation;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocationInfo{};
    allocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VK_ASSERT(vmaCreateBuffer(_pDevice->GetVmaAllocator(), &bufferInfo, &allocationInfo, &buffer, &bufferAllocation, nullptr),
                             VK_SUCCESS, "VulkanTexture: Failed to Create VkBuffer...");

    // Copy Pixel Data to the VulkanBuffer
    void* dataPtr;
    vmaMapMemory(_pDevice->GetVmaAllocator(), bufferAllocation, &dataPtr);
    memcpy(dataPtr, _pixels, imageSize);

    vmaUnmapMemory(_pDevice->GetVmaAllocator(), bufferAllocation);
    freePixels();

    // Create Vulkan Image 
    _format = VK_FORMAT_R8G8B8A8_SRGB;
    _currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = _width;
    imageInfo.extent.height = _height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = _format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    VmaAllocationCreateInfo imageAllocationInfo{};
    imageAllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocation imageAllocation;

    VK_ASSERT(vmaCreateImage(_pDevice->GetVmaAllocator(), &imageInfo, &imageAllocationInfo, &_image, &imageAllocation, nullptr),
                            VK_SUCCESS, "VulkanTexture: Failed to Create VkImage...");
    
    // Change the Image Layout, For Copying To..
    setImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Copy Image to Buffer
    VulkanCommand::CopyBufferToImage(_pDevice, buffer, _image, static_cast<uint32_t>(_width), static_cast<uint32_t>(_height));

    // Change the Image Layout, For Shader Access
    setImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Free the Buffer
    vmaDestroyBuffer(_pDevice->GetVmaAllocator(), buffer, bufferAllocation);

    // Create an Image View
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = _format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VK_ASSERT(vkCreateImageView(_pDevice->GetVkDevice(), &viewInfo, nullptr, &_imageView), VK_SUCCESS, "VulkanTexture: Failed to create VkImageView...");

    // Create and Store Texture Handle
    _handle = storeTextureHandle();
};

void VulkanTexture::setImageLayout(VkImageLayout targetLayout)
{
    VkCommandBuffer commandBuffer = VulkanCommand::beginSingleTimeCommandBuffer(_pDevice);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = _currentLayout;
    barrier.newLayout = targetLayout;
    barrier.image = _image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (_currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && targetLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (_currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && targetLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {  
        Logger::LogError("VulkanTexture: Selected TargetLayout isn't Supported.");
        throw std::runtime_error("Exit...");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrier);
    VulkanCommand::endSingleTimeCommandBuffer(_pDevice, commandBuffer);

    _currentLayout = targetLayout;
};

TextureHandle VulkanTexture::storeTextureHandle()
{
    TextureHandle handle = _pDevice->CreateTextureHandle();
    
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = _currentLayout;
    imageInfo.imageView = _imageView;
    imageInfo.sampler = _pDevice->GetGlobalVkSampler();

    VkWriteDescriptorSet writeInfo{};
    writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.dstSet = _pDevice->GetBindlessDescriptorSet();
    writeInfo.dstBinding = 2; // UniformBuffers at 0; StorageBuffers at 1; Textures at 2;
    writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeInfo.descriptorCount = 1;
    writeInfo.dstArrayElement = handle;
    writeInfo.pImageInfo = &imageInfo;
    
    vkUpdateDescriptorSets(_pDevice->GetVkDevice(), 1, &writeInfo, 0, nullptr);
    return handle;
};

//TODO: this should just be part of base texture and not related to vulkan at all.
TextureHandle VulkanTexture::GetTextureHandle()
{
    return _handle;
};

void VulkanTexture::Bind(RenderContext& rContext)
{
    rContext._Textures.push_back(dynamic_cast<Texture*>(this));
};

void VulkanTexture::Unbind()
{
    Logger::LogInfo("Hello There!");
};

void VulkanTexture::sourceFileModified()
{
    Logger::LogInfo("Hello There!");
};

};