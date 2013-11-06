cmake_minimum_required (VERSION 2.8)
project(Ray)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror")
set(GLM_ROOT_DIR "/home/agrippa/sdks/glm_0.9.4.5")
set(GraphicsMagick++_ROOT_DIR "/home/agrippa/sdks/Graphics-Magick_1.3.18")
set(ASSIMP_DIR, "/home/agrippa/sdks/assimp_3.0.1270")
enable_testing()

include_directories(${Ray_SOURCE_DIR}/include)

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})


find_package(Assimp REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIRS})
set(LIBS ${LIBS} ${ASSIMP_LIBRARIES})

find_package(GraphicsMagick++ REQUIRED)
include_directories(${GraphicsMagick++_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GraphicsMagick++_LIBRARIES})

add_subdirectory(src)
add_subdirectory(tests)
 
