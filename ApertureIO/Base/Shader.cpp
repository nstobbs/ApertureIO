#include "ApertureIO/Shader.hpp"
#include "ApertureIO/GenericFileManager.hpp"
#include "ApertureIO/VulkanShader.hpp"
#include "ApertureIO/Logger.hpp"

#include <thread>

namespace Aio {
    
Shader* Shader::CreateShader(ShaderCreateInfo& createInfo)
{
    auto API =  createInfo.pContext->getRendererAPI();
    switch (API)
    {
        case RendererAPI::eVulkan:
            Shader* shader = new VulkanShader(createInfo);
            shader->_sourceFilepath = createInfo.sourceFilepath;
            return shader;
    }
};

std::filesystem::path& Shader::GetSourceFilePath()
{
    return _sourceFilepath;
};

std::string& Shader::GetName()
{
    return _name;
};


//////////////////////////////////////////////
/// ShaderLibrary - Hot-Reloading Shaders ///
/////////////////////////////////////////////

ShaderLibrary::ShaderLibrary(std::string folderPath)
{
    _shaderFileManager = ShaderFileManager(folderPath);
};

Shader* ShaderLibrary::GetShader(std::string& name)
{
    return _shaders.at(name);
};

void ShaderLibrary::AddShader(Shader* shader)
{
    _shaders.emplace(shader->GetName(), shader);
    _shaderFileManager.AddFileToWatch(shader->GetSourceFilePath(), shader);
};

void ShaderLibrary::CreateShader(ShaderCreateInfo& createInfo)
{
    Shader* shader = Shader::CreateShader(createInfo);
    _shaders.emplace(shader->GetName(), shader);
    _shaderFileManager.AddFileToWatch(shader->GetSourceFilePath(), shader);
};

void ShaderLibrary::DestroyShader(std::string& name)
{
    Shader* shader = _shaders.at(name);
    _shaderFileManager.RemoveObjectFromWatch(shader);
};

};