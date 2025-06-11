#ifndef H_VULKANCOMMON
#define H_VULKANCOMMON

#include <volk.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>

#include <string> //TODO this shouldnt be here
 
std::string VkResultToString(VkResult result);
void VK_ASSERT(VkResult result, VkResult expect, std::string message);

#endif