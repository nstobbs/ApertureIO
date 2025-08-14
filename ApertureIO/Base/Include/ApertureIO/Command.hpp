#pragma once

#include "ApertureIO/Pointers.hpp"

#include "ApertureIO/Device.hpp"
#include "ApertureIO/Context.hpp"
#include "ApertureIO/RenderContext.hpp"

// Can

namespace Aio {

class Device;
class RenderContext;

class Command
{
    public:
        static SharedPtr<Command> CreateCommand(WeakPtr<Context> context, WeakPtr<Device> device);

        virtual void BeginFrame(RenderContext& renderContext) = 0;
        virtual void EndFrame(RenderContext& renderContext) = 0;
        virtual void Draw(RenderContext& renderContext) = 0;
        //virtual void DrawInstance(RenderContext& renderContext) = 0;
        //virtual void DispatchCompute(RenderContext& renderContext) = 0;
        //virtual void Clear(RenderContext& renderContext) = 0; /* Clears All of the FrameBuffers in the renderContext */ 
};

};