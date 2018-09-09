cmake_minimum_required(VERSION 2.6)
project(PerspectiveRendering)


set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCV REQUIRED)


include_directories(${CMAKE_SOURCE_DIR})

add_executable(perspectiveRendering capture_cam_render.cpp shader.hpp stb_image.h)

set(ALL_LIBS ${OPENGL_gl_LIBRARY} ${OpenCV_LIBS} glfw GLEW pthread)

target_link_libraries(perspectiveRendering ${ALL_LIBS})
