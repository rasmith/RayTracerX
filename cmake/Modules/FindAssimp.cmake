# find headers
set(_ASSIMP_HEADER_SEARCH_DIRS,
    "/opt/local/include"
    "/usr/include"
    "/usr/local/include")
set(_ASSIMP_ENV_ROOT_DIR "$ENV{_ASSIMP_ROOT_DIR}")

if(NOT _ASSIMP_ROOT_DIR AND _ASSIMP_ENV_ROOT_DIR)
    set(ASSIMP_ROOT_DIR "${_ASSIMP_ENV_ROOT_DIR}")
endif(NOT _ASSIMP_ROOT_DIR AND _ASSIMP_ENV_ROOT_DIR)

message(STATUS "ASSIMP ROOT DIR  = ${ASSIMP_ROOT_DIR}")
if(ASSIMP_ROOT_DIR)
    set(_ASSIMP_HEADER_SEARCH_DIRS "${ASSIMP_ROOT_DIR}"
                          "${ASSIMP_ROOT_DIR}/include"
                          ${_ASSIMP_HEADER_SEARCH_DIRS})
endif(ASSIMP_ROOT_DIR)

find_path(ASSIMP_INCLUDE_DIR "assimp/config.h"
    PATHS ${_ASSIMP_HEADER_SEARCH_DIRS})

# find libraries
set(_ASSIMP_LIBRARY_SEARCH_DIRS
    "/opt/local/lib"
    "/usr/lib"
    "/usr/local/lib")

message(STATUS "_ASSIMP_LIBRARY_SEARCH_DIRS = ${_ASSIMP_LIBRARY_SEARCH_DIRS}")
if(ASSIMP_ROOT_DIR)
    set(_ASSIMP_LIBRARY_SEARCH_DIRS  "${ASSIMP_ROOT_DIR}"
                                 ${_ASSIMP_LIBRARY_SEARCH_DIRS})
endif(ASSIMP_ROOT_DIR)

message(STATUS "_ASSIMP_LIBRARY_SEARCH_DIRS = ${_ASSIMP_LIBRARY_SEARCH_DIRS}")
find_library(ASSIMP_LIBRARY NAMES assimp
             PATHS "${_ASSIMP_LIBRARY_SEARCH_DIRS}"
             PATH_SUFFIXES lib)

message(STATUS "ASSIMP_INCLUDE_DIR = ${ASSIMP_INCLUDE_DIR}")
message(STATUS "ASSIMP_LIBRARY = ${ASSIMP_LIBRARY}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Assimp REQUIRED_VARS
                                  ASSIMP_LIBRARY
                                  ASSIMP_INCLUDE_DIR)

message(STATUS "ASSIMP_FOUND = ${ASSIMP_FOUND}")

if(ASSIMP_FOUND)
    set(ASSIMP_FOUND "${ASSIMP_FOUND}")
    set(ASSIMP_INCLUDE_DIRS "${ASSIMP_INCLUDE_DIR}")
    set(ASSIMP_LIBRARIES "${ASSIMP_LIBRARY}")
endif(ASSIMP_FOUND)
