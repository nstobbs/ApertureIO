# ApertureIO

#### What is ApertureIO?

ApertureIO is a C++ abstraction layer over Vulkan. For prototyping and messing around with
GPU graphic rendering and computation applications. Written in C++.

### Main Features:

* Shader Hot Reloading - Currently only on GLSL Shaders but planning of supporting hot reloading across the library like textures.

![](misc/ShaderHotReloading.gif)

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

### TODO List:

* [ ] Restructure Source Files.
* [ ] Look into tests and building test application.
* [ ] Restructure CMake and Conan Files.
* [ ] Add Build Steps to README.
* [ ] Move FileIO and Logger out of Aio.
* [ ] Implement a Workflow for UniformBuffers.
* [ ] Decide how to handle platform APIs.

