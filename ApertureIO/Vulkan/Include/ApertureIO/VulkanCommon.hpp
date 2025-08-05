#pragma once

#include <volk.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <shaderc/shaderc.hpp>

#include <string>
 
std::string VkResultToString(VkResult result);
void VK_ASSERT(VkResult result, VkResult expect, std::string message);