#include "ApertureIO/Shader.hpp"
#include "ApertureIO/GenericFileManager.hpp"
#include "ApertureIO/VulkanShader.hpp"
#include "ApertureIO/Logger.hpp"

#include <thread>

namespace Aio {
    
UniquePtr<Shader> Shader::CreateShader(const ShaderCreateInfo& createInfo)
{
    auto API =  createInfo.pContext->getRendererAPI();
    switch (API)
    {
        case RendererAPI::eVulkan:
            return std::make_unique<VulkanShader>(createInfo);
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

ShaderType Shader::GetShaderType()
{
    return _type;
};


//////////////////////////////////////////////
/// ShaderLibrary - Hot-Reloading Shaders ///
/////////////////////////////////////////////

ShaderLibrary::ShaderLibrary(const std::string folderPath)
{
    _shaderFileManager = ShaderFileManager(folderPath);
};

Shader* ShaderLibrary::GetShader(const std::string& name)
{
    return _shaders.at(name).get();
};

void ShaderLibrary::AddShader(UniquePtr<Shader> shader)
{   
    auto pShader = shader.get();
    _shaderFileManager.AddFileToWatch(pShader->GetSourceFilePath(), pShader);
    _shaders.emplace(pShader->GetName(), std::move(shader));
};

void ShaderLibrary::CreateShader(const ShaderCreateInfo& createInfo)
{
    auto name = createInfo.name;
    _shaders.emplace(name, std::move(Shader::CreateShader(createInfo)));
    _shaderFileManager.AddFileToWatch(_shaders.at(name).get()->GetSourceFilePath(), _shaders.at(name).get());
};

void ShaderLibrary::DestroyShader(const std::string& name)
{
    auto shader = _shaders.at(name).get();
    _shaderFileManager.RemoveObjectFromWatch(shader);
};
};