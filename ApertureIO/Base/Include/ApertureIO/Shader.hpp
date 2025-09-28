#pragma once

#include "ApertureIO/GenericFileManager.hpp"

#include "ApertureIO/Context.hpp"
#include "ApertureIO/Device.hpp"
#include "ApertureIO/RenderContext.hpp"

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace Aio {

enum class ShaderType {
    Graphics = 0,
    Compute = 1,
    None = 3
};

struct ShaderCreateInfo
{
    Device* pDevice;
    Context* pContext;
    ShaderType type;
    std::string name;
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
    static UniquePtr<Shader> CreateShader(const ShaderCreateInfo& createInfo);

    virtual void Bind(RenderContext& renderContext) = 0;
    virtual void Unbind() = 0;
    
    virtual void sourceFileModified() = 0;
    
    std::filesystem::path& GetSourceFilePath();
    std::string& GetName();
    ShaderType GetShaderType();

    virtual void SetVec4(std::string name, glm::vec4 value) = 0;
    virtual void SetFloat(std::string name, float value) = 0;

protected:
    ShaderType _type = {ShaderType::None};
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
    ShaderLibrary(const std::string folderPath);
    Shader* GetShader(const std::string& name);
    void AddShader(UniquePtr<Shader> shader);
    void CreateShader(const ShaderCreateInfo& createInfo);
    void DestroyShader(const std::string& name);
    
private:
    std::unordered_map<std::string, UniquePtr<Shader>> _shaders;
    ShaderFileManager _shaderFileManager;
};

}; // End of Aio Namespace 