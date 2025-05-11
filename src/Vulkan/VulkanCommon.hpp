#ifndef H_VULKANCOMMON
#define H_VULKANCOMMON

#include <volk.h>
#include "../../deps/vk-bootstrap/src/VkBootstrap.h"
#include "../../deps/VulkanMemoryAllocator/include/vk_mem_alloc.h"


#include <string>

std::string VkResultToString(VkResult result);
void VK_ASSERT(VkResult result, VkResult expect, std::string message);

#endif