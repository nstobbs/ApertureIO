#pragma once
#include "ApertureIO/VulkanCommon.hpp"

#include <vector>

namespace Aio
{

class VulkanImage
{
public:
    static VulkanImage* CreateForFrameBuffer();
    static VulkanImage* CreateForTexture();
    
    void SetImageLayout();
    VkImageLayout GetImageLayout();

private:
    std::vector<VkImage> _images;
    std::vector<VkImageView> _imageViews;
    std::vector<VkFormat> _formats;
    VkImageLayout _imageLayout;
};

};