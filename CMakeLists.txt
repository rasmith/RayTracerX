cmake_minimum_required (VERSION 2.8)
project(Ray)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++0x -Wall -Wextra -Werror")

enable_testing()

include_directories(${Ray_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tests)
 
