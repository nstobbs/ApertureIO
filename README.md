# ApertureIO

#### What is ApertureIO?

ApertureIO is a C++ abstraction layer over Vulkan. For prototyping and messing around with
GPU rendering and computation for developing graphics based applications.

### Main Features:

* Shader Hot Reloading - Currently only on GLSL Shaders but planning of supporting hot reloading across the library like textures.

![](Misc/ShaderHotReloading.gif)

### Roadmap:

* ShaderGraph System - Shader Graph system that handles the processing
  and rendering of graphs built with user defined passes. User should be
  able to define passes like Compute Passes and Graphic Passes that contain
  define steps of the rendering process with In and Out data points.
  These passes can be added to a ShaderGraph to define the order to process
  these passes. The system should handle all of the object allocation
  required and dispatch tasks to queue effectively.
* Timeline System - For animations stuff...
* Cross-Platform - Support for Linux, MacOS and Windows.
* Multiple Window Support - Output to multiple different surfaces at once.
* Headless Rendering - Allow support later for rendering to a file.

### Build:

###### Building on Windows:

```
git clone git@github.com:conan-io/conan-center-index.git
cd ./conan-center-index/recipes/efsw/all
conan create . --version=1.4.1 --settings=build_type=Debug --build=missing

cd ../../boost/all/
conan create . --version=1.88.0 --settings=build_type=Debug --build=missing

cd ../../../../
git clone git@github.com:nstobbs/ApertureIO.git
cd ./ApertureIO
git submodule update --init --recursive
conan install . --settings=build_type=Debug --build=missing
cd ./build
cmake .. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -G "Visual Studio 17 2022"
cmake --build
```

### TODO List:

* [x] Restructure Source Files.
* [x] Look into tests and building test application.
* [x] Restructure CMake and Conan Files.
* [x] Add Build Steps to README.
* [ ] Move FileIO and Logger out of Aio.
* [ ] Implement a Workflow for UniformBuffers.
* [ ] Decide how to handle platform APIs.
* [ ] Add Tests for finshed classes.