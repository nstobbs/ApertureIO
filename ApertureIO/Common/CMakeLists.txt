add_library(Common STATIC 
        FileIO.cpp
        Logger.cpp
        GenericFileManager.cpp
)

target_include_directories(Common PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Common
        efsw::efsw
        Aio::Base
)

add_library(Aio::Common ALIAS Common)