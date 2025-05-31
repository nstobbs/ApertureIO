#ifndef H_RENDERCONTEXT
#define H_RENDERCONTEXT

#include <functional>

#include "Device.hpp"
#include "Context.hpp"
#include "Command.hpp"
#include "Buffers.hpp"
#include "FrameBuffer.hpp"
#include "Shader.hpp"


/* singleton object that store the currently Bound objects
to be used for rendering the current object. */
namespace Aio {

class RenderContext
{
    public:
    size_t GetHash();

    Device* _Device;
    Context* _Context;

    Command* _Command;

    Buffer* _VertexBuffer;
    Buffer* _IndexBuffer;
    Buffer* _StorageBuffer;

    Shader* _Shader;

    FrameBuffer* _TargetFrameBuffer;


    private:
    

};

};
#endif