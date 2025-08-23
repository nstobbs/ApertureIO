#include "ApertureIO/RenderEngine.hpp"
#include "ApertureIO/Logger.hpp"

namespace Aio
{
RenderEngine::RenderEngine(Device* pDevice, Context* pContext, FrameBuffer* pTarget)
{
    _pDevice = pDevice;
    _pContext = pContext;
    _pTarget = pTarget;

    _command = Command::CreateCommand(_pContext, _pDevice);
};

void RenderEngine::ExecuteFrame()
{
    _activeGraph->ExecuteGraph(this);
};

void RenderEngine::SetActive(const std::string& name)
{
    _activeGraph = _loadedGraphs.at(name).get();
};

void RenderEngine::LoadGraph(const std::string& name, UniquePtr<RenderGraph> renderGraph)
{
    _loadedGraphs.emplace(name, std::move(renderGraph));
    _loadedGraphs.at(name).get()->CompileGraph(this);
};

void RenderEngine::RemoveGraph(const std::string& name)
{
    if (_loadedGraphs.find(name) != _loadedGraphs.end())
    {
        if(_loadedGraphs.at(name).get() != _activeGraph)
        {
            _loadedGraphs.at(name).release();
            _loadedGraphs.erase(name);
        } 
        else
        {
            auto mgs = "Tried Removing Graph but was set as Active...";
            Logger::LogError(mgs);
        }
    }
};
/*
TODO: Not really sure if these functions should be passing around UniquePtr.
We only want to store them an unique ptr. Maybe it's fine like this....
*/
void RenderEngine::StoreBufferPtr(std::string name, UniquePtr<Buffer>pBuffer)
{
    if (_pBuffersMap.find(name) != _pBuffersMap.end())
    {
        auto ptr = _pBuffersMap.at(name).get();
        if (ptr == nullptr)
        {
            _pBuffersMap[name] = std::move(pBuffer);
        }
    }
    else
    {
        _pBuffersMap.emplace(name, std::move(pBuffer));
    }
};

void RenderEngine::StoreTexturePtr(std::string name, UniquePtr<Texture> pTexture)
{
    if (_pTexturesMap.find(name) != _pTexturesMap.end())
    {
        auto ptr = _pTexturesMap.at(name).get();
        if (ptr == nullptr)
        {
            _pTexturesMap[name] = std::move(pTexture);
        }
    }
    else
    {
        _pTexturesMap.emplace(name, std::move(pTexture));
    }
};

Buffer* RenderEngine::GetBufferPtr(std::string name)
{
    auto buffer = _pBuffersMap.at(name).get();
    if (!buffer)
    {
        auto msg = "Tried Accessing " + name + " but resource handle was a nullptr.";
        Logger::LogError(msg);
    }
    return buffer;
};

Texture* RenderEngine::GetTexturePtr(std::string name)
{
    auto texture = _pTexturesMap.at(name).get();
    if (!texture)
    {
        auto msg = "Tried Accessing " + name + " but texture handle was a nullptr.";
        Logger::LogError(msg);
    }
    return texture;
};

Context* RenderEngine::GetContextPtr()
{
    return _pContext;
};

Device* RenderEngine::GetDevicePtr()
{
    return _pDevice;
};

Command* RenderEngine::GetCommandPtr()
{
    return _command.get();
};

FrameBuffer* RenderEngine::GetTargetFrameBufferPtr()
{
    return _pTarget;
};

};