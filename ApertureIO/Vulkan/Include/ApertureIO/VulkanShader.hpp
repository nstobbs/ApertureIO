#pragma once

#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanCommon.hpp"
#include "ApertureIO/VulkanBuffers.hpp"

#include "ApertureIO/FrameBuffer.hpp"
#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Shader.hpp"
#include "ApertureIO/Handles.hpp"

#include "ApertureIO/FileIO.hpp"

#include <vector>
#include <algorithm>

namespace Aio {

//TODO: Don't know why this is in shader...?
struct HandlesPushConstant{
    BufferHandle bufferHandles[16];
    TextureHandle textureHandles[16];
};

class VulkanShader : public Shader
{
public:
    VulkanShader(const ShaderCreateInfo& createInfo);

    ~VulkanShader();
    VulkanShader& operator=(VulkanShader&& otherShader) noexcept;

    void Bind(RenderContext& renderContext) override;
    void Unbind() override;

    void sourceFileModified() override;

    void SetVec4(std::string name, glm::vec4 value) override;
    void SetFloat(std::string name, float value) override;

    VkViewport GetViewport();
    VkRect2D GetScissor();
    VkPipeline GetPipeline();
    VkPipelineLayout GetPipelineLayout();

private:
    ShaderType _type;

    // TODO: Check if this is needed?
    // Uniform Buffer Stuff
    BufferLayout _uniformBufferLayout;
    VulkanBuffer* _uniformBuffer;

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

    /* TODO: */
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

// FileIncluder Class For Shaderc to be able to compile shaders with headers files
// 
class FileIncluder : public shaderc::CompileOptions::IncluderInterface
{
public:
    shaderc_include_result* GetInclude(const char* requested_source,
                                               shaderc_include_type type,
                                               const char* requesting_source,
                                               size_t include_depth) override;
    void ReleaseInclude(shaderc_include_result* data) override;

private:
    std::string _name;
    std::string _sourceCode;
    uint32_t _nameSize;
    uint32_t _sourceCodeSize;

};

}; // End of Aio namespace