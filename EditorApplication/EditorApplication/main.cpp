#include "Application/Application.hpp"

int main(int argc, char** argv) {
    
    Aio::ApplicationSettings Settings(argc, argv);
    Aio::Application App(Settings);
    return 0;
};