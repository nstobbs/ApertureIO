add_executable(TestApplication
        main.cpp
)

target_link_libraries(TestApplication PUBLIC
        Aio::Base
        Aio::Common
        Aio::RenderGraph
        Aio::Passes
        Aio::Vulkan
        TestApplication:Window
)