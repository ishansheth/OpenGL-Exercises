cmake_minimum_required(VERSION 3.0)
project(CommonGLLib)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
file(GLOB LIB_HEADERS include/*.h)
file(GLOB LIB_SOURCES src/*.cpp)

add_library(commonlib STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_include_directories(commonlib PUBLIC include)
