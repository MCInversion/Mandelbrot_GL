cmake_minimum_required(VERSION 3.10)
project(MandelbrotExplorer)

set(CMAKE_CXX_STANDARD 17)

# Add source and header files
file(GLOB Main_Src CONFIGURE_DEPENDS 
	"include/*.h"
	"src/*.cpp"
	"src/*.vert"
        "src/*.frag")

# Add executable
add_executable(MandelbrotExplorer ${Main_Src})

# Include the GLFW submodule
add_subdirectory(external/glfw)

# Add the GLEW submodule
add_subdirectory(external/glew)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/glew/include)

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Link libraries
target_link_libraries(MandelbrotExplorer glfw OpenGL::GL libglew_static)
