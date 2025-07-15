#pragma once

#include "ApertureIO/VulkanContext.hpp"
#include "ApertureIO/VulkanDevice.hpp"

#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/Device.hpp"

namespace Aio {
    class VulkanShader;
}

namespace Aio {

/* TODO: Worth looking into dynamic rendering and might remove the need for render passes
 https://docs.vulkan.org/samples/latest/samples/extensions/dynamic_rendering/README.html
 instead of having to create render passes we can instead just point to the image attachment
 and just render to that instead.*/
class VulkanFrameBuffer : public FrameBuffer
{
    public:
    VulkanFrameBuffer(Device* device, Context* context);
    VulkanFrameBuffer(Device* device);

    bool init() override;

    void Bind(RenderContext& renderContext) override;
    void Unbind() override;

    VkSwapchainKHR GetSwapChain();
    VkRenderPass GetRenderPass();
    VkExtent2D GetExtent();
    VkFramebuffer GetIndexFramebuffer(uint32_t imageIndex);

    private:

    friend class VulkanShader;
    /* Private Functions */
    VkRenderPass CreateVkRenderPass();

    std::vector<VkFramebuffer> CreateVkFramebuffers();
    std::vector<VkFramebuffer> RebuildVkFramebuffers();

    VkFramebuffer CreateVkFramebuffer(std::vector<VkImageView> layerAttachments, uint32_t layerCount);
    /* Private Data */
    VulkanDevice* _pDevice;
    VulkanContext* _pContext;

    //TODO we should know the sizes at construction time.
    //So this should be an fixed size array instead? 
    std::vector<VkFramebuffer> _framebuffers;
    VkRenderPass _renderPass;

    // This data needs to be set before creating the renderPass
    std::vector<VkImage> _images;
    std::vector<VkImageView> _imageViews;
    std::vector<VkFormat> _formats;
    VkExtent2D _extent;

    // if it is a swapchain store it.
    VkSwapchainKHR _swapchain;
}; 
};