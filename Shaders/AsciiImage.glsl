#shader Compute
#version 440
#include "ApertureIO.glsl"

SetReadOnlyStorageImageLayout(readFrameBuffer)
SetWriteOnlyStorageImageLayout(targetFrameBuffer)
SetUniformLayout(Settings, { uint mapSize;  uint charCount; float brightness; bool matrixMode; });

layout(local_size_x = 16, local_size_y = 16) in;

void main()
{
    // Buffer Handles
    uint settingsBufferHandle = uRenderPassHandles.handles[0];
    //uint textureHandle = uRenderPassHandles.handles[1];
    uint inputImageHandle = uRenderPassHandles.handles[10];
    uint targetImageHandle = uRenderPassHandles.handles[11];
    uint charTextureHandles[9] = uint[9](uRenderPassHandles.handles[1],
                                         uRenderPassHandles.handles[2],
                                         uRenderPassHandles.handles[3],
                                         uRenderPassHandles.handles[4],
                                         uRenderPassHandles.handles[5],
                                         uRenderPassHandles.handles[6],
                                         uRenderPassHandles.handles[7],
                                         uRenderPassHandles.handles[8],
                                         uRenderPassHandles.handles[9]);

    // AsciiImage Settings
    uint mapSize = GetResource(Settings, settingsBufferHandle).mapSize;
    uint charCount = GetResource(Settings, settingsBufferHandle).charCount;
    float brightness = GetResource(Settings, settingsBufferHandle).brightness;
    bool matrixMode = GetResource(Settings, settingsBufferHandle).matrixMode;


    brightness = 2.0f;

    // Desaturation Coefficients // Rec709
    vec3 desatCoe = vec3(0.2126f, 0.7152f, 0.0722f);

    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(GetResource(readFrameBuffer, inputImageHandle));

    // Make Sure We are Within Image Range
    if (gid.x >= dims.x || gid.y >= dims.y) return;

    // Create Grid Uv Patten
    vec2 uv;
    uv.x = mod(float(gid.x), float(mapSize)) / float(mapSize);
    uv.y = mod(float(gid.y), float(mapSize)) / float(mapSize);

    // Pixelate Input Image
    ivec2 samplePos;
    samplePos.x = int(mod(gid.x, mapSize));
    samplePos.y = int(mod(gid.y, mapSize));
    samplePos.x = gid.x + int(floor(mapSize / 2)) - samplePos.x;
    samplePos.y = gid.y + int(floor(mapSize / 2)) - samplePos.y;
    vec4 pixelatedColor = imageLoad(GetResource(readFrameBuffer, inputImageHandle), samplePos);

    // Increase Brightness
    pixelatedColor = pixelatedColor * brightness;

    // Desaturate
    float luma  = pixelatedColor.x * desatCoe.x
                + pixelatedColor.y * desatCoe.y
                + pixelatedColor.z * desatCoe.z;
    
    // Quantization
    luma = clamp(luma, 0.0f, 0.9f);

    // Ascii Index Texture Map
    uint index = uint(float(luma) * float(charCount));
    //float normIndex = ((index - 0.0f) / (float(charCount) - 0.0f));
    //uv.x = ((float(gid.x) - 0.0f) / (float(dims.x) - 0.0f));
    //uv.y = ((float(gid.y) - 0.0f) / (float(dims.y) - 0.0f));
    //uv.x = 0.033
    //uint newIndex = uint(index);

    vec4 ascii = texture(uGlobalTextures[charTextureHandles[index]], uv);

    // PreviewSlice
    vec4 outputColor;
    vec2 dimsHalf = dims / 2;
    if (gid.y > dimsHalf.y)
    {
        outputColor = imageLoad(GetResource(readFrameBuffer, inputImageHandle), gid);
    }
    else
    {
        outputColor = ascii * pixelatedColor;
    }
    
    outputColor = ascii * pixelatedColor;
    imageStore(GetResource(targetFrameBuffer, targetImageHandle), gid, outputColor);
}