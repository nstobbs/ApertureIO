#pragma once

#include "ApertureIO/Context.hpp"
#include "ApertureIO/Device.hpp"
#include "ApertureIO/RenderContext.hpp"

#include <glm/glm.hpp>
#include <efsw/efsw.h>
#include <efsw/efsw.hpp>

#include <string>
#include <unordered_map>

namespace Aio {

enum ShaderType {
    Graphics = 0,
    Compute = 1
};

struct ShaderCreateInfo
{
    Device* pDevice;
    Context* pContext;
    ShaderType type;
    std::string shaderName;
    std::string sourceFilepath;
};

class Shader
{
    public:
    static Shader* CreateShader(ShaderCreateInfo& createInfo);

    virtual void Bind(RenderContext& renderContext) = 0;
    virtual void Unbind() = 0;
    
    virtual void rebuildShader() = 0;
    
    std::string& GetSourceFilePath();
    std::string& GetName();

    virtual void SetVec4(std::string name, glm::vec4 value) = 0;
    virtual void SetFloat(std::string name, float value) = 0;

    protected:
    std::string _name;
    std::string _sourceFilepath;
    std::vector<std::string> _uniformBufferNames;
};

/* TODO: Hot-Loading Notes, the shaderLibaray will be watching the source files for any chanages to the files.
Once we get an event back that one of the shader source files have changed. Then we start the rebuild shader
process. The Listening and Creations of New Shaders should happen on new threads.*/

class ShaderManager;

class ShaderListener : public efsw::FileWatchListener
{   
    public:
    static ShaderListener* CreateShaderListener(ShaderManager* manager);
    void handleFileAction( efsw::WatchID watchid, const std::string& dir,
								   const std::string& filename, efsw::Action action,
								   std::string oldFilename) override;
    private:
    ShaderManager* _pManager;
};

//////////////////////////////////////////////
/// ShaderManager - Hot-Reloading Shaders ///
/////////////////////////////////////////////
//TODO: Maybe this class can be more generic so it can be used for other
// objects like textures, obj, etc...

// TODO: Efsw doesn't seems to support watching of single files.
/// instead we should add folders to watch for shader files
// as part of the constructor of ShaderManager.
class ShaderManager
{
    public:
    ShaderManager();

    Shader* GetShader(std::string& name);
    void AddShader(Shader* shader);
    void CreateShader(ShaderCreateInfo& createInfo);
    void DestroyShader(std::string& name);
    
    private:
    std::unordered_map<std::string, Shader*> _shaders;
    
    efsw::FileWatcher* _pFileWatcher;
    ShaderListener* _pShaderListener;

    friend class ShaderListener;
};

}; // End of Aio Namespace 