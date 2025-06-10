#include "Command.hpp"
#include "../Vulkan/VulkanCommand.hpp"

namespace Aio {

Command* Command::CreateCommand(Context* context, Device* device)
{
    auto API = context->getRendererAPI();
    switch (API)
    {
        case eVulkan:
            return new VulkanCommand(context, device);
    };
};

}; // End of Aio namespace