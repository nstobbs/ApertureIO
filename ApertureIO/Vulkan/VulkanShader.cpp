#include "ApertureIO/VulkanShader.hpp"
#include "ApertureIO/VulkanDevice.hpp"
#include "ApertureIO/VulkanFrameBuffer.hpp"

#include "ApertureIO/Logger.hpp"
#include <memory>

namespace Aio {

VulkanShader::VulkanShader(const ShaderCreateInfo& createInfo)
{
    /* General Shader Details */
    _type = createInfo.type;
    _name = createInfo.name;
    _sourceFilepath = createInfo.sourceFilepath;
    _pDevice = dynamic_cast<VulkanDevice*>(createInfo.pDevice);
    _pContext = dynamic_cast<VulkanContext*>(createInfo.pContext);
    auto bindlessLayout = _pDevice->GetBindlessLayout();

    /* Push Constant For Buffer Handles */
    VkPushConstantRange pushConstantHandlesInfo{};
    pushConstantHandlesInfo.stageFlags = VK_SHADER_STAGE_ALL;
    pushConstantHandlesInfo.offset = 0;
    pushConstantHandlesInfo.size = sizeof(HandlesPushConstant);

    /* Pipeline Layout */
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &bindlessLayout;
    layoutInfo.pushConstantRangeCount = 1;
    layoutInfo.pPushConstantRanges = &pushConstantHandlesInfo;
    VK_ASSERT(vkCreatePipelineLayout(_pDevice->GetVkDevice(), &layoutInfo, nullptr, &_layout), VK_SUCCESS, "Create Pipeline Layout Pipeline");

    auto sourceCode = FileIO::ReadSourceFile(createInfo.sourceFilepath.string());

    switch(_type)
    {
        case ShaderType::Graphics:
        {
            Logger::LogInfo("Creating Graphics Pipeline Layout.");

            /* Read Shader Source Code */
            auto vertSource = FileIO::SplitOutShader(sourceCode, SourceFileType::VertexShader);
            auto fragSource = FileIO::SplitOutShader(sourceCode, SourceFileType::FragmentShader);

            /* Compile the Shader Source Code */
            auto vertCompiledCode = compileShaderSource(vertSource, SourceFileType::VertexShader);

            _vertModule = createShaderModule(vertCompiledCode);
            VkPipelineShaderStageCreateInfo vertCreateInfo{};
            vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertCreateInfo.module = _vertModule;
            vertCreateInfo.pName = "main";

            auto fragCompiledCode = compileShaderSource(fragSource, SourceFileType::FragmentShader);

            _fragModule = createShaderModule(fragCompiledCode);
            VkPipelineShaderStageCreateInfo fragCreateInfo{};
            fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragCreateInfo.module = _fragModule;
            fragCreateInfo.pName = "main";
            
            _shaderStages.push_back(vertCreateInfo);
            _shaderStages.push_back(fragCreateInfo);

            // Input Assembly
            _inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            _inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            _inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
            
            // DynamicStates
            _dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            _dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
            
            _dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            _dynamicStateInfo.pNext = nullptr;
            _dynamicStateInfo.flags = 0;
            _dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(_dynamicStates.size());
            _dynamicStateInfo.pDynamicStates = _dynamicStates.data();

            // Viewport
            _viewport.x = 0.0f;
            _viewport.y = 0.0f;
            _viewport.minDepth = 0.0f;
            _viewport.maxDepth = 1.0f;

            // scissor
            _scissor.offset = {0, 0};

            _viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            _viewportStateInfo.viewportCount = 1;
            _viewportStateInfo.pViewports = &_viewport;
            _viewportStateInfo.scissorCount = 1;
            _viewportStateInfo.pScissors = &_scissor;

            // rasterizer
            _rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            _rasterizerInfo.depthClampEnable = VK_FALSE;
            _rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
            _rasterizerInfo.lineWidth = 1.0f;
            _rasterizerInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
            _rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
            _rasterizerInfo.depthBiasEnable = VK_FALSE;
            _rasterizerInfo.depthBiasConstantFactor = 0.0f;
            _rasterizerInfo.depthBiasClamp = 0.0f;
            _rasterizerInfo.depthBiasSlopeFactor = 0.0f;

            // muilt sampling 
            _multiSamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            _multiSamplingInfo.sampleShadingEnable = VK_FALSE;
            _multiSamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            // depth testing
            _depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            _depthStencilInfo.depthTestEnable = VK_TRUE;
            _depthStencilInfo.depthWriteEnable = VK_TRUE;
            _depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
            _depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
            _depthStencilInfo.minDepthBounds = 0.0f;
            _depthStencilInfo.maxDepthBounds = 1.0f;
            _depthStencilInfo.stencilTestEnable = VK_FALSE;
            _depthStencilInfo.front = {};
            _depthStencilInfo.back = {};

            // pipeline 
            _pipelineGraphicsInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            _pipelineGraphicsInfo.stageCount = 2;

            _pipelineGraphicsInfo.pStages = _shaderStages.data();
            _pipelineGraphicsInfo.pInputAssemblyState = &_inputAssemblyInfo;
            _pipelineGraphicsInfo.pViewportState = &_viewportStateInfo;
            _pipelineGraphicsInfo.pRasterizationState = &_rasterizerInfo;
            _pipelineGraphicsInfo.pMultisampleState = &_multiSamplingInfo;
            _pipelineGraphicsInfo.pDepthStencilState = &_depthStencilInfo;
            _pipelineGraphicsInfo.pColorBlendState = &_colorBlendingInfo;
            _pipelineGraphicsInfo.pDynamicState = &_dynamicStateInfo;

            _pipelineGraphicsInfo.layout = _layout;

            _pipelineGraphicsInfo.basePipelineHandle = VK_NULL_HANDLE;
            _pipelineGraphicsInfo.basePipelineIndex = -1;
            break;
        }
            
        case ShaderType::Compute:
        {
            Logger::LogInfo("Creating Compute Pipeline Layout.");

            /* Read & Compile Shader */
            auto computeSource = FileIO::SplitOutShader(sourceCode, SourceFileType::ComputeShader);
            auto computeCompiledCode = compileShaderSource(computeSource, SourceFileType::ComputeShader);
            _compModule = createShaderModule(computeCompiledCode);

            VkPipelineShaderStageCreateInfo computeCreateInfo{};
            computeCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            computeCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            computeCreateInfo.module = _compModule;
            computeCreateInfo.pName = "main";

            _shaderStages.push_back(computeCreateInfo);
            break;
        }   
    };
};

VulkanShader::~VulkanShader()
{
    Logger::LogWarn("VulkanShader Destructor was called.");
    switch (_type)
    {
        case ShaderType::Graphics:
        {
            vkDestroyPipeline(_pDevice->GetVkDevice(), _pipeline, nullptr);
            vkDestroyPipelineLayout(_pDevice->GetVkDevice(), _layout, nullptr);
            vkDestroyShaderModule(_pDevice->GetVkDevice(), _fragModule, nullptr);
            vkDestroyShaderModule(_pDevice->GetVkDevice(), _vertModule, nullptr);
        };
    }
    
};

VulkanShader& VulkanShader::operator=(VulkanShader&& otherShader) noexcept
{
    Logger::LogWarn("VulkanShader Move Assignment was called.");
    if (this != &otherShader)
    {
        _name = otherShader._name;
        _type = otherShader._type;
        _sourceFilepath = otherShader._sourceFilepath;
        _pDevice = otherShader._pDevice;
        _pContext = otherShader._pContext;

        switch (otherShader._type)
        {
            case ShaderType::Graphics:
            {
                _viewport = otherShader._viewport;
                _scissor = otherShader._scissor;
                _vertModule = otherShader._vertModule;
                _fragModule = otherShader._fragModule;
                _dynamicStates = otherShader._dynamicStates;
                _layout = otherShader._layout;

                _shaderStages = otherShader._shaderStages;
                _inputAssemblyInfo = otherShader._inputAssemblyInfo;
                _dynamicStateInfo = otherShader._dynamicStateInfo;
                _dynamicStateInfo.pDynamicStates = _dynamicStates.data();
                _viewportStateInfo = otherShader._viewportStateInfo;
                _viewportStateInfo.pViewports = &_viewport;
                _viewportStateInfo.pScissors = &_scissor;
                _rasterizerInfo = otherShader._rasterizerInfo;
                _multiSamplingInfo = otherShader._multiSamplingInfo;
                //_colorBlendAttachmentInfo = otherShader._colorBlendAttachmentInfo;
                _colorBlendingInfo = otherShader._colorBlendingInfo;
                //_colorBlendingInfo.pAttachments = &_colorBlendAttachmentInfo;
                _depthStencilInfo = otherShader._depthStencilInfo;
                _pipelineGraphicsInfo = otherShader._pipelineGraphicsInfo;
                
                _pipelineGraphicsInfo.pStages = _shaderStages.data();
                _pipelineGraphicsInfo.pInputAssemblyState = &_inputAssemblyInfo;
                _pipelineGraphicsInfo.pViewportState = &_viewportStateInfo;
                _pipelineGraphicsInfo.pRasterizationState = &_rasterizerInfo;
                _pipelineGraphicsInfo.pMultisampleState = &_multiSamplingInfo;
                _pipelineGraphicsInfo.pDepthStencilState = &_depthStencilInfo;
                _pipelineGraphicsInfo.pColorBlendState = &_colorBlendingInfo;
                _pipelineGraphicsInfo.pDynamicState = &_dynamicStateInfo;
                _pipelineGraphicsInfo.layout = _layout;
                
                _previousHash = 0;
            }
        }
    };

    return *this;
};

VkPipeline VulkanShader::createPipeline(RenderContext& renderContext)
{   
    /* TODO: with better hashing, we could use that to id what areas
    of the shader needs rebuilding. E.g. if only the framebuffer
    is different compared to the last shader then we can just update 
    that framebuffer / render pass info and rebuild or if only the 
    source file has been modified then just rebuild shader with new 
    VkShaderModules... 

    Currently any changes to renderContext state will toggle 
    the new creation of a VkPipeline object with updating the
    the buffer layout and the framebuffer swapChain extent for the
    VkPipeline..
    */
    size_t hash = renderContext.GetHash();
    if ( hash != _previousHash )
    {
        _previousHash = hash;
        switch (_type)
        {
            case ShaderType::Graphics:
            {
                Logger::LogWarn("RenderContext Hash is Different. Creating Graphics Pipeline.");
                // Vertex Input
                //TODO: Generate the Vertex Layout from the BufferLayout
                BufferLayout vertexBufferLayout = renderContext._VertexBuffer->GetBufferLayout();
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = vertexBufferLayout.GetStride();
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                //TODO: this needs to calculate where each element is offsets wise
                // TODO: this should be here as it't not really related to the shader.
                // But more about the buffer layout itself
                std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
                uint32_t offset = 0;

                for (int i = 0; i < vertexBufferLayout.GetElementCount(); i++)
                {
                    BufferElement element = vertexBufferLayout.GetBufferElementAt(i);
                    VkVertexInputAttributeDescription description{};
                    description.binding = 0;
                    description.location = i;
                    switch(element.type)
                    {
                        case BufferElementType::Float:
                            if (element.count == 1)
                            {   
                                description.format = VK_FORMAT_R32_SFLOAT;
                                description.offset = offset;

                            } else if (element.count == 2)
                            {

                                description.format = VK_FORMAT_R32G32_SFLOAT;
                                description.offset = offset;

                            } else if (element.count == 3)
                            {
                                
                                description.format = VK_FORMAT_R32G32B32_SFLOAT;
                                description.offset = offset;

                            } else if (element.count == 4)
                            {
                                description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
                                description.offset = offset;
                            }

                            offset = offset + (sizeof(float) * element.count);
                            break;

                        case BufferElementType::Int:
                            if (element.count == 1)
                            {   
                                description.format = VK_FORMAT_R32_SINT;
                                description.offset = offset;

                            } else if (element.count == 2)
                            {

                                description.format = VK_FORMAT_R32G32_SINT;
                                description.offset = offset;

                            } else if (element.count == 3)
                            {
                                
                                description.format = VK_FORMAT_R32G32B32_SINT;
                                description.offset = offset;

                            } else if (element.count == 4)
                            {
                                description.format = VK_FORMAT_R32G32B32A32_SINT;
                                description.offset = offset;
                            }

                            offset = offset + (sizeof(int) * element.count);
                            break;

                        case BufferElementType::Bool:
                            break;
                    }

                    attributeDescriptions.push_back(description);
                };
                
                VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
                vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputInfo.vertexBindingDescriptionCount = 1;
                vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
                vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
                vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

                _pipelineGraphicsInfo.pVertexInputState = &vertexInputInfo;

                /* Target FrameBuffer*/
                FrameBuffer* framebuffer = renderContext._TargetFrameBuffer;
                _pipelineGraphicsInfo.renderPass = dynamic_cast<VulkanFrameBuffer*>(framebuffer)->GetRenderPass();

                auto targetExtent = dynamic_cast<VulkanFrameBuffer*>(framebuffer)->GetExtent();
                _viewport.height = targetExtent.height;
                _viewport.width = targetExtent.width;
                _scissor.extent = targetExtent;

                // colour attachment
                VkPipelineColorBlendAttachmentState colorAttachmentInfo{};
                std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentInfos;

                colorAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
                colorAttachmentInfo.blendEnable = VK_FALSE;
                colorAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                colorAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                colorAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
                colorAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                colorAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                colorAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;

                colorAttachmentInfos.push_back(colorAttachmentInfo);
                colorAttachmentInfos.push_back(colorAttachmentInfo);
                
                _colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                _colorBlendingInfo.logicOpEnable = VK_FALSE;
                _colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
                _colorBlendingInfo.attachmentCount = static_cast<uint32_t>(colorAttachmentInfos.size());
                _colorBlendingInfo.pAttachments = colorAttachmentInfos.data();
                _colorBlendingInfo.blendConstants[0] = 0.0f;
                _colorBlendingInfo.blendConstants[1] = 0.0f;
                _colorBlendingInfo.blendConstants[2] = 0.0f;
                _colorBlendingInfo.blendConstants[3] = 0.0f;

                VkPipeline pipeline;
                auto result = vkCreateGraphicsPipelines(_pDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &_pipelineGraphicsInfo, nullptr, &pipeline);
                VK_ASSERT(result, VK_SUCCESS, "Failed: vkCreateGraphicsPipelines.");
                return pipeline;
            }

            case ShaderType::Compute:
            {
                Logger::LogWarn("RenderContext Hash is Different. Creating Compute Pipeline.");
                _pipelineComputeInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
                _pipelineComputeInfo.layout = _layout;
                _pipelineComputeInfo.stage = _shaderStages[0];
                
                VkPipeline pipeline;
                auto result = vkCreateComputePipelines(_pDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &_pipelineComputeInfo, nullptr, &pipeline);
                VK_ASSERT(result, VK_SUCCESS, "Failed: vkCreateComputePipelines.");
                return pipeline;
            }
        };
    }

    return VK_NULL_HANDLE;
};

void VulkanShader::sourceFileModified()
{   
    if(!_alreadyRebuilding)
    {
        auto msg = "Rebuilding Shader: " + GetName();
        Logger::LogWarn(msg);

        _alreadyRebuilding = true;
        _bound->PauseRendering();
        VK_ASSERT(vkDeviceWaitIdle(_pDevice->GetVkDevice()), VK_SUCCESS, "Wait for Device to Idle.");
        
        switch (_type)
        {
            case ShaderType::Graphics:
            // Read Shader Source File
            //TODO: Double check if we are safe to call GetSourceFilePath().
            auto sourceCode = FileIO::ReadSourceFile(GetSourceFilePath().string());
            auto vertSource = FileIO::SplitOutShader(sourceCode, SourceFileType::VertexShader);
            auto fragSource = FileIO::SplitOutShader(sourceCode, SourceFileType::FragmentShader);
        
            // ReCompile Source Code.
            auto compiledVertCode = compileShaderSource(vertSource, SourceFileType::VertexShader);
            auto compiledFragCode = compileShaderSource(fragSource, SourceFileType::FragmentShader);
        
            // Destroy Old ShaderModule 
            vkDestroyShaderModule(_pDevice->GetVkDevice(), _vertModule, nullptr);
            vkDestroyShaderModule(_pDevice->GetVkDevice(), _fragModule, nullptr);

            // Create a ShaderModule
            _vertModule = createShaderModule(compiledVertCode);
            _fragModule = createShaderModule(compiledFragCode);

            // Update VkPipelineShaderStageCreateInfo
            VkPipelineShaderStageCreateInfo vertCreateInfo{};
            vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertCreateInfo.module = _vertModule;
            vertCreateInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fragCreateInfo{};
            fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragCreateInfo.module = _fragModule;
            fragCreateInfo.pName = "main";

            // Update _shaderStages
            _shaderStages.clear();
            _shaderStages.push_back(vertCreateInfo);
            _shaderStages.push_back(fragCreateInfo);

            // Run CreatePipeline and Create an New* Pipeline
            //TODO: temp workaround for recreating pipline. Needs to think more about the renderContext object.
            VkPipeline freshPipeline;
            if(_bound)
            {
                RenderContext& rContext = *_bound;
                _previousHash = NULL; // FORCES A Rebuild of the pipeline cause of the hashes
                freshPipeline = createPipeline(rContext);
            }

            if (freshPipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(_pDevice->GetVkDevice(), _pipeline, nullptr);
                _pipeline = freshPipeline;
            }

            _bound->UnpauseRendering();
            _alreadyRebuilding = false;
            break;
        }
    };
};

void VulkanShader::Bind(RenderContext& renderContext)
{
    //TODO: createPipeline will need to be moved to a better
    // time of the renderContext cycle instead of happening
    // at bind time. 
    renderContext._Shader = dynamic_cast<Shader*>(this);
    _pipeline = createPipeline(renderContext);
    _bound = &renderContext;
};

void VulkanShader::Unbind()
{

};

void VulkanShader::SetVec4(std::string name, glm::vec4 value)
{

};

void VulkanShader::SetFloat(std::string name, float value)
{
    // Check if this variable exists or not.
    if (std::find(_uniformBufferNames.begin(), _uniformBufferNames.end(), name) != _uniformBufferNames.end())
    {
        // Update the Value of the Uniform Buffer.
    }
    else
    {
        // Create the Uniform Buffer and Update the Value.
        _uniformBufferNames.push_back(name);

        BufferElement elementInfo{};
        elementInfo.count = 1;
        elementInfo.type = BufferElementType::Float;
        elementInfo.normalized = false;

        _uniformBufferLayout.AddBufferElement(elementInfo);

        // ReBuild Uniform Buffer with new Layout.
        // If we can't set new uniform variables after it starts rendering
        // then we won't need a rebuild uniform buffer.
        // This whole class could be moved out of here. 

        // Set the Value
    };
};

VkShaderModule VulkanShader::createShaderModule(std::vector<uint32_t>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size() * sizeof(uint32_t);
    createInfo.pCode = code.data();
    
    VkShaderModule shaderModule;
    VK_ASSERT(vkCreateShaderModule(_pDevice->GetVkDevice(), &createInfo, nullptr, &shaderModule), VK_SUCCESS, "Create Shader Module");
    return shaderModule;
};

std::vector<uint32_t> VulkanShader::compileShaderSource(std::string& code, SourceFileType type)
{
    shaderc_shader_kind shadercType;
    auto shaderName = GetName();
    switch(type)
    {
        case SourceFileType::VertexShader:
            shadercType = shaderc_glsl_vertex_shader;
            shaderName = shaderName + "_VertexShader";
            break;

        case SourceFileType::FragmentShader:
            shadercType = shaderc_glsl_fragment_shader;
            shaderName = shaderName + "_FragmentShader";
            break;
        
        case SourceFileType::ComputeShader:
            shadercType = shaderc_glsl_compute_shader;
            shaderName = shaderName + "_ComputeShader";
            break;
    }
    
    shaderc::Compiler* compiler = _pContext->GetShadercCompiler();
    shaderc::CompileOptions options;
    FileIncluder includer;
    options.SetIncluder(std::make_unique<FileIncluder>());

    auto result = compiler->CompileGlslToSpv(code.c_str(), code.size(),
                                                    shadercType, shaderName.c_str(), options);

    auto compileStatus = result.GetCompilationStatus();
    if (compileStatus != shaderc_compilation_status_success)
    {
        auto msg = "Failed to Compile: " + shaderName;
        Logger::LogError(msg);
        Logger::LogError(result.GetErrorMessage());
        throw std::runtime_error("");
    }
    
    std::vector<uint32_t> compiledCode(result.cbegin(), result.cend());
    // TODO: release the compiled shader 
    return compiledCode;
};

VkViewport VulkanShader::GetViewport()
{
    return _viewport;
};

VkRect2D VulkanShader::GetScissor()
{
    return _scissor;
};

VkPipeline VulkanShader::GetPipeline()
{
    return _pipeline;
};

VkPipelineLayout VulkanShader::GetPipelineLayout()
{
    return _layout;
};


// FileIncluder Functions

shaderc_include_result* FileIncluder::GetInclude(const char* requested_source,
                                               shaderc_include_type type,
                                               const char* requesting_source,
                                               size_t include_depth)
{
    //TODO: Comeback and fix hardcoded file path
    _sourceCode = FileIO::ReadSourceFile(std::string("./Shaders/ApertureIO.glsl"));
    _sourceCodeSize = _sourceCode.size();

    _name = std::string(requested_source);
    _nameSize = _name.size();

    shaderc_include_result* result = new shaderc_include_result{_name.c_str(), _nameSize, _sourceCode.c_str(), _sourceCodeSize, nullptr};
    return result;
};

void FileIncluder::ReleaseInclude(shaderc_include_result* data)
{
    //TODO: come back and fixme
    //delete[] data->source_name;
    //delete[] data->content;
    //delete data;
};

};