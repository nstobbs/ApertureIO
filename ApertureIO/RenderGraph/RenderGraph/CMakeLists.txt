add_library(RenderGraph STATIC
                RenderGraph.cpp
                RenderPass.cpp
)

target_include_directories(RenderGraph PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RenderGraph
        Aio::Base
)

add_library(Aio::RenderGraph ALIAS RenderGraph)