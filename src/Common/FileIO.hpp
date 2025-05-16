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
};

};

#endif