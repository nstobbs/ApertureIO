#pragma once

#include "ApertureIO/Context.hpp"
#include "ApertureIO/Command.hpp"
#include "ApertureIO/Handles.hpp"

#include <vector>

class Command;

namespace Aio {

class Device
{
public:
    static UniquePtr<Device> CreateDevice(Context* context);
    virtual bool init() = 0;

    BufferHandle CreateUniformBufferHandle();
    BufferHandle CreateStorageBufferHandle();
    TextureHandle CreateTextureHandle();
    TextureHandle CreateStorageImageHandle();

private:
    std::vector<BufferHandle> _uniformBufferHandles;
    std::vector<BufferHandle> _storageBufferHandles;
    std::vector<TextureHandle> _textureHandles;
    std::vector<TextureHandle> _storageImageHandles;
};
}; // End Aio namespace