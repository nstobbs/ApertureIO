#pragma once

#include "stb_image.h"

#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Handles.hpp"
#include "ApertureIO/Device.hpp"
#include "ApertureIO/Buffers.hpp"

#include "ApertureIO/GenericFileManager.hpp"
#include "ApertureIO/Logger.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace Aio
{

struct TextureCreateInfo
{   
    Device* device = {nullptr};
    Context* context = {nullptr};
    std::string filePath;
    //TODO Add Colorspace...
};

class Texture
{
public:
    static Texture* CreateTexture(TextureCreateInfo createInfo);

    virtual TextureHandle* GetTextureHandle() = 0;
    virtual void Bind(RenderContext& rContext) = 0;
    virtual void Unbind() = 0;

    virtual void sourceFileModified() = 0;
    
    std::filesystem::path& GetSourceFilePath();

protected:
    std::string _filepath;
    TextureHandle _handle;
    Device* _device = {nullptr};
    
    //TODO: Maybe Rethink about how we want to Handle
    // So that it's works
    void readTextureSourceFile();
    void freePixels();

    /* Raw Texture Stuff */
    stbi_uc* _pixels = {nullptr};
    int _height = {0};
    int _width = {0};
    int _channels = {0};
};

class TextureManager
{
public:
    void CreateTexture(TextureCreateInfo createInfo);
    void AddTexture(Texture* ptrTexture);

private:
    GenericFileManager<Texture> _textureFileManager;
    std::vector<Texture*> _textures;
};


};