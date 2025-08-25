#shader Vertex
#version 440
#include "ApertureIO.glsl"

layout (location = 0) in vec3 inPositions;
layout (location = 1) in vec3 inNormals;
layout (location = 2) in vec3 inUVs;

layout (location = 0) out vec3 outNormals;
layout (location = 1) out vec3 outUVs;

SetUniformLayout(Camera, { mat4 view;  mat4 projection; });

void main()
{
    mat4 view = GetResource(Camera, 0).view;
    mat4 projection = GetResource(Camera, 0).projection;

    gl_Position = projection * view * vec4(inPositions, 1.0f);

    outNormals = inNormals;
    outUVs = inUVs;
}

#shader Fragment
#version 440
#include "ApertureIO.glsl"


layout (location = 0) in vec3 inNormals;
layout (location = 1) in vec3 inUVs;

layout(location = 0) out vec4 outColor;

void main()
{
    vec2 fixedUv = inUVs.xy;
    outColor = texture(uGlobalTextures[0], fixedUv);
}