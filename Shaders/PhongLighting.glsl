#shader Compute
#version 440
#include "ApertureIO.glsl"

struct Light
{
    vec4 position;
    vec4 colour;
    float intensity;
    float _pad1;
    float _pad2;
    float _pad3;
};

SetWriteOnlyStorageImageLayout(targetFrameBuffer)
SetReadOnlyStorageImageLayout(readFrameBuffer)
SetUniformLayout(Camera, {mat4 view;  mat4 projection;});
SetUniformLayout(Lights, {  Light lights[128]; });
SetUniformLayout(LightingInfo, {uint numOfLights;   float specular;  float diffuse;    float ambient;    float shininess;});

layout(local_size_x = 16, local_size_y = 16) in;

void main()
{
    // Uniform Buffer Handles
    uint cameraBufferHandle = uRenderPassHandles.handles[0];
    uint gLightsBufferHandle = uRenderPassHandles.handles[1];
    uint metadataBufferHandle = uRenderPassHandles.handles[2];
    // gBuffer Handles
    uint gColorImageHandle = uRenderPassHandles.handles[3];
    uint gPositionsImageHandle = uRenderPassHandles.handles[4];
    uint gNormalsImageHandle = uRenderPassHandles.handles[5];
    // Target Layer Handle 
    uint targetImageHandle = uRenderPassHandles.handles[6];

    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(GetResource(readFrameBuffer, targetImageHandle));

    // Make Sure We are Within Image Range
    if (gid.x >= dims.x || gid.y >= dims.y) return;

    vec4 colour = imageLoad(GetResource(readFrameBuffer, gColorImageHandle), gid);
    vec4 position = normalize(imageLoad(GetResource(readFrameBuffer, gPositionsImageHandle), gid));
    vec4 normal = normalize(imageLoad(GetResource(readFrameBuffer, gNormalsImageHandle), gid));

    uint lightCount = GetResource(LightingInfo, metadataBufferHandle).numOfLights;
    //float specular = GetResource(LightingInfo, metadataBufferHandle).specular;
    //float diffuse = GetResource(LightingInfo, metadataBufferHandle).diffuse;
    //float ambient = GetResource(LightingInfo, metadataBufferHandle).ambient;
    //float shininess = GetResource(LightingInfo, metadataBufferHandle).shininess;

    float specular = 0.5f;
    float diffuse = 1.0f;
    float ambient = 0.1f;
    float shininess = 1.0f;
    
    //TODO: Should move this off the GPU onto the CPU instead.
    mat4 cameraView = GetResource(Camera, cameraBufferHandle).view;
    vec3 cameraPosition = normalize(inverse(cameraView) * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
    vec3 V = normalize(cameraPosition - position.xyz);

    vec4 pixelColour = vec4(0.0f);

    //lightCount = 1;
    for (uint i = 0; i < lightCount; i++)
    {
        vec3 lightPos =  GetResource(Lights, gLightsBufferHandle).lights[i].position.xyz;
        vec3 lightColour =  GetResource(Lights, gLightsBufferHandle).lights[i].colour.xyz;
        float lightIntensity =  GetResource(Lights, gLightsBufferHandle).lights[i].intensity;

        //lightPos = vec3(0.0f, 1.0f, 0.0f);
        //lightColour = vec3(1.0f, 1.0f, 1.0f);

        lightIntensity = 0.5f;
        vec4 combinedLight = vec4((lightColour * lightIntensity), 1.0f);

        // Surface to Light Source
        vec3 lightDirection = normalize(lightPos - position.xyz);

        // Diffuse
        float cosAngIncidence = dot(lightDirection, normal.xyz);
        cosAngIncidence = max(0.0, cosAngIncidence);
        vec4 diffuseLighting = diffuse * cosAngIncidence * combinedLight;

        // Specular
        vec3 reflectedLight = reflect(-lightDirection, normal.xyz);
        float phongTerm = dot(V, reflectedLight);
        phongTerm = max(0.0f, phongTerm);
        phongTerm = pow(phongTerm, shininess);
        vec4 specularLighting = specular * phongTerm * combinedLight;

        // Final
        pixelColour = pixelColour + diffuseLighting + specularLighting;
    };

    // Ambient
    vec4 ambientLighting = vec4(0.5f, 0.5f, 0.5f, 0.0f) * ambient;
    pixelColour = pixelColour + ambientLighting;  
    pixelColour = vec4((pixelColour.xyz * colour.xyz), 1.0f);

    imageStore(GetResource(targetFrameBuffer, targetImageHandle), gid, pixelColour);
}