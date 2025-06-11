#include "ApertureIO/RenderContext.hpp"

#include <boost/functional/hash.hpp>

namespace Aio {

size_t RenderContext::GetHash()
{
    if (!_valid)
    {
        size_t seed = 0;
        boost::hash_combine(seed, _VertexBuffer);
        boost::hash_combine(seed, _IndexBuffer);
        boost::hash_combine(seed, _StorageBuffer);
        boost::hash_combine(seed, _TargetFrameBuffer);
        boost::hash_combine(seed, _Shader);

        _valid = true;
        return seed;
    }
    
    return _hash;
};


void RenderContext::PauseRendering()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // TODO: not sure if we need the added scope here.
    {
        std::lock_guard<std::mutex> lock(mtx);
        _isReloading = true;
        Logger::LogWarn("Request to Pause Rendering...");
    }

    cv.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // waits for rendering to finish up
};

void RenderContext::UnpauseRendering()
{

    {
        std::lock_guard<std::mutex> lock(mtx);
        _isReloading = false;
        Logger::LogWarn("Request to Start Rendering Again...");
    }

    cv.notify_all();
};

void RenderContext::IsPaused()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] {return !_isReloading.load();});
    lock.unlock();
};

};