add_library(Passes STATIC
                BasicRenderPass.cpp
                ReadAssimp.cpp
                PhongLighting.cpp
                AsciiImage.cpp
                CameraManager.cpp
                ViewportGrid.cpp
                MergeLayers.cpp
)

target_include_directories(Passes PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Passes
        Aio::Base
        Aio::RenderGraph
        assimp::assimp
)

add_library(Aio::Passes ALIAS Passes)