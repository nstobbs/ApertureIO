#include "VulkanFrameBuffer.hpp"

namespace ApertureIO {

VulkanFrameBuffer::VulkanFrameBuffer(Device* device, Context* context)
{
    // create with swap chain
    _pDevice = dynamic_cast<VulkanDevice*>(device);
    _pContext = dynamic_cast<VulkanContext*>(context);

    vkb::SwapchainBuilder swapchainBuilder { _pDevice->_device };
    swapchainBuilder.set_desired_min_image_count(context->getMaxFramesInFlight());
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
    char* name = "Colour_Output";
    _layers.insert(std::make_pair(name, COLOR_RGBA_8888));

    /* Create the RenderPass */
    _renderPass = CreateVkRenderPass();

    /* Create the FrameBuffer*/
    _framebuffers = CreateVkFramebuffers();
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

VkRenderPass VulkanFrameBuffer::CreateVkRenderPass()
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
        }

        descriptions.push_back(layerInfo);
        references.push_back(layerRef);
        layerCount++;
    }

    /*TODO: Comes back here and do a better handling of the subpass
    as this wont be dymanic at all.
    Subpasses seems like extra steps we can add to our framebuffer overall.
    Not sure if I really want to handle the framebuffer like that. So for now
    we could just have basic filling out the details but no subpasses for 
    actually rendering.
    */
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; //TODO double check what this means...
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &references[0]; //TODO fix / improve me...
    subpass.pDepthStencilAttachment = nullptr;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(descriptions.size());
    renderPassInfo.pAttachments = descriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    VkRenderPass renderPass;
    //TODO need to start using assert to test the results and shutdown if not like here!
    auto result = vkCreateRenderPass(_pDevice->getVkDevice(), &renderPassInfo, nullptr, &renderPass);
    if(!result)
    {
        std::cout << "Failed to renderpass :(\n";
    }

    return renderPass;
};

std::vector<VkFramebuffer> VulkanFrameBuffer::CreateVkFramebuffers()
{
    auto imageCount = _imageViews.size();
    std::vector<VkFramebuffer> framebuffers;

    for (size_t i = 0; i < imageCount; i++)
    {
        /* TODO we need to know the order of imageViews
        and what attachment should be connected to them.
        This means keeping better track of these data types
        before we start creating framebuffers with them.*/
    };
    return framebuffers;
};

void VulkanFrameBuffer::Bind()
{

};

void VulkanFrameBuffer::Unbind()
{

};

} // End ApertureIO namespace