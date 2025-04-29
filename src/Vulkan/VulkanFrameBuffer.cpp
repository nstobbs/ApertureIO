#include "VulkanFrameBuffer.hpp"

namespace ApertureIO {

VulkanFrameBuffer::VulkanFrameBuffer(Device* device, Context* context)
{
    // create with swap chain
    _pDevice = dynamic_cast<VulkanDevice*>(device);
    _pContext = dynamic_cast<VulkanContext*>(context);

    vkb::SwapchainBuilder swapchainBuilder { _pDevice->_device };
    auto swapchainResult = swapchainBuilder.build();
    if (!swapchainResult)
    {
        std::cout << swapchainResult.error().message() << "\n";
    }

    vkb::Swapchain swapchain = swapchainResult.value();

    _images = swapchain.get_images().value();
    _imageViews = swapchain.get_image_views().value();
    _swapchain = swapchain.swapchain;
    _extent = swapchain.extent;
    // framebuffers for swapchains must have the swapchain format at index 0.
    _formats.push_back(swapchain.image_format); 
    _layers.emplace("Color_Output", COLOR_RGBA_8888);

    /* Create the RenderPass */
};


VulkanFrameBuffer::VulkanFrameBuffer(Device* device)
{
    // create without swap chain
    _pDevice = dynamic_cast<VulkanDevice*>(device);
};

bool VulkanFrameBuffer::init()
{
    std::cout << ":0\n";
    return false;
};

VkRenderPass VulkanFrameBuffer::CreateRenderPass()
{
    /*
    TODO: Check how we want to setup LoadOp and StoreOps 
    differently depending of the usage of the framebuffer.
    I'm sure I'm not going to be doing all of my rendering
    within one command buffer. 
    */

    std::vector<VkAttachmentDescription> descriptions;
    std::vector<VkAttachmentReference> references;

    /* We Need to check each _layers and create an 
    attachment for each layer.  */
    uint32_t layerCount = 0;
    for (auto layer : _layers)
    {
        VkAttachmentDescription layerInfo{};
        VkAttachmentReference layerRef{};
        switch(layer.second)
        {   
            case COLOR_RGBA_8888:
                layerInfo.format = _formats[layerCount];
                layerInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                
                layerInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                layerInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                layerInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                layerInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

                layerInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                layerInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                layerRef.attachment = layerCount;
                break;
            
            case DEPTH_D32_S8:
                // TODO Add Depth-Testing
        };

        descriptions.push_back(layerInfo);
        references.push_back(layerRef);
        layerCount++;
    }
};
} // End ApertureIO namespace