#shader Compute
#version 440
#include "ApertureIO.glsl"

SetWriteOnlyStorageImageLayout(outputFrameBuffer)
SetReadOnlyStorageImageLayout(inputFrameBuffer)

layout(local_size_x = 16, local_size_y = 16) in;

void main()
{
    // Image Buffer Handles
    uint imageALayerBufferHandles = uRenderPassHandles.handles[0];
    uint imageADepthBufferHandles = uRenderPassHandles.handles[0];
    
    uint imageBLayerBufferHandles = uRenderPassHandles.handles[1];
    uint imageBDepthBufferHandles = uRenderPassHandles.handles[0];

    uint targetLayerBufferHandles = uRenderPassHandles.handles[2];
    uint targetDepthBufferHandles = uRenderPassHandles.handles[2];

    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(GetResource(inputFrameBuffer, imageBBufferHandles));

    // Make Sure We are Within Image Range
    if (gid.x >= dims.x || gid.y >= dims.y) return;
    
    // Merge A Over B based of Depth
    vec4 outColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Write Out Result 
    imageStore(GetResource(targetLayerBufferHandles, targetImageHandle), gid, outColour);
    imageStore(GetResource(targetDepthBufferHandles, targetImageHandle), gid, outColour);
}