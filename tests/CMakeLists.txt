cmake_minimum_required(VERSION 2.8)

add_subdirectory(gmock)

include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#  message(STATUS "dir='${dir}'")
#endforeach()
add_executable(parse_utils_test parse_utils_test.cpp ${Ray_SOURCE_DIR}/src/parse_utils.cpp )
add_executable(image_test image_test.cpp ${Ray_SOURCE_DIR}/src/image_utils.cpp )

target_link_libraries(parse_utils_test  gtest gtest_main)
target_link_libraries(image_test  gtest gtest_main)

add_test(parse_utils_test parse_utils_test)
add_test(image_test image_test)