#ifndef H_RENDERCONTEXT
#define H_RENDERCONTEXT

#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>

#include "Device.hpp"
#include "Context.hpp"
#include "Command.hpp"
#include "Buffers.hpp"
#include "FrameBuffer.hpp"
#include "Shader.hpp"

#include "../Common/Logger.hpp"

namespace Aio {
    class Buffer;
    class Shader;
    class FrameBuffer;
}

/* singleton object that store the currently Bound objects
to be used for rendering the current object. */
namespace Aio {

class RenderContext
{
    public:
    size_t GetHash();
    
    void IsPaused(); // TODO: not a good name since it doesn't return an bool anymore.
    void PauseRendering();
    void UnpauseRendering();

    Device* _Device;
    Context* _Context;
    
    Buffer* _VertexBuffer;
    Buffer* _IndexBuffer;
    Buffer* _StorageBuffer;
    Shader* _Shader;
    FrameBuffer* _TargetFrameBuffer;

    private:

    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> _isReloading = {false};

    bool _valid = {false};
    size_t _hash;
};

};
#endif