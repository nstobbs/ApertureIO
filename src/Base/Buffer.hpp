#ifndef H_BUFFER
#define H_BUFFER

#include <vector>

namespace ApertureIO {

class VertexBuffer
{
    public:
    static VertexBuffer* CreateVertexBuffer();

    private:
};

class IndexBuffer
{
    public:
    static IndexBuffer* CreateVertexBuffer();

    private:
};

class VertexArray
{
    public:
    void* getDataPtr();
    size_t sizeOfArray();

    private:
    std::vector<VertexBuffer*> _pVertexBuffers;
    IndexBuffer* _pIndexBuffer;
};

} // End ApertureIO namespace

#endif