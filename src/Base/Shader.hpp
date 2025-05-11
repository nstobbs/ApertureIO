#ifndef H_SHADER
#define H_SHADER

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Context.hpp"
#include "Device.hpp"

namespace ApertureIO {

//TODO shorter name maybe?
struct ShaderComputeCreateInfo
{
    Device* pDevice;
    Context* pContext;
    std::string shaderName;
    std::string srcFilepath;
};

struct ShaderGraphicsCreateInfo
{
    Device* pDevice;
    Context* pContext;
    std::string shaderName;
    std::string srcFragFilepath;
    std::string srcVertFilepath;
};

class Shader
{
    public:
    static Shader* CreateShader(ShaderComputeCreateInfo& createInfo);
    static Shader* CreateShader(ShaderGraphicsCreateInfo& createInfo);

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual void SetVec4(std::string name, glm::vec4 value) = 0;

    private:
    char* _name = "";
    std::vector<std::string> _uniformBufferNames;
};

class ShaderLibrary
{
    public:
    Shader* GetShader(std::string name);

    void AddShader(Shader shader, std::string name);
    void CreateComputeShader(ShaderComputeCreateInfo* createInfo);
    void CreateGraphicsShader(ShaderGraphicsCreateInfo* createInfo);

    void DestroyShader(std::string name);
    
    private:
    std::unordered_map<std::string, Shader*> _shaders;
};

}; // End of ApertureIO Namespace 

#endif