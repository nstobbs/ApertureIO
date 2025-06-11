#include "ApertureIO/VulkanFrameBuffer.hpp"
#include "ApertureIO/Logger.hpp"

#include <string>

namespace Aio {

VulkanFrameBuffer::VulkanFrameBuffer(Device* device, Context* context)
{
    Logger::LogInfo("Created VulkanFrameBuffer");
    // create with swap chain
    isSwapChainTarget = true;
    _pDevice = dynamic_cast<VulkanDevice*>(device);
    _pContext = dynamic_cast<VulkanContext*>(context);

    vkb::SwapchainBuilder swapchainBuilder { _pDevice->_device };
    
    /* Compute Shader Access */
    //swapchainBuilder.add_image_usage_flags(VK_IMAGE_USAGE_STORAGE_BIT); // Write Access
    //swapchainBuilder.add_image_usage_flags(VK_IMAGE_USAGE_SAMPLED_BIT); // Read Access

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

    _width = _extent.width;
    _height = _extent.height;

    // framebuffers for swapchains must have the swapchain format at index 0.
    _formats.push_back(swapchain.image_format);
    std::string name = "Colour_Output";
    _layers.insert(std::make_pair(name, COLOR_RGBA_8888));
    _layerCount++;
    /* Image Layout when using as a swapchain.
    the image layout in arrays will be 
    (as if framesInFlight was 2)
    {layer1}  * n {layer2} {layer2} {layer3} {layer3}  
    else we are using image layout as 
    {layer1} {layer2} {layer3} ...*/

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

/* Do I even Need this?*/
bool VulkanFrameBuffer::init()
{
    return true;
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
                layerRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
    auto result = vkCreateRenderPass(_pDevice->GetVkDevice(), &renderPassInfo, nullptr, &renderPass);
    if(result != VK_SUCCESS)
    {
        std::cout << "Failed to renderpass :(\n";
    }

    return renderPass;
};
// TODO: Come back and see if you can make these loops better 
std::vector<VkFramebuffer> VulkanFrameBuffer::CreateVkFramebuffers()
{
    VkDevice device = _pDevice->GetVkDevice();
    std::vector<VkFramebuffer> framebuffers;
    if (isSwapChainTarget)
    {
        // create for each frameInFlight;
        auto numInFlight = _pContext->getMaxFramesInFlight();
        for (uint32_t i = 0; i < numInFlight; i++)
        {
            std::vector<VkImageView> layerAttachments;
            for (int layer = 0; layer < static_cast<int>(_layerCount); layer++)
            {
                auto index = i + layer;
                layerAttachments.push_back(_imageViews[index]);
            };

            auto framebuffer = CreateVkFramebuffer(layerAttachments, _layerCount);
            framebuffers.push_back(framebuffer);
        };

    } else {
        // create one framebuffer
        // TODO: this should be in a functions and not repeated!
        std::vector<VkImageView> layerAttachments;
        for (uint32_t layer = 0; layer < _layerCount; layer++)
        {
            layerAttachments.push_back(_imageViews[layer]);
        };

        auto framebuffer = CreateVkFramebuffer(layerAttachments, _layerCount);
        framebuffers.push_back(framebuffer);
    }

    return framebuffers;
};

VkFramebuffer VulkanFrameBuffer::CreateVkFramebuffer(std::vector<VkImageView> layerAttachments, uint32_t layerCount)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = _renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(layerCount);
    framebufferInfo.pAttachments = layerAttachments.data();
    framebufferInfo.width = _extent.width;
    framebufferInfo.height = _extent.height;
    framebufferInfo.layers = 1; // TODO Double check this and fix it.

    VkFramebuffer framebuffer;
    if(vkCreateFramebuffer(_pDevice->GetVkDevice(), &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Create FrameBuffer!");
    };
    
    return framebuffer;
};

void VulkanFrameBuffer::Bind(RenderContext& renderContext)
{
    renderContext._TargetFrameBuffer = dynamic_cast<FrameBuffer*>(this);
};

void VulkanFrameBuffer::Unbind()
{

};

VkSwapchainKHR VulkanFrameBuffer::GetSwapChain()
{
    return _swapchain;
};

VkRenderPass VulkanFrameBuffer::GetRenderPass()
{
    return _renderPass;
};

VkExtent2D VulkanFrameBuffer::GetExtent()
{
    return _extent;
};

VkFramebuffer VulkanFrameBuffer::GetIndexFramebuffer(uint32_t imageIndex)
{
    return _framebuffers[imageIndex];
};

} // End Aio namespace