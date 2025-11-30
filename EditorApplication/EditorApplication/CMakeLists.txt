add_executable(EditorApplication
        main.cpp
)

target_link_libraries(EditorApplication PUBLIC
        Qt6::Gui
        Aio::Base
        Aio::Common
        Aio::RenderGraph
        Aio::Passes
        Aio::Vulkan
        Application
)