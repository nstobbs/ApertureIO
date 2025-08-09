#pragma once

#include "ApertureIO/GenericFileManager.hpp"

#include "ApertureIO/Context.hpp"
#include "ApertureIO/Device.hpp"
#include "ApertureIO/RenderContext.hpp"

#include <glm/glm.hpp>
#include <efsw/efsw.h>
#include <efsw/efsw.hpp>

#include <string>
#include <filesystem>
#include <unordered_map>

namespace Aio {

enum class ShaderType {
    Graphics = 0,
    Compute = 1
};

struct ShaderCreateInfo
{
    Device* pDevice;
    Context* pContext;
    ShaderType type;
    std::string shaderName;
    std::filesystem::path sourceFilepath;
};

/**
 * @class Shader
 * @brief A Shader Program that runs on a logical Aio::Device.
 * This can be a Compute Shaders for running calculation on the GPU, or
 *  Graphics Shaders for use of fixed functions on graphics rendering tasks.
 */

class Shader
{
public:
    static Shader* CreateShader(ShaderCreateInfo& createInfo);

    virtual void Bind(RenderContext& renderContext) = 0;
    virtual void Unbind() = 0;
    
    virtual void sourceFileModified() = 0;
    
    std::filesystem::path& GetSourceFilePath();
    std::string& GetName();

    virtual void SetVec4(std::string name, glm::vec4 value) = 0;
    virtual void SetFloat(std::string name, float value) = 0;

protected:
    std::string _name;
    std::filesystem::path _sourceFilepath;
    std::vector<std::string> _uniformBufferNames;
};

/**
 * @class ShaderLibrary
 * @brief ShaderLibrary Creates and Stores any shaders. Returns shaders if requested.
 *  Rebuilds any Shaders when the source file has been modified via calling on sourceFileModified().
 */
class ShaderLibrary
{
public:
    ShaderLibrary(std::string folderPath);
    Shader* GetShader(std::string& name);
    void AddShader(Shader* shader);
    void CreateShader(ShaderCreateInfo& createInfo);
    void DestroyShader(std::string& name);
    
private:
    std::unordered_map<std::string, Shader*> _shaders;
    ShaderFileManager _shaderFileManager;
};

}; // End of Aio Namespace 