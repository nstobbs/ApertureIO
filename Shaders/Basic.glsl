#shader Vertex
#version 440
// -------- TODO Need to Auto Create this Part !Start --------
#extension GL_EXT_nonuniform_qualifier : enable

// ---- Descriptor Bindings ----
layout(std140, set = 0, binding = 0) uniform UniformBuffersArray {
    float a;
} deviceUniformBuffers[1024];
// -------- TODO Need to Auto Create this Part !End --------

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

void main()
{
    gl_Position = vec4(inPosition, 1.0f);
    outColor = inColor;
}

#shader Fragment
#version 440
// -------- TODO Need to Auto Create this Part !Start --------
#extension GL_EXT_nonuniform_qualifier : enable

// ---- Descriptor Bindings ----
layout(std140, set = 0, binding = 0) uniform UniformBuffersArray {
    float a;
} deviceUniformBuffers[1024];
// -------- TODO Need to Auto Create this Part !End --------


layout (location = 0) in vec3 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    
    //outColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    float test = float(deviceUniformBuffers[0].a);
    outColor = vec4(test, 1.0f, 0.0f, 1.0f);
    //outColor = vec4(inColor - test, 1.0f);
}
