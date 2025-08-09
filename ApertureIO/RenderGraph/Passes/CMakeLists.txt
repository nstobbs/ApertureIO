#add_library(Passes STATIC
#                RenderGraph.cpp
#)

#target_include_directories(Passes PUBLIC 
#        ${CMAKE_CURRENT_SOURCE_DIR}/include
#)

#target_link_libraries(Passes
#        Aio::Base
#        Aio::RenderGraph
#)

#add_library(Aio::Passes ALIAS Passes)