add_library(Window STATIC
    WindowGLFWImpl.cpp
    #WindowQtImpl.cpp
)

target_include_directories(Window PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Window
    Qt6::Core
    Qt6::Gui
    glfw
    Aio::Common
    Aio::Base
    Aio::Vulkan
)

add_library(TestApplication:Window ALIAS Window) 