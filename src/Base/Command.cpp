#include "Command.hpp"
#include "../Vulkan/VulkanCommand.hpp"

namespace ApertureIO {

Command* Command::CreateCommand(Context* context)
{
    auto API = context->getRendererAPI();
    switch (API)
    {
        case eVulkan:
            return new VulkanCommand(context);
    };
};

}; // End of ApertureIO namespace