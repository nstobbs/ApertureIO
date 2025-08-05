#pragma once

namespace Aio
{
enum RenderPassType
{
    Graphics = 0,
    Compute = 1,
    Transfer = 2
};

class RenderPass
{
public:
    virtual void ExecutePass() = 0; /* Sumbits the Pass for Rendering */

protected:
    RenderPassType _type;
    RenderPass* _Next;
};
};