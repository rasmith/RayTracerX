# find headers
set(_GraphicsMagick++_HEADER_SEARCH_DIRS,
    "/usr/include"
    "/usr/local/include"
    "/opt/local/include")
    
set(_GraphicsMagick++_ENV_ROOT_DIR "$ENV{_GraphicsMagick_ROOT_DIR}")    

if(NOT _GraphicsMagick++_ROOT_DIR AND _GraphicsMagick++_ENV_ROOT_DIR)
    set(GraphicsMagick++_ROOT_DIR "${_GraphicsMagick++_ENV_ROOT_DIR}")
endif(NOT _GraphicsMagick++_ROOT_DIR AND _GraphicsMagick++_ENV_ROOT_DIR)

if(GraphicsMagick++_ROOT_DIR)
    set(_GraphicsMagick++_HEADER_SEARCH_DIRS "${GraphicsMagick++_ROOT_DIR}"
                          "${GraphicsMagick++_ROOT_DIR}/include/GraphicsMagick"
                          ${_GraphicsMagick++_HEADER_SEARCH_DIRS})
endif(GraphicsMagick++_ROOT_DIR)                                

find_path(GraphicsMagick++_INCLUDE_DIR "Magick++.h"
    PATHS ${_GraphicsMagick++_HEADER_SEARCH_DIRS}
    PATH_SUFFIXES GraphicsMagick)
     
# find libraries
set(_GraphicsMagick++_LIBRARY_SEARCH_DIRS,
    "/usr/lib"
    "/usr/local/lib"
    "/opt/local/lib")
    
if(GraphicsMagick++_ROOT_DIR)
    set(_GraphicsMagick++_LIBRARY_SEARCH_DIRS  "${GraphicsMagick++_ROOT_DIR}"                             
                                 ${_GraphicsMagick++_LIBRARY_SEARCH_DIRS})
endif(GraphicsMagick++_ROOT_DIR)
                              
find_library(GraphicsMagick++_LIBRARY NAMES GraphicsMagick++
             PATHS "${_GraphicsMagick++_LIBRARY_SEARCH_DIRS}"
             PATH_SUFFIXES lib)
             
message(STATUS "GraphicsMagick++_INCLUDE_DIR = ${GraphicsMagick++_INCLUDE_DIR}" )                              
message(STATUS "GraphicsMagick++_LIBRARY = ${GraphicsMagick++_LIBRARY}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GraphicsMagick++ REQUIRED_VARS
                                  GraphicsMagick++_LIBRARY 
                                  GraphicsMagick++_INCLUDE_DIR)
                                  
message(STATUS "GRAPHICSMAGICK++_FOUND = ${GRAPHICSMAGICK++_FOUND}")  
                              
if(GRAPHICSMAGICK++_FOUND)
    set(GraphicsMagick++_FOUND "${GRAPHICSMAGICK++_FOUND}")
    set(GraphicsMagick++_INCLUDE_DIRS "${GraphicsMagick++_INCLUDE_DIR}")   
    set(GraphicsMagick++_LIBRARIES "${GraphicsMagick++_LIBRARY}")     
endif(GRAPHICSMAGICK++_FOUND)
