cmake_minimum_required(VERSION 3.5)
project(raytracer)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_library(raytracer SHARED ${SOURCES})
target_link_libraries(raytracer ${ImageMagick_LIBRARIES})
