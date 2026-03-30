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

add_executable(render render.cpp)
target_link_libraries(render raytracer ${ImageMagick_LIBRARIES})

# Tests
enable_testing()

add_executable(render_test tests/render_test.cpp)
target_link_libraries(render_test raytracer ${ImageMagick_LIBRARIES})

add_executable(noise_test tests/noise_test.cpp)
target_link_libraries(noise_test raytracer ${ImageMagick_LIBRARIES})
add_test(NAME test_noise COMMAND noise_test)

add_executable(noise_preview tests/noise_preview.cpp)
target_link_libraries(noise_preview raytracer ${ImageMagick_LIBRARIES})

set(REFS_DIR ${CMAKE_SOURCE_DIR}/tests/references)
set(COMPARE_SCRIPT ${CMAKE_SOURCE_DIR}/tests/compare.sh)

foreach(SCENE sphere_plain sphere_shadow sphere_two sphere_two_reflective sphere_soft_shadow cone cylinder rectangle pyramid quad checkerboard image_texture noise_simplex noise_simplex_fractal noise_value noise_cellular mesh showcase)
    add_test(
        NAME test_${SCENE}
        COMMAND bash ${COMPARE_SCRIPT} $<TARGET_FILE:render_test> ${SCENE} ${REFS_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endforeach()
