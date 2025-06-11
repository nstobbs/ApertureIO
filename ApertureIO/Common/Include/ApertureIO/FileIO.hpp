#pragma once

#include <vector>
#include <string>

namespace Aio 
{

enum SourceFileType
{
    NoneShader = -1,
    VertexShader = 0,
    FragmentShader = 1,
    ComputeShader = 2
    ,
};

class FileIO
{
    public:
    static std::string ReadSourceFile(const std::string& filepath);
    static std::string SplitOutShader(std::string& sourceCode, SourceFileType shaderType);
};

};