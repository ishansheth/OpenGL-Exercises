cmake_minimum_required(VERSION 2.6)
project(PerspectiveRendering)


find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)


include_directories(${CMAKE_SOURCE_DIR})

add_executable(perspectiveRendering opengl_perspective_rendering.cpp shader.hpp shader.cpp stb_image.h)

set(ALL_LIBS ${OPENGL_gl_LIBRARY} glfw GLEW)

target_link_libraries(perspectiveRendering ${ALL_LIBS})
