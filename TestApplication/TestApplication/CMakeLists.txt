add_executable(TestApplication
        main.cpp
)

target_link_libraries(TestApplication PUBLIC
        Base
        Common
        Window
        Vulkan
)