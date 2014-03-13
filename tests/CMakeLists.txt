cmake_minimum_required(VERSION 2.8)

add_subdirectory(gmock)

include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#  message(STATUS "dir='${dir}'")
#endforeach()
add_executable(parse_utils_test parse_utils_test.cpp 
                                ${Ray_SOURCE_DIR}/src/parse_utils.cpp)
add_executable(image_storage_test image_storage_test.cpp 
                                  ${Ray_SOURCE_DIR}/src/image.cpp)
add_executable(image_test image_test.cpp ${Ray_SOURCE_DIR}/src/image.cpp)
add_executable(octree_test octree_test.cpp
                                  ${Ray_SOURCE_DIR}/src/camera.cpp
                                  ${Ray_SOURCE_DIR}/src/geometry.cpp
                                  ${Ray_SOURCE_DIR}/src/io_utils.cpp
                                  ${Ray_SOURCE_DIR}/src/image.cpp
                                  ${Ray_SOURCE_DIR}/src/light.cpp
                                  ${Ray_SOURCE_DIR}/src/material.cpp
                                  ${Ray_SOURCE_DIR}/src/mesh.cpp
                                  ${Ray_SOURCE_DIR}/src/octnode64.cpp
                                  ${Ray_SOURCE_DIR}/src/octree_base.cpp
                                  ${Ray_SOURCE_DIR}/src/ray.cpp
                                  ${Ray_SOURCE_DIR}/src/raytracer.cpp
                                  ${Ray_SOURCE_DIR}/src/scene.cpp
                                  ${Ray_SOURCE_DIR}/src/scene_utils.cpp
                                  ${Ray_SOURCE_DIR}/src/shape.cpp
                                  ${Ray_SOURCE_DIR}/src/texture.cpp
                                  ${Ray_SOURCE_DIR}/src/transform.cpp)
add_executable(raytracer_test raytracer_test.cpp 
                                  ${Ray_SOURCE_DIR}/src/camera.cpp
                                  ${Ray_SOURCE_DIR}/src/geometry.cpp
                                  ${Ray_SOURCE_DIR}/src/io_utils.cpp
                                  ${Ray_SOURCE_DIR}/src/image.cpp
                                  ${Ray_SOURCE_DIR}/src/light.cpp
                                  ${Ray_SOURCE_DIR}/src/material.cpp
                                  ${Ray_SOURCE_DIR}/src/mesh.cpp
                                  ${Ray_SOURCE_DIR}/src/octree_base.cpp
                                  ${Ray_SOURCE_DIR}/src/ray.cpp
                                  ${Ray_SOURCE_DIR}/src/raytracer.cpp
                                  ${Ray_SOURCE_DIR}/src/scene.cpp
                                  ${Ray_SOURCE_DIR}/src/scene_utils.cpp
                                  ${Ray_SOURCE_DIR}/src/shape.cpp
                                  ${Ray_SOURCE_DIR}/src/texture.cpp
                                  ${Ray_SOURCE_DIR}/src/transform.cpp)
add_executable(scene_loader_test scene_loader_test.cpp 
                                  ${Ray_SOURCE_DIR}/src/camera.cpp
                                  ${Ray_SOURCE_DIR}/src/geometry.cpp
                                  ${Ray_SOURCE_DIR}/src/io_utils.cpp
                                  ${Ray_SOURCE_DIR}/src/image.cpp
                                  ${Ray_SOURCE_DIR}/src/light.cpp
                                  ${Ray_SOURCE_DIR}/src/material.cpp
                                  ${Ray_SOURCE_DIR}/src/mesh.cpp
                                  ${Ray_SOURCE_DIR}/src/octree_base.cpp
                                  ${Ray_SOURCE_DIR}/src/ray.cpp
                                  ${Ray_SOURCE_DIR}/src/scene.cpp
                                  ${Ray_SOURCE_DIR}/src/scene_utils.cpp
                                  ${Ray_SOURCE_DIR}/src/shape.cpp
                                  ${Ray_SOURCE_DIR}/src/texture.cpp
                                  ${Ray_SOURCE_DIR}/src/transform.cpp)
target_link_libraries(image_storage_test  ${LIBS} gtest gtest_main)
target_link_libraries(image_test  ${LIBS} gtest gtest_main)
target_link_libraries(octree_test  ${LIBS} gtest gtest_main)
target_link_libraries(parse_utils_test  gtest gtest_main)
target_link_libraries(raytracer_test  ${LIBS} gtest gtest_main)
target_link_libraries(scene_loader_test  ${LIBS} gtest gtest_main)


add_test(parse_utils_test parse_utils_test)
add_test(image_test image_test)
add_test(image_storage_test image_storage_test)
add_test(scene_loader_test scene_loader_test)
add_test(raytracer_test raytracer_test)