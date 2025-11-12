add_library(Application STATIC
    Application.cpp
)

target_include_directories(Application PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Application
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Aio::Common
    Aio::Base
    Aio::Vulkan
)

add_library(ApertureIOEditor:Application ALIAS Application) 