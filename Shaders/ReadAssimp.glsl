#shader Vertex
#version 440
#include "ApertureIO.glsl"

layout (location = 0) in vec3 inPositions;
layout (location = 1) in vec3 inNormals;
layout (location = 2) in vec3 inUVs;

layout (location = 0) out vec3 outNormals;
layout (location = 1) out vec3 outUVs;

SetUniformLayout(Camera, { mat4 model;   mat4 view;  mat4 projection; });

void main()
{
    // Buffer Handles
    uint cameraBufferHandle = uRenderPassHandles.handles[0];

    mat4 model = GetResource(Camera, uRenderPassHandles.handles[0]).model; 
    mat4 view = GetResource(Camera, uRenderPassHandles.handles[0]).view;
    mat4 projection = GetResource(Camera, uRenderPassHandles.handles[0]).projection;

    gl_Position = projection * view * model * vec4(inPositions, 1.0f);

    outNormals = inNormals;
    outUVs = inUVs;
}

#shader Fragment
#version 440
#include "ApertureIO.glsl"


layout (location = 0) in vec3 inNormals;
layout (location = 1) in vec3 inUVs;

layout(location = 0) out vec4 outViewer;
layout(location = 1) out vec4 outNormals;

void main()
{
    // Texture Handles
    uint textureBufferHandle = uRenderPassHandles.handles[1];

    vec2 fixedUv = inUVs.xy;
    outViewer = texture(uGlobalTextures[textureBufferHandle], fixedUv);
    outNormals = vec4(inNormals, 1.0f);
}