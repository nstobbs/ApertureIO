#include "ApertureIO/VulkanFrameBuffer.hpp"
#include "ApertureIO/Logger.hpp"

#include <boost/functional/hash.hpp>
#include <string>

namespace Aio {

VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferCreateInfo& createInfo)
{
    Logger::LogInfo("Created VulkanFrameBuffer");

    _name = createInfo.name;
    isSwapChainTarget = createInfo.isSwapChain;
    _pDevice = dynamic_cast<VulkanDevice*>(createInfo.pDevice);
    _pContext = dynamic_cast<VulkanContext*>(createInfo.pContext);

    /* Create the SwapChain*/
    if (isSwapChainTarget)
    {
        vkb::SwapchainBuilder swapchainBuilder { _pDevice->_device };
        swapchainBuilder.add_image_usage_flags(VK_IMAGE_USAGE_SAMPLED_BIT);
        swapchainBuilder.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        swapchainBuilder.set_desired_min_image_count(_pContext->getMaxFramesInFlight());
        auto swapchainResult = swapchainBuilder.build();
        if (!swapchainResult)
        {
            auto msg = "VulkanFrameBuffer: " + _name + " Failed to Create VkSwapChain...";
            Logger::LogWarn(msg);
            Logger::LogError(swapchainResult.error().message());
        }

        _bootstrapSwapchain = swapchainResult.value();

        _swapchain = _bootstrapSwapchain.swapchain;
        _extent = _bootstrapSwapchain.extent;

        _width = _extent.width;
        _height = _extent.height;

        VulkanImageSwapChainInfo swapChainInfo{};
        swapChainInfo.images = _bootstrapSwapchain.get_images().value();
        swapChainInfo.imageViews = _bootstrapSwapchain.get_image_views().value();
        swapChainInfo.width = _width;
        swapChainInfo.height = _height;
        swapChainInfo.format = _bootstrapSwapchain.image_format;
        swapChainInfo.count = swapChainInfo.images.size();
        swapChainInfo.pVulkanDevice = _pDevice;
        
        /* SwapChain Attachment For Presenting to Screen */
        std::string name = "Viewer";
        _layersMap.insert(std::make_pair(name, FrameBufferPixelFormat::COLOR_RGBA_8888));
        _layerOrder.push_back(name);
        _vulkanImagesMap.emplace(name, VulkanImage::CreateVulkanImage(swapChainInfo));
        _layerCount++;
    }
    else
    {
        _height = createInfo.height;
        _width = createInfo.width;

        VkExtent2D extent{};
        extent.height = _height;
        extent.width = _width;
        _extent = extent;
    };
};

void VulkanFrameBuffer::createVulkanImages()
{
    for (auto layer : _layersMap)
    {
        if (layer.first != "Viewer")
        {
            VulkanImageCreateInfo createInfo{};
            createInfo.pVulkanDevice = _pDevice;
            createInfo.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            createInfo.width = _width;
            createInfo.height = _height;
            createInfo.count = _pContext->getMaxFramesInFlight();

            // TODO: this should be a function that returns the VK_FORMAT
            switch (layer.second)
            {
                case FrameBufferPixelFormat::COLOR_RGBA_8888:
                    createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
                    break;
                case FrameBufferPixelFormat::COLOR_RGBA_16161616_sFloat:
                    createInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
                    break;
                case FrameBufferPixelFormat::DEPTH_D32_S8:
                    //FIXME
                    break;
            };
            _vulkanImagesMap.emplace(layer.first, std::move(VulkanImage::CreateVulkanImage(createInfo)));
        };
    };
};

VkRenderPass VulkanFrameBuffer::CreateVkRenderPass()
{
    /*
    TODO: Check how we want to setup LoadOp and StoreOps 
    differently depending of the usage of the framebuffer.
    I'm sure I'm not going to be doing all of my rendering
    within one command buffer. 
    */

    std::vector<VkAttachmentDescription> colorDescriptions;
    VkAttachmentReference* depthReferences = nullptr;
    std::vector<VkAttachmentReference> references;
    
    uint32_t layerID = 0;
    for (auto layerName : _layerOrder)
    {   
        FrameBufferPixelFormat pixelFormat = _layersMap.at(layerName);
        VkAttachmentDescription attachmentDescription{};
        VkAttachmentReference attachmentReference{};

        switch(pixelFormat)
        {   
            case FrameBufferPixelFormat::COLOR_RGBA_8888:
                attachmentDescription.format = VK_FORMAT_R8G8B8A8_UNORM;
                attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
                if (layerName == "Viewer")
                    attachmentDescription.format = _bootstrapSwapchain.image_format;

                attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

                attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                colorDescriptions.push_back(attachmentDescription);

                attachmentReference.attachment = layerID;
                attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                break;
            
            case FrameBufferPixelFormat::COLOR_RGBA_16161616_sFloat:
                /* For Use with GBuffer for Normals */
                attachmentDescription.format = VK_FORMAT_R16G16B16A16_SFLOAT;
                attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

                attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

                attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                colorDescriptions.push_back(attachmentDescription);
                
                attachmentReference.attachment = layerID;
                attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                break;
            
            case FrameBufferPixelFormat::DEPTH_D32_S8:
                attachmentDescription. format = VK_FORMAT_D32_SFLOAT; // TODO: Create an Function to Return the Current Depth Format for this Device
                attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
                
                attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

                attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

                attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                attachmentReference.attachment = 1;
                attachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depthReferences = &attachmentReference;
                break;
        }
        references.push_back(attachmentReference);
        layerID++;
    }

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; //TODO double check what this means...
    subpass.colorAttachmentCount = layerID;
    subpass.pColorAttachments = references.data();
    subpass.pDepthStencilAttachment = depthReferences;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(colorDescriptions.size());
    renderPassInfo.pAttachments = colorDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    VkRenderPass renderPass;
    auto error = "VulkanFrameBuffer: " + _name + " Failed to Create VkRenderPass...";
    VK_ASSERT(vkCreateRenderPass(_pDevice->GetVkDevice(), &renderPassInfo, nullptr, &renderPass), VK_SUCCESS, error);

    auto msg = "VulkanFrameBuffer: " + _name + " Created a VkRenderPass.";
    Logger::LogInfo(msg);
    return renderPass;
};
 
std::vector<VkFramebuffer> VulkanFrameBuffer::CreateVkFramebuffers()
{
    auto numInFlight = _pContext->getMaxFramesInFlight();
    std::vector<VkFramebuffer> framebuffers;
    framebuffers.resize(numInFlight);
    
    for (int frame = 0; frame < numInFlight; frame++)
    {
        std::vector<VkImageView> attachmentImageViews;
        for (auto layerName : _layerOrder)
        {
            attachmentImageViews.push_back(_vulkanImagesMap.at(layerName)->GetImageView(frame));
        };
        framebuffers[frame] = CreateVkFramebuffer(attachmentImageViews, _layerCount); 
    };

    return framebuffers;
};

VkFramebuffer VulkanFrameBuffer::CreateVkFramebuffer(std::vector<VkImageView> attachmentsImageViews, uint32_t attachmentCount)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = _renderPass;
    framebufferInfo.attachmentCount = attachmentCount;
    framebufferInfo.pAttachments = attachmentsImageViews.data();
    framebufferInfo.width = _extent.width;
    framebufferInfo.height = _extent.height;
    framebufferInfo.layers = 1;

    VkFramebuffer framebuffer;
    auto error = "VulkanFrameBuffer: " + _name + " Failed to create VkFramebuffer...";
    VK_ASSERT(vkCreateFramebuffer(_pDevice->GetVkDevice(), &framebufferInfo, nullptr, &framebuffer), VK_SUCCESS, error);

    auto msg = "VulkanFrameBuffer: " + _name + " Created a VkFrameBuffer.";
    Logger::LogInfo(msg);
    return framebuffer;
};

VulkanImage* VulkanFrameBuffer::GetLayerVulkanImage(const std::string& name)
{
    return _vulkanImagesMap.at(name).get();
};

void VulkanFrameBuffer::Rebuild()
{
    _requestedRebuild = true;
};

bool VulkanFrameBuffer::CheckRebuildInProgress()
{
    return _requestedRebuild;
};

 void VulkanFrameBuffer::buildRenderableObjects()
 {
       size_t currentHash = 0;
       boost::hash_combine(currentHash, _framebuffers);
       boost::hash_combine(currentHash, _renderPass);

       if (currentHash != _hash)
       {
            auto error = "VulkanFrameBuffer: " + _name + " Hash is Different. Creating new VulkanImage, VkRenderPass and VkFramebuffers.";
            Logger::LogWarn(error);
            //TODO: Add deletion
            /* Build the FrameBuffer */
            createVulkanImages();
            _renderPass = CreateVkRenderPass();
            _framebuffers = CreateVkFramebuffers();

            _hash = 0;
            boost::hash_combine(_hash, _framebuffers);
            boost::hash_combine(_hash, _renderPass);
       }
 };

void VulkanFrameBuffer::Bind(RenderContext& renderContext, bool isTarget)
{
    if (isTarget)
    {
        renderContext._TargetFrameBuffer = dynamic_cast<FrameBuffer*>(this);
        buildRenderableObjects();
        /* If we know this is going to be used inside of a compute shader
        then we should build it as a s*/
    }
    else
    {
         /* We can use the framebuffer as an sampled image within the shader
         as this is the fastest for read only.*/
        renderContext._FrameBuffers.push_back(dynamic_cast<FrameBuffer*>(this));
       
    }
    
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