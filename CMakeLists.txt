cmake_minimum_required(VERSION 2.6)
project(PerspectiveRendering)

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCV REQUIRED)

add_custom_command(OUTPUT print
  COMMAND echo hello
  )

add_custom_target(test_target
  COMMAND python build_script.py
  DEPENDS print
  VERBATIM
  )

include_directories(${CMAKE_SOURCE_DIR})

add_executable(perspectiveRendering capture_cam_render.cpp shader.hpp stb_image.h)
add_dependencies(perspectiveRendering test_target)

set(ALL_LIBS ${OPENGL_gl_LIBRARY} ${OpenCV_LIBS} glfw GLEW pthread)

target_link_libraries(perspectiveRendering ${ALL_LIBS})


add_executable(sampleProg sample_triangle.cpp)
target_link_libraries(sampleProg ${OPENGL_gl_LIBRARY} glfw GLEW)

add_executable(sampleScene opengl_scene.cpp)
target_link_libraries(sampleScene ${OPENGL_gl_LIBRARY} glfw GLEW)


add_executable(hellowindow hello_window.cpp)
target_link_libraries(hellowindow ${OPENGL_gl_LIBRARY} glfw GLEW)


add_executable(hellotri hello_triangle.cpp)
target_link_libraries(hellotri ${OPENGL_gl_LIBRARY} glfw GLEW)

add_executable(hellosq hello_square.cpp VertexBuffer.h VertexBuffer.cpp IndexBuffer.h IndexBuffer.cpp VertexArray.h VertexArray.cpp VertexBufferLayout.h Renderer.h Renderer.cpp Texture.h Texture.cpp)
target_link_libraries(hellosq ${OPENGL_gl_LIBRARY} glfw GLEW)


add_executable(transformSquare squares_with_transform.cpp VertexBuffer.h VertexBuffer.cpp IndexBuffer.h IndexBuffer.cpp VertexArray.h VertexArray.cpp VertexBufferLayout.h Renderer.h Renderer.cpp Texture.h Texture.cpp)
target_link_libraries(transformSquare ${OPENGL_gl_LIBRARY} glfw GLEW)


add_executable(mvpCube mvp_cube.cpp VertexBuffer.h VertexBuffer.cpp IndexBuffer.h IndexBuffer.cpp VertexArray.h VertexArray.cpp VertexBufferLayout.h Renderer.h Renderer.cpp Texture.h Texture.cpp)
target_link_libraries(mvpCube ${OPENGL_gl_LIBRARY} glfw GLEW)


add_executable(mvpCamCubes mvp_with_camera.cpp VertexBuffer.h VertexBuffer.cpp IndexBuffer.h IndexBuffer.cpp VertexArray.h VertexArray.cpp VertexBufferLayout.h Renderer.h Renderer.cpp Texture.h Texture.cpp)
target_link_libraries(mvpCamCubes ${OPENGL_gl_LIBRARY} glfw GLEW)


add_executable(lightingCubes cube_lighting.cpp VertexBuffer.h VertexBuffer.cpp IndexBuffer.h IndexBuffer.cpp VertexArray.h VertexArray.cpp VertexBufferLayout.h Renderer.h Renderer.cpp Texture.h Texture.cpp)
target_link_libraries(lightingCubes ${OPENGL_gl_LIBRARY} glfw GLEW)


add_executable(lightingMaterialCubes cube_lighting_with_material.cpp VertexBuffer.h VertexBuffer.cpp IndexBuffer.h IndexBuffer.cpp VertexArray.h VertexArray.cpp VertexBufferLayout.h Renderer.h Renderer.cpp Texture.h Texture.cpp)
target_link_libraries(lightingMaterialCubes ${OPENGL_gl_LIBRARY} glfw GLEW)


add_executable(modelLoading model_loading_lib.cpp model.h mesh.h shader.hpp)
target_link_libraries(modelLoading ${OPENGL_gl_LIBRARY} glfw GLEW assimp)
