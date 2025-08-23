#define STB_IMAGE_IMPLEMENTATION

#include "ApertureIO/Texture.hpp"
#include "ApertureIO/VulkanTexture.hpp"

namespace Aio 
{

UniquePtr<Texture> Texture::CreateTexture(const TextureCreateInfo& createInfo)
{
    switch(createInfo.pContext->getRendererAPI())
    {
        case RendererAPI::eVulkan:
            return std::make_unique<VulkanTexture>(createInfo);
    }
};

void Texture::readTextureSourceFile()
{
    _pixels = stbi_load(_filepath.c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);
    if (!_pixels)
    {
        auto msg = "Aio::Texture Failed to read Texture Source File: " +  _filepath;
        Logger::LogError(msg);
        throw std::runtime_error("Exit...");
    }
};

void Texture::freePixels()
{
    stbi_image_free(_pixels);
};

};