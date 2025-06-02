#ifndef H_SHADER
#define H_SHADER

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Context.hpp"
#include "Device.hpp"
#include "RenderContext.hpp"

#include <efsw/efsw.h>
#include <efsw/efsw.hpp>

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

    private:
    std::string _name = "";
    std::string _sourceFilepath = "";
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

#endif