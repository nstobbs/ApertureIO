#include "ApertureIO/VulkanImage.hpp"
#include "ApertureIO/VulkanCommand.hpp"

const bool DEBUG_IMAGE_LAYOUT = false;

namespace Aio
{

UniquePtr<VulkanImage> VulkanImage::CreateVulkanImage(const VulkanImageCreateInfo& createInfo)
{
    /* Create VkImage */
    UniquePtr<VulkanImage> imagePtr = std::make_unique<VulkanImage>();
    imagePtr->_count = createInfo.count;
    imagePtr->_format = createInfo.params.format;
    imagePtr->_height = createInfo.params.height;
    imagePtr->_width = createInfo.params.width;
    imagePtr->_pVulkanDevice = createInfo.params.pVulkanDevice;

    auto device = imagePtr->_pVulkanDevice;

    /* Checks Format is Valid */
    std::vector<VkFormat> requestedFormats;
    requestedFormats.push_back(createInfo.params.format);
    if (findFirstSupportedFormat(device->GetVkPhysicalDevice(), requestedFormats) == VK_FORMAT_UNDEFINED)
    {
        auto error = "Couldn't Find a Supported Format for Requested Image\n";
        Logger::LogError(error);
    };

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = createInfo.params.width;
    imageInfo.extent.height = createInfo.params.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = createInfo.params.format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

    /* TODO: Need to add better logic for knowing if this is going be used as a FrameBuffer
    Layer, Depth Layer or just a Texture Image. For now any images */

    if (imagePtr->GetFormat() == toVkFormat(imagePtr->_pVulkanDevice, FrameBufferPixelFormat::DEPTH_STENCIL_D32_S8))
    {
        /* Depth Stencil Layer */
        imagePtr->_forDepthStencil = true;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                               VK_IMAGE_USAGE_SAMPLED_BIT |
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                               VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    else if (imagePtr->_count == 1)
    {
        /* Texture From Disk */
        imagePtr->_forTextureReading = true;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                          VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    else
    {
        /* Colour Layer */
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                               VK_IMAGE_USAGE_SAMPLED_BIT |
                               VK_IMAGE_USAGE_STORAGE_BIT |
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                               VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    VmaAllocationCreateInfo imageAllocationInfo{};
    imageAllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

    for (int i = 0; i < createInfo.count; i++)
    {
        VmaAllocation imageAllocation;
        VkImage image;

        VK_ASSERT(vmaCreateImage(device->GetVmaAllocator(), &imageInfo, &imageAllocationInfo, &image, &imageAllocation, nullptr),
                                VK_SUCCESS, "VulkanTexture: Failed to Create VkImage...");

        imagePtr->_images.push_back(image);
        imagePtr->_currentLayouts.push_back(VK_IMAGE_LAYOUT_UNDEFINED);
        imagePtr->_imageAllocation.push_back(imageAllocation);
        
         /* Create the VkImageViews */
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = createInfo.params.format;
        viewInfo.subresourceRange.aspectMask = (!imagePtr->_forDepthStencil) ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT
                                                                                                         | VK_IMAGE_ASPECT_STENCIL_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;   

        VkImageView imageView;
        VK_ASSERT(vkCreateImageView(createInfo.params.pVulkanDevice->GetVkDevice(), &viewInfo, nullptr, &imageView), VK_SUCCESS, "VulkanTexture: Failed to create VkImageView...");
        imagePtr->_imageViews.push_back(imageView);

        if (imagePtr->_forDepthStencil)
        {
            imagePtr->SetImageLayout(i, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, false);
            imagePtr->_textureHandles.push_back(imagePtr->createTextureHandle(i, image, imageView));
        } else {
            /* Create Storage Image Handles For Compute Shaders */
            imagePtr->SetImageLayout(i, VK_IMAGE_LAYOUT_GENERAL, false);
            imagePtr->_storageHandles.push_back(imagePtr->createStorageHandle(i, image, imageView));
            imagePtr->SetImageLayout(i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, false);
        };
    };

    return imagePtr;
};

UniquePtr<VulkanImage> VulkanImage::CreateVulkanImage(const VulkanImageTextureInfo& textureInfo)
{
    /* Create VulkanImage */
    UniquePtr<VulkanImage> imagePtr = std::make_unique<VulkanImage>();
    
    imagePtr->_format = textureInfo.params.format;
    imagePtr->_height = textureInfo.params.height;
    imagePtr->_width = textureInfo.params.width;
    imagePtr->_count = 1;
    imagePtr->_pVulkanDevice = textureInfo.params.pVulkanDevice;
    imagePtr->_forTextureReading = true;
    
    imagePtr->_imageSize = textureInfo.params.height *
                           textureInfo.params.width *
                           textureInfo.channels;

    /* Create VkBuffer TODO: use VulkanBuffer instead */
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imagePtr->_imageSize;
    bufferInfo.usage = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocationInfo{};
    allocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VK_ASSERT(vmaCreateBuffer(textureInfo.params.pVulkanDevice->GetVmaAllocator(), &bufferInfo, &allocationInfo, &imagePtr->_buffer, &imagePtr->_bufferAllocation, nullptr),
                             VK_SUCCESS, "VulkanImage: Failed to Create VkBuffer For Copying Texture to...");

    /* Checks Format is Valid */
    std::vector<VkFormat> requestedFormats;
    requestedFormats.push_back(textureInfo.params.format);
    if (findFirstSupportedFormat(textureInfo.params.pVulkanDevice->GetVkPhysicalDevice(), requestedFormats) == VK_FORMAT_UNDEFINED)
    {
        auto error = "Couldn't Find a Supported Format for Requested Image\n";
        Logger::LogError(error);
    };

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = textureInfo.params.width;
    imageInfo.extent.height = textureInfo.params.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = textureInfo.params.format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                            VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    /* Expects Data is Ready for Copy */
    void* toPtr;
    vmaMapMemory(imagePtr->_pVulkanDevice->GetVmaAllocator(), imagePtr->_bufferAllocation, &toPtr);
    memcpy(toPtr, textureInfo.dataPtr, imagePtr->_imageSize);
    vmaUnmapMemory(imagePtr->_pVulkanDevice->GetVmaAllocator(), imagePtr->_bufferAllocation);

    /* Create VkImage */
    VkImage image;
    VmaAllocation imageAllocation;
    VmaAllocationCreateInfo imageAllocationInfo{};
    imageAllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VK_ASSERT(vmaCreateImage(imagePtr->_pVulkanDevice->GetVmaAllocator(), &imageInfo, &imageAllocationInfo, &image, &imageAllocation, nullptr),
                                VK_SUCCESS, "VulkanTexture: Failed to Create VkImage...");
    imagePtr->_images.push_back(image);
    imagePtr->_currentLayouts.push_back(VK_IMAGE_LAYOUT_UNDEFINED);
    imagePtr->_imageAllocation.push_back(imageAllocation);

    /* Copy Buffer To Image */
    imagePtr->SetImageLayout(0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, false);
    VulkanCommand::CopyBufferToImage(imagePtr->_pVulkanDevice, imagePtr->_buffer, imagePtr->GetImage(0), imagePtr->_width, imagePtr->_height);
    imagePtr->SetImageLayout(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, false);

    /* Delete Buffer */
    vmaDestroyBuffer(textureInfo.params.pVulkanDevice->GetVmaAllocator(), imagePtr->_buffer, imagePtr->_bufferAllocation);

    /* Create Image Views */
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = imagePtr->GetImage(0);
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = imagePtr->GetFormat();
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    VK_ASSERT(vkCreateImageView(imagePtr->_pVulkanDevice->GetVkDevice(), &viewInfo, nullptr, &imageView), VK_SUCCESS, "VulkanTexture: Failed to create VkImageView...");
    imagePtr->_imageViews.push_back(imageView);

    auto handle = imagePtr->createTextureHandle(0, imagePtr->GetImage(0), imagePtr->GetImageView(0));
    imagePtr->_textureHandles.push_back(handle);

    return imagePtr;
};

UniquePtr<VulkanImage> VulkanImage::CreateVulkanImage(const VulkanImageSwapChainInfo& ingestInfo)
{
    UniquePtr<VulkanImage> imagePtr = std::make_unique<VulkanImage>();
    imagePtr->_height = ingestInfo.params.height;
    imagePtr->_width = ingestInfo.params.width;
    imagePtr->_format = ingestInfo.params.format;
    imagePtr->_currentLayouts.assign(ingestInfo.count, ingestInfo.params.layout);
    imagePtr->_images = ingestInfo.images;
    imagePtr->_imageViews = ingestInfo.imageViews;
    imagePtr->_pVulkanDevice = ingestInfo.params.pVulkanDevice;

    return imagePtr;
};

VkFormat VulkanImage::toVkFormat(VulkanDevice* device, FrameBufferPixelFormat format)
{
    switch (format)
    {
        case FrameBufferPixelFormat::COLOR_RGBA_8888:
            return VK_FORMAT_R8G8B8A8_UNORM;
            
        case FrameBufferPixelFormat::COLOR_RGBA_16161616_sFloat:
            return VK_FORMAT_R16G16B16A16_SFLOAT;

        case FrameBufferPixelFormat::DEPTH_STENCIL_D32_S8:
            std::vector<VkFormat> depthStencilFormats;
            depthStencilFormats.push_back(VK_FORMAT_D32_SFLOAT_S8_UINT);
            depthStencilFormats.push_back(VK_FORMAT_D24_UNORM_S8_UINT);
            depthStencilFormats.push_back(VK_FORMAT_D16_UNORM_S8_UINT);
            return findFirstSupportedFormat(device->GetVkPhysicalDevice(), depthStencilFormats);
    };
};

TextureHandle VulkanImage::createTextureHandle(uint32_t index, VkImage image, VkImageView view)
{
    TextureHandle handle = _pVulkanDevice->CreateTextureHandle();

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = _currentLayouts.at(index);
    imageInfo.imageView = _imageViews.at(index);
    imageInfo.sampler = _pVulkanDevice->GetGlobalVkSampler();

    VkWriteDescriptorSet writeInfo{};
    writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.dstSet = _pVulkanDevice->GetBindlessDescriptorSet();
    writeInfo.dstBinding = TEXTURE_BINDING_POINT;
    writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeInfo.descriptorCount = 1;
    writeInfo.dstArrayElement = handle;
    writeInfo.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(_pVulkanDevice->GetVkDevice(), 1, &writeInfo, 0, nullptr);
    return handle;
};  

TextureHandle VulkanImage::createStorageHandle(uint32_t index, VkImage image, VkImageView view)
{
    TextureHandle handle = _pVulkanDevice->CreateStorageBufferHandle();
    
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = _currentLayouts.at(index);
    imageInfo.imageView = view;
    imageInfo.sampler = _pVulkanDevice->GetGlobalVkSampler();

    VkWriteDescriptorSet writeInfo{};
    writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.dstSet = _pVulkanDevice->GetBindlessDescriptorSet();
    writeInfo.dstBinding = STORAGE_IMAGE_BINDING_POINT;
    writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeInfo.descriptorCount = 1;
    writeInfo.dstArrayElement = handle;
    writeInfo.pImageInfo = &imageInfo;
    
    vkUpdateDescriptorSets(_pVulkanDevice->GetVkDevice(), 1, &writeInfo, 0, nullptr);
    return handle;
};

void VulkanImage::SetImageLayout(uint32_t index, VkImageLayout targetLayout, bool inRenderingState)
{
    VkCommandBuffer commandBuffer;
    VkSemaphore currentTask;
    VkSemaphore thisTask;
    VulkanTimeline* timeline;

    bool skip = false;
    
    VkImageLayout currentLayout = _currentLayouts.at(index);
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = currentLayout;
    barrier.newLayout = targetLayout;
    barrier.image = _images.at(index);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    VkImageAspectFlags flags;
    if (_forDepthStencil)
    {
        flags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    } else {
        flags = VK_IMAGE_ASPECT_COLOR_BIT;
    };

    barrier.subresourceRange.aspectMask = flags;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    /* TODO: Really need to sort out this function. Can't be relaying on a massive IF ELSE
    block to know how to move to next image layout. Instead we can use the TimelineState to hold the last known
    state of the pipeline and use that to set the correct source and destination stage of the pipeline. */
    if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && targetLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && targetLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && targetLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && targetLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_GENERAL && targetLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && targetLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT ;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_GENERAL && targetLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_GENERAL && targetLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && targetLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && targetLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_GENERAL && targetLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && targetLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else if (currentLayout == targetLayout)
    {   
        if (DEBUG_IMAGE_LAYOUT)
            Logger::LogWarn("SetImageLayout was called but wasn't needed...");
        skip = true;
    }
    else
    {  
        Logger::LogError("VulkanImage: Selected TargetLayout isn't Supported.");
        throw std::runtime_error("Exit...");
    }

    if (!skip)
    {
        if(inRenderingState)
        {
            /* Needs to waiting on other jobs to finished before
            switching the image layout ..*/
            commandBuffer = _pVulkanDevice->GetCurrentCommandBuffer(index);
            timeline = _pVulkanDevice->GetTimeline(index);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;
            VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), VK_SUCCESS, "Begin Command Buffer - VulkanImage::SetImageLayout");
        }
        else
        {
            /* The image is currently not be using in any task and can freely
            switch to different image layouts */
            commandBuffer = VulkanCommand::beginSingleTimeCommandBuffer(_pVulkanDevice);
        };

        /* Change Image Layout Command */
        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrier);

        if (inRenderingState)
        {
            VK_ASSERT(vkEndCommandBuffer(commandBuffer), VK_SUCCESS, "End Command Buffer - VulkanImage::SetImageLayout");

            /* Sumbit with Timeline */
            VulkanCommand::submitCommandBuffer(_pVulkanDevice, timeline, commandBuffer);

            /* Index to Next Command Buffer */
            _pVulkanDevice->GetNextCommandBuffer(index);
        }
        else
        {
            VulkanCommand::endSingleTimeCommandBuffer(_pVulkanDevice, commandBuffer);
        }
    };
    _currentLayouts[index] = targetLayout;
};

VkFormat VulkanImage::findFirstSupportedFormat(VkPhysicalDevice device, std::vector<VkFormat> &requestedFormats)
{
    for (auto format : requestedFormats)
    {
        VkFormatProperties2 properties{.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
        vkGetPhysicalDeviceFormatProperties2(device, format, &properties);
        auto bufferFlags = properties.formatProperties.bufferFeatures;
        auto linearFlags = properties.formatProperties.linearTilingFeatures;
        auto optimalFlags = properties.formatProperties.optimalTilingFeatures;
        if (bufferFlags != 0 || linearFlags != 0 || optimalFlags != 0)
        {
            return format;
        };
    };

    return VK_FORMAT_UNDEFINED;
};

TextureHandle VulkanImage::GetStorageImageHandle(uint32_t index)
{
    return _storageHandles.at(index);
};

TextureHandle VulkanImage::GetTextureHandle(uint32_t index)
{
    return _textureHandles.at(index);
};

VkImage VulkanImage::GetImage(uint32_t index)
{
    return _images.at(index);
};

VkImageView VulkanImage::GetImageView(uint32_t index)
{
    return _imageViews.at(index);
};
VkFormat VulkanImage::GetFormat()
{
    return _format;
};

VkImageLayout VulkanImage::GetImageLayout(uint32_t index)
{
    return _currentLayouts.at(index);
};

};