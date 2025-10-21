add_executable(TestApplication
        main.cpp
)

target_link_libraries(TestApplication PUBLIC
        Qt6::Gui
        Aio::Base
        Aio::Common
        Aio::RenderGraph
        Aio::Passes
        Aio::Vulkan
        TestApplication:Window
)