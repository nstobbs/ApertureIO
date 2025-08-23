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

class VulkanImage
{
public:
    static UniquePtr<VulkanImage> CreateVulkanImage(const VulkanImageCreateInfo& createInfo);
    static UniquePtr<VulkanImage> CreateVulkanImage(const VulkanImageSwapChainInfo& ingestInfo);

    void SetImageLayout(VkImageLayout targetLayout);

    VkImage GetImage(uint32_t index);
    VkImageView GetImageView(uint32_t index);
    VkFormat GetFormat();
    VkImageLayout GetImageLayout();

private:
    VulkanDevice* _pVulkanDevice;

    uint32_t _height;
    uint32_t _width;
    uint32_t _count;
    std::vector<VkImage> _images;
    std::vector<VkImageView> _imageViews;
    VkFormat _format;
    VkImageLayout _currentLayout;
};

};