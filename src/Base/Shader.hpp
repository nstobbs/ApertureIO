#ifndef H_SHADER
#define H_SHADER

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Context.hpp"
#include "Device.hpp"

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

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    
    virtual void rebuildShader() = 0;

    virtual void SetVec4(std::string name, glm::vec4 value) = 0;

    private:
    char* _name = "";
    std::vector<std::string> _uniformBufferNames;
};

/* TODO: Hot-Loading Notes, the shaderLibaray will be watching the source files for any chanages to the files.
Once we get an event back that one of the shader source files have changed. Then we start the rebuild shader
process.*/

class ShaderLibrary
{
    public:
    Shader* GetShader(std::string& name);

    void AddShader(Shader* shader, std::string& name);
    void CreateShader(ShaderCreateInfo& createInfo);

    void DestroyShader(std::string& name);
    
    private:
    std::unordered_map<std::string, Shader*> _shaders;
};

}; // End of Aio Namespace 

#endif