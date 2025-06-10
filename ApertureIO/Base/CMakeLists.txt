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

add_library(Aio::Base ALIAS Base)