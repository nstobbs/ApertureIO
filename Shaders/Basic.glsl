#shader Vertex
#version 440
#include "ApertureIO.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inUv;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outUv;

SetUniformLayout(testUniform, { float a; });

void main()
{
    gl_Position = vec4(inPosition, 1.0f);
    outColor = inColor;
    outUv = inUv;
}

#shader Fragment
#version 440
#include "ApertureIO.glsl"


layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inUv;

layout(location = 0) out vec4 outColor;

SetUniformLayout(testUniform, { float a; });

void main()
{
    vec2 fixedUv = inUv.xy;
    float test = GetResource(testUniform, 0).a;

    //outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    outColor = vec4(inColor, 1.0f);
    //outColor = outColor + vec4(fixedUv.x, fixedUv.y, 0.0f, 1.0f);
    outColor = outColor * vec4(test, test, test, 1.0f); 
    outColor = outColor + texture(uGlobalTextures[0], fixedUv);

}
