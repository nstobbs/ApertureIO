#include "ApertureIO/Texture.hpp"
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanContext.hpp"


namespace Aio
{

class VulkanTexture : public Texture
{
public:
    VulkanTexture(TextureCreateInfo createInfo);

    TextureHandle* GetTextureHandle() override;
    void Bind(RenderContext& rContext) override;
    void Unbind() override;

    void sourceFileModified() override;

    void setImageLayout(VkImageLayout targetLayout);

private:
    TextureHandle storeTextureHandle(); //TODO: Maybe this store texture handles should be virtual in texture class and these becomes override funcs instead ?

    VulkanDevice* _pDevice;
    VulkanContext* _pContext;
    
    VkImage _image;
    VkImageView _imageView;
    VkImageLayout _currentLayout;
    VkFormat _format;
};

};