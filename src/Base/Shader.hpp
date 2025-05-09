#ifndef H_SHADER
#define H_SHADER

#include "Context.hpp"

namespace ApertureIO {

class Shader
{
    public:
    static Shader* CreateShader(Context* context);

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    private:
    char* _name = "";
};

}; // End of ApertureIO Namespace 

#endif