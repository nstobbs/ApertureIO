add_library(Common STATIC 
        FileIO.cpp
        Logger.cpp
)

target_include_directories(Common PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Common

)

add_library(Aio::Common ALIAS Common)