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
    VulkanImageCommonParameter commonInfo = {.pVulkanDevice = _pDevice,
                                             .height = static_cast<uint32_t>(_height),
                                             .width = static_cast<uint32_t>(_width),
                                             .format = VK_FORMAT_R8G8B8A8_SRGB,
                                             .layout = VK_IMAGE_LAYOUT_UNDEFINED}; 
    VulkanImageTextureInfo textureInfo = {.params = commonInfo,
                                          .channels = static_cast<uint32_t>(_channels),
                                          .dataPtr = _pixels};

    _image = std::move(VulkanImage::CreateVulkanImage(textureInfo));
    _handle = _image->GetTextureHandle(0);
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