cmake_minimum_required(VERSION 3.21)
project(ArrhythmiaStudio)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(include/ffmpeg)
include_directories(include/ffmpegcpp)
include_directories(include/msdf-atlas-gen)
include_directories(include/msdfgen)
include_directories(include/freetype)
include_directories(include/harfbuzz)
include_directories(include/bson)

file(GLOB_RECURSE SOURCES src/*.h src/*.cpp include/*.h include/*.cpp include/*.c)

file(GLOB LIBRARIES libs/*.lib)
file(GLOB LIBRARIES_DEBUG libs/debug_libs/*.lib)
file(GLOB LIBRARIES_RELEASE libs/release_libs/*.lib)

add_compile_definitions(
        STB_IMAGE_IMPLEMENTATION
        GLFW_INCLUDE_NONE
        NOMINMAX)

add_compile_definitions("PROJECT_NAME=Arrhythmia Studio")

link_directories(libs)

add_executable(ArrhythmiaStudio ${SOURCES})
target_link_libraries(ArrhythmiaStudio ${LIBRARIES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(ArrhythmiaStudio ${LIBRARIES_DEBUG})
else (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(ArrhythmiaStudio ${LIBRARIES_RELEASE})
endif()

add_custom_command(
        TARGET ArrhythmiaStudio PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/copy_to_build ${CMAKE_CURRENT_BINARY_DIR})
