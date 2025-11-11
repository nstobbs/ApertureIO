add_library(RenderGraph STATIC
                Knob.cpp
                Port.cpp
                RenderPass.cpp
                RenderPassFactory.cpp
                RenderGraph.cpp
                RenderEngine.cpp
)

target_include_directories(RenderGraph PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RenderGraph
        Qt6::Core
        Aio::Base
        Aio::Passes
        Aio::Common
)

add_library(Aio::RenderGraph ALIAS RenderGraph)