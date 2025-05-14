#include "Command.hpp"
#include "../Vulkan/VulkanCommand.hpp"

namespace Aio {

Command* Command::CreateCommand(Context* context)
{
    auto API = context->getRendererAPI();
    switch (API)
    {
        case eVulkan:
            return new VulkanCommand(context);
    };
};

}; // End of Aio namespace