#ifndef H_BUFFERLAYOUT
#define H_BUFFERLAYOUT

#include <glm/glm.hpp>
#include <vector> 

namespace Aio {


enum BufferElementType
{
    Float = 0,
    Int = 1,
    Bool = 2
};

struct BufferElement
{
    BufferElementType type;
    uint32_t count;
    bool normalized;
};

class BufferLayout
{
    public:
    void AddBufferElement(BufferElement element);
    uint32_t GetElementCount();
    size_t GetStride();
    BufferElement GetBufferElementAt(uint32_t index);

    private:
    std::vector<BufferElement> _elements;
    uint32_t _stride; // the distance between vertices 

    size_t calculateStride();
};

};

#endif