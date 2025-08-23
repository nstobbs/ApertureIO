add_library(RenderGraph STATIC
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
        Aio::Base
        Aio::Passes
)

add_library(Aio::RenderGraph ALIAS RenderGraph)