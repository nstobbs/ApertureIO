#include "Shader.hpp"
#include "../Common/Logger.hpp"

#include "../Vulkan/VulkanShader.hpp"

namespace Aio {
    
Shader* Shader::CreateShader(ShaderCreateInfo& createInfo)
{
    auto API =  createInfo.pContext->getRendererAPI();
    switch (API)
    {
        case eVulkan:
            return new VulkanShader(createInfo);
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
/// ShaderWatcher - Hot-Reloading Shaders ///
/////////////////////////////////////////////

ShaderWatcher::ShaderWatcher()
{
    // Start Watcher Thread.?!?.
    _fileWatcher.watch();
};

/* TODO: need to rethink how I want to handle the listening and watch of shaders.
Any shader that a part of the shaderLibary should be watched.
Whenever the Watched Shader Source Files */
void ShaderListener::handleFileAction( efsw::WatchID watchid, const std::string& dir,
								   const std::string& filename, efsw::Action action,
								   std::string oldFilename)
{
    for(auto shader : _pShaders)
    {   
        if (filename != shader->GetSourceFilePath())
        {
            switch(action)
            {
                case efsw::Action::Modified:
                    Logger::LogInfo("ShaderListener: " + filename + " Was Modified..");
                    shader->rebuildShader();
                    Logger::LogWarn("ShaderListener: Rebuilding Shader " + shader->GetName() + "\n");
                    break;

                case efsw::Action::Moved:
                    Logger::LogInfo("ShaderListener: " + filename + " Was Moved..");
                    break;
                
                case efsw::Action::Add:
                    Logger::LogInfo("ShaderListener: " + filename + " Was Added..");
                    break;

                case efsw::Action::Delete:
                    Logger::LogInfo("ShaderListener: " + filename + " Was Deleted..");
                    break;
            };
        };
    };
};

};