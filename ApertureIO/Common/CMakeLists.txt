add_library(Common STATIC 
        FileIO.cpp
        Logger.cpp
)

add_library(Aio::Common ALIAS Common)