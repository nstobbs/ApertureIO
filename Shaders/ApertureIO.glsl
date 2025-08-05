
// @filename ApertureIO.glsl
// @brief This file includes all of the marcos and stuff required
// to help with ApertureIO accessing data from the Bindless Buffers within Vulkan.  
// All Shaders need to have included this header to have access to the to the layouts.

#extension GL_EXT_nonuniform_qualifier : enable

// Global Descriptor Set:
#define BindlessDescriptorSet 0

// Binding for Uniform, Storage, Texture
#define BindlessUniformBinding 0
#define BindlessStorageBinding 1
#define BindlessTextureBinding 2

#define GetLayoutVariableName(Name) u##Name##Register

// Set Uniform Buffers Layout
#define SetUniformLayout(Name, Struct) \
    layout(set = BindlessDescriptorSet, binding = BindlessUniformBinding) \
        uniform Name Struct \
        GetLayoutVariableName(Name)[]

// Set Storage Buffers Layout
//TODO: Sort out later when I need compute

// Set Textures Layout
layout (set = BindlessDescriptorSet, binding = BindlessTextureBinding) uniform sampler2D uGlobalTextures[];

// Access a specific Resource
#define GetResource(Name, Index) \
    GetLayoutVariableName(Name)[Index]

// TODO: Add Stuff for the PushConstants