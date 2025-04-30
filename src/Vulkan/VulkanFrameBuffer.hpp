#ifndef H_VULAKNFRAMEBUFFER
#define H_VULAKNFRAMEBUFFER

#include "../Base/FrameBuffer.hpp"
#include "../Base/Context.hpp"
#include "../Base/Device.hpp"

#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"

namespace ApertureIO {

class VulkanFrameBuffer : public FrameBuffer
{
    public:
    VulkanFrameBuffer(Device* device, Context* context);
    VulkanFrameBuffer(Device* device);

    bool init() override;

    void Bind() override;
    void Unbind() override;

    private:
    /* Private Functions */
    VkRenderPass CreateVkRenderPass();
    std::vector<VkFramebuffer> CreateVkFramebuffers();
    std::vector<VkFramebuffer> RebuildVkFramebuffers();

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
}

#endif