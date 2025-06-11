#include "ApertureIO/Shader.hpp"
#include "ApertureIO/VulkanShader.hpp"
#include "ApertureIO/Logger.hpp"

#include <thread>

namespace Aio {
    
Shader* Shader::CreateShader(ShaderCreateInfo& createInfo)
{
    auto API =  createInfo.pContext->getRendererAPI();
    switch (API)
    {
        case eVulkan:
            Shader* shader = new VulkanShader(createInfo);
            shader->_sourceFilepath = createInfo.sourceFilepath;
            shader->_name = createInfo.shaderName;
            return shader;
    }
};

std::string& Shader::GetSourceFilePath()
{
    return _sourceFilepath;
};

std::string& Shader::GetName()
{
    return _name;
};

//////////////////////////////////////////////
/// ShaderListener - Hot-Reloading Shaders ///
/////////////////////////////////////////////

ShaderListener* ShaderListener::CreateShaderListener(ShaderManager* manager)
{
    ShaderListener* listener = new ShaderListener;
    listener->_pManager = manager;
    return listener;
};

void ShaderListener::handleFileAction( efsw::WatchID watchid, const std::string& dir,
								   const std::string& filename, efsw::Action action,
								   std::string oldFilename)
{
    for (auto& s : _pManager->_shaders)
    {
        auto shader = s.second;
        auto shaderSourcePath = shader->GetSourceFilePath(); 
        auto pos = shaderSourcePath.find_last_of("/");

        std::string shaderFilename = shaderSourcePath.substr(pos + 1);

        if (filename == shaderFilename && action == efsw::Action::Modified)
        {
            auto msg = "ShaderListener: Shader SourceFile: " + filename + " was modified.";
            Logger::LogWarn(msg);
            shader->rebuildShader();
        }        
    }
};

//////////////////////////////////////////////
/// ShaderManager - Hot-Reloading Shaders ///
/////////////////////////////////////////////

ShaderManager::ShaderManager()
{
    _pFileWatcher = new efsw::FileWatcher;
    _pShaderListener = ShaderListener::CreateShaderListener(this);

    _pFileWatcher->watch();
};

Shader* ShaderManager::GetShader(std::string& name)
{
    return _shaders.at(name);
};

void ShaderManager::AddShader(Shader* shader)
{
    std::string name = shader->GetName();
    _shaders.emplace(name, shader);

    efsw::WatchID id = _pFileWatcher->addWatch(shader->GetSourceFilePath(), _pShaderListener, false);
    efsw::WatchID id2 = _pFileWatcher->addWatch("./Shaders/", _pShaderListener, false);
    auto msg = "ShaderManager: Started Watching - " + shader->GetSourceFilePath();
    Logger::LogInfo(msg);
};

void ShaderManager::CreateShader(ShaderCreateInfo& createInfo)
{
    Shader* shader = Shader::CreateShader(createInfo);
    std::string name = shader->GetName();
    _shaders.emplace(name, shader);

    efsw::WatchID id = _pFileWatcher->addWatch(shader->GetSourceFilePath(), _pShaderListener, false);
    auto msg = "ShaderManager: Started Watching - " + shader->GetSourceFilePath();
    Logger::LogInfo(msg);
};

void ShaderManager::DestroyShader(std::string& name)
{
    // remove from watch list

    // destroy shader
};

};