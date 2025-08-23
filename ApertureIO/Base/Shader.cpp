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


//////////////////////////////////////////////
/// ShaderLibrary - Hot-Reloading Shaders ///
/////////////////////////////////////////////

//TODO: Come back and FIXME. UniquePtrs broke it 

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
    //TODO: Double check this function
    auto shader = Shader::CreateShader(createInfo);
    _shaders.emplace(shader.get()->GetName(), shader.get());
    _shaderFileManager.AddFileToWatch(shader->GetSourceFilePath(), shader.get());
};

void ShaderLibrary::DestroyShader(std::string& name)
{
    Shader* shader = _shaders.at(name);
    _shaderFileManager.RemoveObjectFromWatch(shader);
};

};