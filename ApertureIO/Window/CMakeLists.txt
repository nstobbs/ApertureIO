add_library(Window STATIC 
        Window.cpp
)

target_include_directories(Window PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Window
    glfw
    
    Aio::Common
)

add_library(Aio::Window ALIAS Window)