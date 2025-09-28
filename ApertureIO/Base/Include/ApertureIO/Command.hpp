#pragma once

#include "ApertureIO/Pointers.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"

namespace Aio {

class Device;
class RenderContext;

class Command
{
    public:
        static UniquePtr<Command> CreateCommand(Context* context, Device* device);

        virtual void BeginFrame(RenderContext& renderContext) = 0;
        virtual void EndFrame(RenderContext& copyFrameBuffer, RenderContext& renderContext) = 0; /* TODO: seems like bad design if i have to pass back the RenderPass */
        virtual void Draw(RenderContext& renderContext) = 0;
        virtual void DispatchCompute(RenderContext& renderContext, uint32_t X, uint32_t Y, uint32_t Z) = 0;
        //virtual void DrawInstance(RenderContext& renderContext) = 0;
        //virtual void Clear(RenderContext& renderContext) = 0; /* Clears All of the FrameBuffers in the renderContext */ 
};

};