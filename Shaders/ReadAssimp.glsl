#shader Vertex
#version 440
#include "ApertureIO.glsl"

struct Transforms
{
    mat4 transforms[32];
    uint meshCount;
};

layout (location = 0) in vec3 inPositions;
layout (location = 1) in vec3 inNormals;
layout (location = 2) in vec3 inUVs;

layout (location = 0) out vec3 outNormals;
layout (location = 1) out vec3 outPosition;
layout (location = 2) out vec3 outUVs;

SetUniformLayout(CameraU, { mat4 view;  mat4 projection; });
SetUniformLayout(TransformU, { Transforms info; });

void main()
{
    // Handles
    uint cameraBufferHandle = uRenderPassHandles.handles[0];
    uint transformBufferHandle = uRenderPassHandles.handles[1];

    uint meshCount = GetResource(TransformU, transformBufferHandle).info.meshCount;
    mat4 view = GetResource(CameraU, cameraBufferHandle).view;
    mat4 projection = GetResource(CameraU, cameraBufferHandle).projection;

    for (uint i = 0; i < meshCount; i++)
    {
        mat4 model = GetResource(TransformU, transformBufferHandle).info.transforms[i];

        gl_Position = projection * view * model * vec4(inPositions, 1.0f);
        outPosition = gl_Position.xyz / gl_Position.w;
        outNormals = inNormals;
        outUVs = inUVs;
    };
}

#shader Fragment
#version 440
#include "ApertureIO.glsl"


layout (location = 0) in vec3 inNormals;
layout (location = 1) in vec3 inPositions;
layout (location = 2) in vec3 inUVs;

layout(location = 0) out vec4 outColour;
layout(location = 1) out vec4 outPosition;
layout(location = 2) out vec4 outNormals;

void main()
{
    // Texture Handles
    uint textureBufferHandle = uRenderPassHandles.handles[2];

    vec2 fixedUv = inUVs.xy;
    outColour = texture(uGlobalTextures[textureBufferHandle], fixedUv);
    outPosition = vec4(inPositions, 1.0f);
    outNormals = vec4(inNormals, 1.0f);
}