#include "ApertureIO/VulkanImage.hpp"
#include "ApertureIO/VulkanCommand.hpp"

namespace Aio
{

UniquePtr<VulkanImage> VulkanImage::CreateVulkanImage(const VulkanImageCreateInfo& createInfo)
{
    /* Create VkImage */
    UniquePtr<VulkanImage> vulkanImagePtr = std::make_unique<VulkanImage>();
    auto ptr = vulkanImagePtr.get();
    ptr->_count = 1;
    ptr->_format = createInfo.format;
    ptr->_height = createInfo.height;
    ptr->_width = createInfo.width;
    ptr->_pVulkanDevice = createInfo.pVulkanDevice;

    auto device = ptr->_pVulkanDevice;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = createInfo.width;
    imageInfo.extent.height = createInfo.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = createInfo.format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    VmaAllocationCreateInfo imageAllocationInfo{};
    imageAllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

    //TODO: This should be in a loop in range of count
    for (int i = 0; i < createInfo.count; i++)
    {
        VmaAllocation imageAllocation; //TODO: store this on the class......
        VkImage image;

        VK_ASSERT(vmaCreateImage(device->GetVmaAllocator(), &imageInfo, &imageAllocationInfo, &image, &imageAllocation, nullptr),
                                VK_SUCCESS, "VulkanTexture: Failed to Create VkImage...");

        ptr->_images.push_back(image);
        
         /* Create the VkImageViews */
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = createInfo.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;   

        VkImageView imageView;
        VK_ASSERT(vkCreateImageView(createInfo.pVulkanDevice->GetVkDevice(), &viewInfo, nullptr, &imageView), VK_SUCCESS, "VulkanTexture: Failed to create VkImageView...");
        ptr->_imageViews.push_back(imageView);
    };
    // Change the Image Layout, For Copying To..
    vulkanImagePtr->SetImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    return vulkanImagePtr;
};

UniquePtr<VulkanImage> VulkanImage::CreateVulkanImage(const VulkanImageSwapChainInfo& ingestInfo)
{
    UniquePtr<VulkanImage> vulkanImagePtr = std::make_unique<VulkanImage>();
    auto ptr = vulkanImagePtr.get();
    ptr->_height = ingestInfo.height;
    ptr->_width = ingestInfo.width;
    ptr->_format = ingestInfo.format;
    ptr->_currentLayout = ingestInfo.layout;
    ptr->_images = ingestInfo.images;
    ptr->_imageViews = ingestInfo.imageViews;
    ptr->_pVulkanDevice = ingestInfo.pVulkanDevice;

    return vulkanImagePtr;
};


void VulkanImage::SetImageLayout(VkImageLayout targetLayout)
{
    /* TODO: Test if this will work fine within RenderGraph...*/
    for (auto image : _images)
    {
        VkCommandBuffer commandBuffer = VulkanCommand::beginSingleTimeCommandBuffer(_pVulkanDevice);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = _currentLayout;
        barrier.newLayout = targetLayout;
        barrier.image = image;
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
        VulkanCommand::endSingleTimeCommandBuffer(_pVulkanDevice, commandBuffer);
    };
    
    _currentLayout = targetLayout; 
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

VkImageLayout VulkanImage::GetImageLayout()
{
    return _currentLayout;
};

};