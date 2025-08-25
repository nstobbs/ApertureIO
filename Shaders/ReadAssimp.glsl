#shader Vertex
#version 440
#include "ApertureIO.glsl"

layout (location = 0) in vec3 inPositions;
layout (location = 1) in vec3 inNormals;
layout (location = 2) in vec3 inUVs;

layout (location = 0) out vec3 outNormals;
layout (location = 1) out vec3 outUVs;

SetUniformLayout(Camera, { mat4 model;  mat4 view;  mat4 projection; });

void main()
{
    gl_Position = vec4(inPositions, 1.0f);
    outNormals = inNormals;
    outUVs = inUVs;
}

#shader Fragment
#version 440
#include "ApertureIO.glsl"


layout (location = 0) in vec3 inNormals;
layout (location = 1) in vec3 inUVs;

layout(location = 0) out vec4 outColor;

SetUniformLayout(Camera, { mat4 model;  mat4 view;  mat4 projection; });

void main()
{
    vec2 fixedUv = inUVs.xy;
    mat4 model = GetResource(Camera, 0).model;
    mat4 view = GetResource(Camera, 0).view;
    mat4 projection = GetResource(Camera, 0).projection;

    outColor = texture(uGlobalTextures[0], fixedUv);
}