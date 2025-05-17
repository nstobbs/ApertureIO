#ifndef H_FILEIO
#define H_FILEIO

#include <vector>
#include <string>

namespace Aio 
{

class FileIO
{
    public:
    static std::vector<char> ReadSourceFile(const std::string& filepath);
    static std::vector<std::string> SplitOutShader(std::vector<char>& sourceCode, std::string shaderType);
};

};

#endif