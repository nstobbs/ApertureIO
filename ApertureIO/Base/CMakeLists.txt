add_library(Base STATIC 
        BufferLayout.cpp
        Buffers.cpp
        Command.cpp
        Context.cpp
        Device.cpp
        FrameBuffer.cpp
        RenderContext.cpp
        Shader.cpp
)

target_include_directories(Base PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Base
        efsw::efsw
        glm::glm
        boost::boost
        
        Aio::Common
        Aio::Vulkan
        Aio::Window
)

add_library(Aio::Base ALIAS Base)