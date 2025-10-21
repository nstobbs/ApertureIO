#pragma once
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"

#include <vector>

namespace Aio
{

struct VulkanImageCreateInfo
{
    VulkanDevice* pVulkanDevice;
    uint32_t height;
    uint32_t width;
    uint32_t count;
    VkFormat format;
    VkImageLayout layout;
};

struct VulkanImageSwapChainInfo
{
    VulkanDevice* pVulkanDevice;
    uint32_t height;
    uint32_t width;
    uint32_t count;
    VkFormat format;
    VkImageLayout layout;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};

/* Notes: VulkanImage can be stored and accessed two different ways.
As a Storage Image Buffer or as a Combined Texture With Sampler.
Since not all of the different VkFormats support Write and Read
access.*/

class VulkanImage
{
public:
    static UniquePtr<VulkanImage> CreateVulkanImage(const VulkanImageCreateInfo& createInfo);
    static UniquePtr<VulkanImage> CreateVulkanImage(const VulkanImageSwapChainInfo& ingestInfo);
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

    std::vector<VkImage> _images;
    std::vector<VkImageView> _imageViews;
    std::vector<VkImageLayout> _currentLayouts;

    std::vector<TextureHandle> _storageHandles;
    std::vector<TextureHandle> _textureHandles;
};

};