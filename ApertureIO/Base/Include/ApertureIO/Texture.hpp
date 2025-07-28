#pragma once

#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/Handles.hpp"
#include "ApertureIO/Device.hpp"

#include "ApertureIO/GenericFileManager.hpp"

#include <string>

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

    virtual void Bind(RenderContext* rContext) = 0;
    virtual void Unbind() = 0;

protected:
    TextureHandle* _handle;
    std::string _filepath; 
};

class TextureManager
{
public:

private:
    GenericFileManager<Texture> FileManager; 

};


};