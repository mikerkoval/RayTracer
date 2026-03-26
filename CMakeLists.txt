cmake_minimum_required(VERSION 3.5)
project(Animation)

find_package(ImageMagick COMPONENTS Magick++)

include_directories(${ImageMagick_INCLUDE_DIRS})

add_subdirectory(raytracer)

include_directories(raytracer/include)

add_executable(Animation main.cpp)
target_link_libraries(Animation raytracer ${ImageMagick_LIBRARIES})
