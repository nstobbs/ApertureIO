#pragma once
#include "ApertureIO/RenderPass.hpp"

#include "ApertureIO/Pointers.hpp"

#include <string>

namespace Aio
{

class RenderPass;

class RenderPassFactory
{
public:
    static UniquePtr<RenderPass> CreateRenderPass(const std::string& name);
};

};

