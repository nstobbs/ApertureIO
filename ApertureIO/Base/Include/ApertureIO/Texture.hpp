#pragma once

#include "stb_image.h"

#include "ApertureIO/GenericFileManager.hpp"
#include "ApertureIO/Logger.hpp"

#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Handles.hpp"
#include "ApertureIO/Device.hpp"
#include "ApertureIO/Buffers.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace Aio
{

struct TextureCreateInfo
{   
    WeakPtr<Device> pDevice;
    WeakPtr<Context> pContext;
    std::string& filePath;
};

class Texture
{
public:
    static SharedPtr<Texture> CreateTexture(const TextureCreateInfo& createInfo);

    virtual TextureHandle GetTextureHandle() = 0;
    virtual void Bind(RenderContext& rContext) = 0;
    virtual void Unbind() = 0;

    virtual void sourceFileModified() = 0;
    
    std::filesystem::path& GetSourceFilePath();

protected:
    WeakPtr<Device> _device;
    std::string _filepath;
    TextureHandle _handle;
    
    /* TODO: Replace this with some sort of ReaderManager or TextureFactor.
    So that we can support multiple different image types like OpenEXR. */
    void readTextureSourceFile();
    void freePixels();

    /* Raw Texture Stuff */
    stbi_uc* _pixels = {nullptr};
    int _height = {0};
    int _width = {0};
    int _channels = {0};
};

/* Not Ready Yet !*/
class TextureManager
{
public:
    void CreateTexture(const TextureCreateInfo& createInfo);
    void AddTexture(SharedPtr<Texture> ptrTexture);

private:
    GenericFileManager<Texture> _textureFileManager;
    std::vector<SharedPtr<Texture>> _textures;
};
};