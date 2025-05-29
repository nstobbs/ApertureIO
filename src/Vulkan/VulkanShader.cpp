#include "VulkanShader.hpp"
#include "VulkanDevice.hpp"
#include "../Common/Logger.hpp"
#include "../Common/FileIO.hpp"

namespace Aio {

VulkanShader::VulkanShader(ShaderCreateInfo& createInfo)
{
    ShaderType currentType = createInfo.type;
    auto l = _pDevice->GetBindlessLayout();
    _pDevice = dynamic_cast<VulkanDevice*>(createInfo.pDevice);

    switch(currentType)
    {   
        /* Create a Pipeline Layout.*/
        case ShaderType::Graphics:
        {
            Logger::LogInfo("Creating Graphics Pipeline Layout.");

            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount = 1;
            layoutInfo.pSetLayouts = &l;
            VK_ASSERT(vkCreatePipelineLayout(_pDevice->GetVkDevice(), &layoutInfo, nullptr, &_layout), VK_SUCCESS, "Create Graphics Pipeline");

            /* Create the Pipeline */
            /* Read Shader Source Code */
            auto sourceCode = FileIO::ReadSourceFile(createInfo.shaderName);
            //auto fragSource = FileIO::SplitOutShader(sourceCode, SourceFileType::FragmentShader); 
            //auto vertSource = FileIO::SplitOutShader(sourceCode, SourceFileType::VertexShader);

            /* Compile the Shader Source Code */

            break;
        }
            
        case ShaderType::Compute:
        {
            Logger::LogInfo("Creating Compute Pipeline Layout.");
            break;
        }
            
    };
};

void VulkanShader::rebuildShader()
{

};

VulkanShader::~VulkanShader()
{

};

void VulkanShader::Bind()
{

};
void VulkanShader::Unbind()
{

};

void VulkanShader::SetVec4(std::string name, glm::vec4 value)
{

};

VkShaderModule VulkanShader::createShaderModule(VkDevice device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = nullptr;
    
    VkShaderModule nullShader;
    VK_ASSERT(vkCreateShaderModule(device, &createInfo, nullptr, &nullShader), VK_SUCCESS, "NULL SHADER");
    return nullShader;
};

};