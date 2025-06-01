#ifndef H_BUFFERS
#define H_BUFFERS

#include <vector>
#include "Device.hpp"
#include "Context.hpp"
#include "RenderContext.hpp"
#include "BufferLayout.hpp"

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

} // End Aio namespace

#endif