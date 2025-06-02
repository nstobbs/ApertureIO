#shader Vertex
#version 440

layout (location = 0) in vec3 inPosition;

void main()
{
    gl_Position = vec4(inPosition, 1.0f);
}

#shader Fragment
#version 440

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(inColor, 1.0f);
}