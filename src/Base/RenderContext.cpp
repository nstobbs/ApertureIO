#include "RenderContext.hpp"
#include <boost/functional/hash.hpp>

namespace Aio {

size_t RenderContext::GetHash()
{
    if (!_valid)
    {
        size_t seed = 0;
        boost::hash_combine(seed, _VertexBuffer);
        boost::hash_combine(seed, _IndexBuffer);
        boost::hash_combine(seed, _StorageBuffer);
        boost::hash_combine(seed, _TargetFrameBuffer);
        boost::hash_combine(seed, _Shader);
        
        _valid = true;
        return seed;
    };
    
    return _hash;
};

};