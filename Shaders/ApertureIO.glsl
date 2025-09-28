
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
#define BindlessStorageImageBinding 3

#define GetLayoutVariableName(Name) u##Name##Register

// Set Uniform Buffers Layout
#define SetUniformLayout(Name, Struct) \
    layout(set = BindlessDescriptorSet, binding = BindlessUniformBinding) \
        uniform Name Struct \
        GetLayoutVariableName(Name)[]

// Set Storage Buffers Layout
#define SetStorageLayout(Name, Struct) \
    layout(set = BindlessDescriptorSet, binding = BindlessStorageBinding) \
        buffer Name Struct \
        GetLayoutVariableName(Name)[]

#define SetWriteOnlyStorageImageLayout(Name) \
    layout(set = BindlessDescriptorSet, binding = BindlessStorageImageBinding, rgba8) uniform writeonly image2D GetLayoutVariableName(Name)[];

#define SetReadOnlyStorageImageLayout(Name) \
    layout(set = BindlessDescriptorSet, binding = BindlessStorageImageBinding, rgba8) uniform readonly image2D GetLayoutVariableName(Name)[];
        
// Set Textures Layout
layout (set = BindlessDescriptorSet, binding = BindlessTextureBinding) uniform sampler2D uGlobalTextures[];

// Access a specific Resource
#define GetResource(Name, Index) \
    GetLayoutVariableName(Name)[Index]

// Set PushConstants
// Order Of Handles is bases of the Order placed within Each Group.
// At the time of binding to a renderContext.
// Group Order = Uniforms -> Storages -> Textures -> FrameBuffers
// The Target FrameBuffer will the the Last Handles in the Array.
layout(push_constant) uniform  PushConstants { uint handles[64]; } uRenderPassHandles;

