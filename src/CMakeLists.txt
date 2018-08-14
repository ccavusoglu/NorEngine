cmake_minimum_required(VERSION 3.4.1)

project(Nor)

set(PROJECT_VERSION "1.0.0")
set(CMAKE_VERBOSE_MAKEFILE on)
set (CMAKE_CXX_STANDARD 14)

message(Making ${PROJECT_SOURCE_DIR} ...)

add_definitions(-DNOR_ANDROID)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LIB_INCLUDE_PATH}/GLM/include/
    ${LIB_INCLUDE_PATH}/SDL2/include/
    ${LIB_INCLUDE_PATH}/SDL2_gpu/include/
    ${LIB_INCLUDE_PATH}/SDL2_ttf/include/)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(main SHARED ${SOURCE_FILES})

target_link_libraries(main
        ${LIB_PATH}/libSDL2.so
        ${LIB_PATH}/libSDL2_gpu.so
        ${LIB_PATH}/libSDL2_ttf.so
        -ldl -lGLESv1_CM -lGLESv2 -llog -landroid)
