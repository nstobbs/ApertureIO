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
    /* TODO: really shouldn't need to pass the context around like this.*/
    static Buffer* CreateBuffer(BufferCreateInfo* createInfo);

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    private:
    BufferLayout _layout;
};

class IndexBuffer
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