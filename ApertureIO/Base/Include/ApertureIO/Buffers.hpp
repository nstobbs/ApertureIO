#pragma once

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"
#include "ApertureIO/BufferLayout.hpp"

#include <vector>

namespace Aio {
    class Device;
    class RenderContext;
};

namespace Aio {

enum BufferType
{
    Vertex = 0,
    Index = 1,
    Uniform = 2
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
    static Buffer* CreateBuffer(BufferCreateInfo* createInfo);

    BufferLayout GetBufferLayout();
    void SetBufferLayout(BufferLayout layout);

    virtual void Bind(RenderContext& renderContext) = 0;
    virtual void Unbind() = 0;

    private:
    BufferLayout _layout;
};

// TODO: Rethink about buffers. Could we have a base buffer class that can be used to create index and vertex buffers from, as well as normal buffers??
// TODO: These classes arent currently being used and can be removed.
class IndexBuffer : public Buffer
{
    public:
    static IndexBuffer* CreateIndexBuffer();

    virtual void Bind();
    virtual void Unbind();

    private:
};

class VertexArray
{
    public:
    void* getDataPtr();
    size_t sizeOfArray();

    void AddVertexBuffer();
    void SetIndexBuffer();

    virtual void Bind();
    virtual void Unbind();

    private:
    std::vector<Buffer*> _pVertexBuffers;
    Buffer* _pIndexBuffer;
};

}; // End Aio namespace