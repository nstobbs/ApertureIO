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
    RenderGraph(Device* pDevice, Context* pContext, FrameBuffer* pTargetSwapChain);
    void AppendRenderPass(RenderPass* pass);
    
    void CompileGraph(); /* Preps the Graph for Rendering, allocate required resources. */
    void RenderFrame(); /* Renders Current Frame*/

    void StoreBufferPtr(std::string name, Buffer* pBuffer);
    void StoreTexturePtr(std::string name, Texture* pTexture);

    Buffer* GetBufferPtr(std::string name);
    Texture* GetTexturePtr(std::string name);
    Context* GetContextPtr();
    Device* GetDevicePtr();
    FrameBuffer* GetTargetFrameBufferPtr();

private:
    bool isValid(); /* Checks the Graph is Still Valid for Rendering */
    std::vector<RenderPass*> sortGraphTaskOrder(); /* Returns an vector<RenderPass*> in order for tasking. */

    Device* _pDevice;
    Context* _pContext;
    FrameBuffer* _pTargetSwapChain;

    std::unordered_map<Buffer*, std::string> _pBuffersMap;
    std::unordered_map<Texture*, std::string> _pTexturesMap;

    std::vector<RenderPass*> _renderPasses;
    size_t _hash; /* A Combined Hash Of all of the RenderContext hashes within the graph.*/
};

};