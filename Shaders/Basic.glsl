#shader Vertex
#version 440

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

void main()
{
    gl_Position = vec4(inPosition, 1.0f);
    outColor = inColor;
}

#shader Fragment
#version 440

layout (location = 0) in vec3 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    
    outColor = vec4(inColor, 1.0f);
    //outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
