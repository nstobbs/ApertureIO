#ifndef H_FILEIO
#define H_FILEIO

#include <vector>
#include <string>

namespace Aio 
{

enum SourceFileType
{
    NoneShader = -1,
    VertexShader = 0,
    FragmentShader = 1,
    ComputeShader = 3,
};

class FileIO
{
    public:
    static std::string FileIO::ReadSourceFile(const std::string& filepath);
    static std::string SplitOutShader(std::string& sourceCode, SourceFileType shaderType);
};

};

#endif