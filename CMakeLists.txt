cmake_minimum_required(VERSION 3.5)
project(Animation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ImageMagick COMPONENTS Magick++)

include_directories(SYSTEM ${ImageMagick_INCLUDE_DIRS})

add_subdirectory(raytracer)

include_directories(raytracer/include)

add_executable(Animation main.cpp)
target_link_libraries(Animation raytracer ${ImageMagick_LIBRARIES})
