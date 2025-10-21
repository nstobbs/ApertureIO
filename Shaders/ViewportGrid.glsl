#shader Vertex
#version 440
#include "ApertureIO.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inUVs;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outUV;
layout (location = 2) out mat4 outView;
layout (location = 6) out mat4 outProj; 

SetUniformLayout(CameraU, { mat4 view;  mat4 projection; });

void main()
{
    // Handles
    uint cameraBufferHandle = uRenderPassHandles.handles[0];
    
    // Camera
    mat4 view = GetResource(CameraU, cameraBufferHandle).view;
    mat4 proj = GetResource(CameraU, cameraBufferHandle).projection;

    vec3 scaledPos = inPosition * 2.5f;

    gl_Position = proj * view * vec4(scaledPos, 1.0f);
    outPosition = gl_Position.xyz / gl_Position.w;
    outUV = inUVs;
    outView = view;
    outProj = proj;
}

#shader Fragment
#version 440
#include "ApertureIO.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inUV;
layout (location = 2) in mat4 inView;
layout (location = 6) in mat4 inProj;

layout (location = 0) out vec4 outColour;

void main()
{
    // ---------------- Grid ----------------  

    // LineColors
	vec4 cellColor = vec4(0.1f);
    vec4 subCellColor = vec4(0.01f);

    // Cell Size
	float cellSize = 0.1f;
	float halfCellSize = cellSize * 0.5f;

	float subCellSize = 0.02f;
	float halfSubCellSize = subCellSize * 0.5f;

	// Line Thickness
	float cellLineThickness = 0.01f;
	float subCellLineThickness = 0.1f;

	// Cell Coords
	vec2 cellCoords = mod(inUV.xy, cellSize) / cellSize;	
	vec2 subCellCoords = mod(inUV.xy, subCellSize) / subCellSize;
	
	// Cell Distance
	vec2 distanceToCell = abs(cellCoords - halfCellSize);
	vec2 distanceToSubCell = abs(subCellCoords - halfSubCellSize);

	// Adjusted Thickness
	vec2 d = fwidth(inUV.xy);
	vec2 adjustedCellLineThickness = 0.5 * (cellLineThickness + d);
	vec2 adjustedSubCellLineThickness = 0.5 * (subCellLineThickness + d);
	
	outColour = vec4(0.0f);
	if (distanceToSubCell.x < adjustedSubCellLineThickness.x ||
		distanceToSubCell.x < adjustedSubCellLineThickness.y ||
		distanceToSubCell.y < adjustedSubCellLineThickness.x ||
		distanceToSubCell.y < adjustedSubCellLineThickness.y)
	{
		outColour = subCellColor;
	};

	if (distanceToCell.x < adjustedCellLineThickness.x || 
		distanceToCell.x < adjustedCellLineThickness.y ||
		distanceToCell.y < adjustedCellLineThickness.x ||
		distanceToCell.y < adjustedCellLineThickness.y)
	{
		outColour = cellColor;
	};

    // ---------------- Falloff ----------------
    float radius = 0.5f;
    float falloff = 1.0f;
    vec2 center = vec2(0.5f, 0.5f);

    vec2 p = inUV.xy - center;
    float di = length(p);
    float alpha = 1.0f - smoothstep(radius - falloff, radius, di);

    outColour = outColour * alpha;
    outColour = outColour / 2.2f; 
}