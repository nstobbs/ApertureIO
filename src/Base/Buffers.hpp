#ifndef H_BUFFERS
#define H_BUFFERS

#include <vector>
#include "Device.hpp"
#include "Context.hpp"
#include "BufferLayout.hpp"

namespace ApertureIO {

struct BufferCreateInfo
{
    Device* device;
    Context* context;
    bool hostAccess = false; // Read and Write Access
    void* data;
    BufferLayout layout;
    uint32_t count;
};

class Buffer
{
    public:
    static Buffer* CreateBuffer(BufferCreateInfo* createInfo);

    virtual void Bind() = 0;
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

} // End ApertureIO namespace

#endif