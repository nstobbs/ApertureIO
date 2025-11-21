#pragma once

#include "ApertureIO/Texture.hpp"
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanContext.hpp"
#include "ApertureIO/VulkanImage.hpp"

namespace Aio
{

class VulkanTexture : public Texture
{
public:
    VulkanTexture(const TextureCreateInfo& createInfo);

    TextureHandle GetTextureHandle() override;
    void Bind(RenderContext& rContext) override;
    void Unbind() override;

    void sourceFileModified() override;

private:
    VulkanDevice* _pDevice;
    VulkanContext* _pContext;
    
    UniquePtr<VulkanImage> _image;
};

};