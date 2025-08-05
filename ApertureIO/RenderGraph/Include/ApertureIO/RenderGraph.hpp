#pragma once
#include "ApertureIO/RenderPass.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/FrameBuffer.hpp"

/* Note: This is the start of planning of the renderGraph System. */

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
    void AppendPass(RenderPass* pass);
    void RemovePass(RenderPass* pass);
    
    void UpdateGraph(); /* Preps the Graph for Rendering, allocate required resources*/
    void ProcessGraph(); /* Renders the Graph*/

private:
    bool isValid(); /* Checks the Graph is Still Valid for Rendering */

    Device* _pDevice;
    Context* _pContext;
    FrameBuffer* _pTargetSwapChain;

    RenderPass* _rootPass;
    size_t _hash;
};

};