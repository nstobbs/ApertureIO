#shader Compute
#version 440
#include "ApertureIO.glsl"

SetWriteOnlyStorageImageLayout(targetFrameBuffer)
SetReadOnlyStorageImageLayout(readFrameBuffer)

layout(local_size_x = 16, local_size_y = 16) in;

//SetUniformLayout(gLights, { vec3 pos;  vec3 color; uint intensity; });
//SetUniformLayout(gMetadata, { uint width; uint height; uint numOfLights; });

void main()
{
    // Buffer Handles
    uint metadataBufferHandle = uRenderPassHandles.handles[0];
    uint gLightsBufferHandle = uRenderPassHandles.handles[1];
    uint gColorImageHandle = uRenderPassHandles.handles[2];
    uint gNormalsImageHandle = uRenderPassHandles.handles[3];
    uint targetImageHandle = uRenderPassHandles.handles[4];

    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(GetResource(readFrameBuffer, targetImageHandle));

    // Make Sure We are Within Image Range
    if (gid.x >= dims.x || gid.y >= dims.y) return;
    float xColor = float(gid.x) / float (dims.x);
    float yColor = float(gid.y) / float (dims.y);

    vec4 testColor = imageLoad(GetResource(readFrameBuffer, gColorImageHandle), gid);

    vec4 cColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
    vec4 nColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
    vec4 tColor = vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue

    // Keyer
    if (testColor == vec4(0.0f, 0.0f, 0.0f, 1.0f))
    {
        //testColor = vec4(xColor, yColor, 0.0f, 1.0f);
        testColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    //imageStore(GetResource(targetFrameBuffer, gColorImageHandle), gid, cColor);
    //imageStore(GetResource(targetFrameBuffer, gNormalsImageHandle), gid, nColor);
    imageStore(GetResource(targetFrameBuffer, targetImageHandle), gid, testColor);
}