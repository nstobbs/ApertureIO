#ifndef H_VULKANSHADER
#define H_VULKANSHADER

#include "VulkanDevice.hpp"
#include "VulkanCommon.hpp"

#include "../Base/FrameBuffer.hpp"
#include "../Base/RenderContext.hpp"
#include "../Base/Shader.hpp"

#include "../Common/FileIO.hpp"

#include <vector>

namespace Aio {

class VulkanShader : public Shader
{
    public:
    VulkanShader(ShaderCreateInfo& createInfo);

    ~VulkanShader();

    void Bind(RenderContext& renderContext) override;
    void Unbind() override;

    void rebuildShader() override; //TODO: Needs the RenderContext

    void SetVec4(std::string name, glm::vec4 value) override;

    VkViewport GetViewport();
    VkRect2D GetScissor();
    VkPipeline GetPipeline();
    VkPipelineLayout GetPipelineLayout();

    private:
    ShaderType _type;

    // Shader Components
    VkViewport _viewport;
    VkRect2D _scissor;
    VkShaderModule _vertModule;
    VkShaderModule _fragModule;
    
    std::vector<VkDynamicState> _dynamicStates;

    /* Store all of the createInfos so that we can
    rebuild the shader if the source file has been modified or 
    if the renderContext has a different hash meaning that the bound
    objects have changed.*/
    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;

    VkPipelineInputAssemblyStateCreateInfo _inputAssemblyInfo{};
    VkPipelineDynamicStateCreateInfo _dynamicStateInfo{};
    VkPipelineViewportStateCreateInfo _viewportStateInfo{};
    VkPipelineRasterizationStateCreateInfo _rasterizerInfo{};
    VkPipelineMultisampleStateCreateInfo _multiSamplingInfo{};
    VkPipelineColorBlendAttachmentState _colorBlendAttachmentInfo{};
    VkPipelineColorBlendStateCreateInfo _colorBlendingInfo{};
    VkPipelineDepthStencilStateCreateInfo _depthStencilInfo{};
    VkGraphicsPipelineCreateInfo _pipelineInfo{};

    VkPipeline _pipeline;
    VkPipelineLayout _layout;

    VulkanDevice* _pDevice;
    VulkanContext* _pContext;

    VkPipeline createPipeline(RenderContext& renderContext);
    VkShaderModule createShaderModule(std::vector<uint32_t>& code);
    std::vector<uint32_t> compileShaderSource(std::string& code, SourceFileType type);

    //TODO: if we can have an vector of RenderContexts then doesnt that mean we should have an vector of VkPipeline for each different RenderContext???
    RenderContext* _bound; // I don't really like this idea. But seems needed for hot-reloading...
    bool _alreadyRebuilding = {false};

    size_t _previousHash;
};

}; // End of Aio namespace

#endif