#pragma once
#include "ApertureIO/RenderPass.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIo/Buffers.hpp"
#include "ApertureIO/FrameBuffer.hpp"

#include <string>
#include <unordered_map>

namespace Aio
{

/* The RenderGraph is that main system for rendering these passes.
The RenderGraph will check all of the Passes dependency on resources.
Like what buffers / FrameBuffers it wants to Read or Write to.
It will then start allocating Aio Resources required to Process this graph. 
!Not includeing the SwapChain Framebuffer!
Once everything on the device is ready. RenderGraph will begin with 
submiting jobs to the gpu to process.

*/

class RenderGraph
{
public:
    RenderGraph(WeakPtr<Device> pDevice, WeakPtr<Context> pContext, WeakPtr<FrameBuffer> pTargetSwapChain);
    void AppendRenderPass(WeakPtr<RenderPass> pass);
    
    void CompileGraph(); /* Preps the Graph for Rendering, allocate required resources. */
    void RenderFrame(); /* Renders Current Frame*/

    void StoreBufferPtr(std::string name, SharedPtr<Buffer> pBuffer);
    void StoreTexturePtr(std::string name, SharedPtr<Texture> pTexture);

    SharedPtr<Buffer> GetBuffer(std::string name);
    SharedPtr<Texture> GetTexture(std::string name);
    SharedPtr<Context> GetContext();
    SharedPtr<Device> GetDevice();
    SharedPtr<FrameBuffer> GetTargetFrameBuffer();

private:
    std::vector<SharedPtr<RenderPass>> sortGraph(); /* Does Topological Sort on the DAG */

    WeakPtr<Device> _pDevice;
    WeakPtr<Context> _pContext;
    WeakPtr<FrameBuffer> _pTargetSwapChain;

    std::unordered_map<std::string, SharedPtr<Buffer>> _pBuffersMap;
    std::unordered_map<std::string, SharedPtr<Texture>> _pTexturesMap;

    std::vector<SharedPtr<RenderPass>> _renderPasses;
    size_t _hash; /* A Combined Hash Of all of the RenderContext hashes within the graph.*/
};

};