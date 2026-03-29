cmake_minimum_required(VERSION 3.5)
project(raytracer)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_library(raytracer SHARED ${SOURCES})
target_compile_options(raytracer PRIVATE -isystem ${CMAKE_CURRENT_SOURCE_DIR}/include/Eigen)
target_link_libraries(raytracer ${ImageMagick_LIBRARIES})
