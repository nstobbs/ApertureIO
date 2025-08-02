#shader Vertex
#version 440
// -------- TODO Need to Auto Create this Part !Start --------
#extension GL_EXT_nonuniform_qualifier : enable

// ---- Descriptor Uniform Bindings ----
layout(std140, set = 0, binding = 0) uniform UniformBuffersArray {
    float a;
} deviceUniformBuffers[1024];

// ---- Descriptor Texture Bindings ----
layout(set = 0, binding = 2) uniform sampler2D textures[1024];

// -------- TODO Need to Auto Create this Part !End --------

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inUv;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outUv;

void main()
{
    gl_Position = vec4(inPosition, 1.0f);
    outColor = inColor;
    outUv = inUv;
}

#shader Fragment
#version 440
// -------- TODO Need to Auto Create this Part !Start --------
#extension GL_EXT_nonuniform_qualifier : enable

// ---- Descriptor Uniform Bindings ----
layout(std140, set = 0, binding = 0) uniform UniformBuffersArray {
    float a;
} deviceUniformBuffers[1024];

// ---- Descriptor Texture Bindings ----
layout(set = 0, binding = 2) uniform sampler2D textures[1024];

// -------- TODO Need to Auto Create this Part !End --------


layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inUv;

layout(location = 0) out vec4 outColor;

void main()
{
    vec2 fixedUv = inUv.xy;

    //outColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    float test = float(deviceUniformBuffers[0].a);
    outColor = vec4(inColor, 1.0f);
    outColor = outColor * vec4(test, test, test, 1.0f); 
    outColor = outColor + texture(textures[0], fixedUv);
    outColor = vec4(inColor - test, 1.0f);
}
