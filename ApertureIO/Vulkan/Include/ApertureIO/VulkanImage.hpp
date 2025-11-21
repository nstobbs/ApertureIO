#pragma once
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"

#include <vector>

namespace Aio
{

struct VulkanImageCommonParameter
{
    VulkanDevice* pVulkanDevice;

    uint32_t height;
    uint32_t width;

    VkFormat format;
    VkImageLayout layout;
};

struct VulkanImageCreateInfo
{
    VulkanImageCommonParameter params;
    uint32_t count;
};

struct VulkanImageSwapChainInfo
{
    VulkanImageCommonParameter params;
    uint32_t count;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};

struct VulkanImageTextureInfo
{
    VulkanImageCommonParameter params;
    uint32_t channels; // channels = RGBA = 4
    void* dataPtr = {nullptr};
};

/* Notes: VulkanImage can be stored and accessed two different ways.
As a Storage Image Buffer or as a Combined Texture With Sampler.
Since not all of the different VkFormats support Write and Read
access.*/

class VulkanImage
{
public:
    static UniquePtr<VulkanImage> CreateVulkanImage(const VulkanImageCreateInfo& createInfo); /* Create Images for Rendering */
    static UniquePtr<VulkanImage> CreateVulkanImage(const VulkanImageSwapChainInfo& ingestInfo); /* Create Images for "Present" */
    static UniquePtr<VulkanImage> CreateVulkanImage(const VulkanImageTextureInfo& textureInfo); /* Create Images for Reading from Disk */
    static VkFormat toVkFormat(VulkanDevice* device, FrameBufferPixelFormat format);

    TextureHandle GetStorageImageHandle(uint32_t index);
    TextureHandle GetTextureHandle(uint32_t index);

    void SetImageLayout(uint32_t index, VkImageLayout targetLayout, bool inRenderingState);

    VkImage GetImage(uint32_t index);
    VkImageView GetImageView(uint32_t index);
    VkImageLayout GetImageLayout(uint32_t index);
    VkFormat GetFormat();

private:
    static VkFormat findFirstSupportedFormat(VkPhysicalDevice device, std::vector<VkFormat> &requestedFormats);
    TextureHandle createStorageHandle(uint32_t index, VkImage image, VkImageView view);
    TextureHandle createTextureHandle(uint32_t index, VkImage image, VkImageView view);

    VulkanDevice* _pVulkanDevice;

    bool _forDepthStencil = {false};
    bool _forTextureReading = {false};

    uint32_t _height;
    uint32_t _width;
    uint32_t _count;
    VkFormat _format;

    /* For Copying Texture From the CPU to GPU */
    VkDeviceSize _imageSize;
    VkBuffer _buffer;
    VmaAllocation _bufferAllocation;

    std::vector<VkImage> _images;
    std::vector<VkImageView> _imageViews;
    std::vector<VkImageLayout> _currentLayouts;
    std::vector<VmaAllocation> _imageAllocation;

    std::vector<TextureHandle> _storageHandles;
    std::vector<TextureHandle> _textureHandles;
};

};