#pragma once

#include "ApertureIO/VulkanContext.hpp"
#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanImage.hpp"

#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/Device.hpp"

namespace Aio {

class VulkanShader;

/* TODO: Worth looking into dynamic rendering and might remove the need for render passes
 https://docs.vulkan.org/samples/latest/samples/extensions/dynamic_rendering/README.html
 instead of having to create render passes we can instead just point to the image attachment
 and just render to that instead.*/
class VulkanFrameBuffer : public FrameBuffer
{
public:
    VulkanFrameBuffer(const FrameBufferCreateInfo& createInfo);

    void Bind(RenderContext& renderContext, bool isTarget) override;
    void Unbind() override;

    VkSwapchainKHR GetSwapChain();
    VkRenderPass GetRenderPass();
    VkExtent2D GetExtent();
    VkFramebuffer GetIndexFramebuffer(uint32_t imageIndex);
    VulkanImage* GetLayerVulkanImage(const std::string& name);
    std::vector<VkClearValue> GetClearValue();

    void Rebuild();

    bool CheckRebuildInProgress();

private:
    /* Private Functions */
    VkRenderPass CreateVkRenderPass();
    std::vector<VkFramebuffer> CreateVkFramebuffers();
    std::vector<VkFramebuffer> RebuildVkFramebuffers();
    void createVulkanImages();
    void buildRenderableObjects(); /* TODO: think of a better name than this! */
    void createStorageImageHandles();

    VkFramebuffer CreateVkFramebuffer(std::vector<VkImageView> layerAttachments, uint32_t layerCount);

    /* Private Data */
    VulkanDevice* _pDevice;
    VulkanContext* _pContext;

    // This data needs to be set before creating the renderPass
    /* Layers : VulkanImage */
    std::unordered_map<std::string, UniquePtr<VulkanImage>> _vulkanImagesMap;
    
    VkExtent2D _extent;

    // Swapchain Objects
    vkb::Swapchain _bootstrapSwapchain;
    VkSwapchainKHR _swapchain;

    std::vector<VkFramebuffer> _framebuffers;
    VkRenderPass _renderPass;
    size_t _hash;
    
    bool _requestedRebuild = {false};
}; 
};