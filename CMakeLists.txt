cmake_minimum_required(VERSION 3.0)
project(PerspectiveRendering)

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCV REQUIRED)

#add_custom_command(OUTPUT print
#  COMMAND echo hello
#  )

#add_custom_target(test_target
#  COMMAND python build_script.py
#  DEPENDS print
#  VERBATIM
#  )

#include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(commonlib)

add_library(imgui STATIC imgui_files/imconfig.h imgui_files/imgui.cpp imgui_files/imgui.h imgui_files/imgui_draw.cpp imgui_files/imgui_internal.h imgui_files/imgui_tables.cpp imgui_files/imgui_widgets.cpp imgui_files/imstb_rectpack.h imgui_files/imstb_textedit.h imgui_files/imstb_truetype.h imgui_files/imgui_impl_glfw.h imgui_files/imgui_impl_glfw.cpp imgui_files/imgui_impl_opengl3.h imgui_files/imgui_impl_opengl3.cpp)

set(ALL_LIBS ${OPENGL_gl_LIBRARY} ${OpenCV_LIBS} glfw GLEW commonlib pthread imgui)

add_executable(perspectiveRendering capture_cam_render.cpp)
#add_dependencies(perspectiveRendering test_target)
target_link_libraries(perspectiveRendering ${ALL_LIBS})


add_executable(sampleProg sample_triangle.cpp)
target_link_libraries(sampleProg ${ALL_LIBS})

add_executable(sampleScene opengl_scene.cpp)
target_link_libraries(sampleScene ${ALL_LIBS})

add_executable(hellowindow hello_window.cpp)
target_link_libraries(hellowindow ${ALL_LIBS})

add_executable(hellotri hello_triangle.cpp)
target_link_libraries(hellotri ${ALL_LIBS})

add_executable(hellosq hello_square.cpp)
target_link_libraries(hellosq ${ALL_LIBS})

add_executable(transformSquare squares_with_transform.cpp)
target_link_libraries(transformSquare ${ALL_LIBS})

add_executable(mvpCube mvp_cube.cpp)
target_link_libraries(mvpCube ${ALL_LIBS})

add_executable(mvpCamCubes mvp_with_camera.cpp)
target_link_libraries(mvpCamCubes ${ALL_LIBS})

add_executable(lightingCubes cube_lighting.cpp)
target_link_libraries(lightingCubes ${ALL_LIBS})

add_executable(lightingMaterialCubes cube_lighting_with_material.cpp)
target_link_libraries(lightingMaterialCubes ${ALL_LIBS})

add_executable(lightingMaterialMaps cube_lighting_map.cpp)
target_link_libraries(lightingMaterialMaps ${ALL_LIBS})

add_executable(lightCasting light_casting_with_imgui.cpp)
target_link_libraries(lightCasting ${ALL_LIBS})
target_include_directories(lightCasting PRIVATE imgui_files)

add_executable(modelLoading model_loading_lib.cpp)
target_link_libraries(modelLoading ${ALL_LIBS} assimp)
