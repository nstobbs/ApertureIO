add_library(Base STATIC 
        BufferLayout.cpp
        Buffers.cpp
        Command.cpp
        Context.cpp
        Device.cpp
        FrameBuffer.cpp
        RenderContext.cpp
        Shader.cpp
        Texture.cpp
)

target_include_directories(Base PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Base
        glm::glm
        boost::boost
        stb::stb
        
        Aio::Common
        Aio::Vulkan
)

add_library(Aio::Base ALIAS Base)