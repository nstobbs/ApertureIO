#pragma once

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/BufferLayout.hpp"
#include "ApertureIO/Handles.hpp"

#include <vector>

namespace Aio {
    class Device;
    class RenderContext;
};

namespace Aio {

enum class BufferType
{
    Vertex = 0,
    Index = 1,
    Uniform = 2,
};

struct BufferCreateInfo
{
    Device* device;
    Context* context;
    BufferType type;
    BufferLayout layout;
    void* data;
    uint32_t count;
};

class Buffer
{
public:
    static UniquePtr<Buffer> CreateBuffer(BufferCreateInfo& createInfo);

    BufferLayout GetBufferLayout();
    virtual BufferHandle GetBufferHandle() = 0;
    void SetBufferLayout(BufferLayout layout);

    virtual void Bind(RenderContext& renderContext) = 0;
    virtual void Unbind() = 0;
    uint32_t Count();

    virtual void UploadToDevice(void* data) = 0;

protected:
    BufferLayout _layout;
    uint32_t _count;
};
}; // End Aio namespace