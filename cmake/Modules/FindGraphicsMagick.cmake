
# find headers
set(_GraphicsMagick_HEADER_SEARCH_DIRS,
    "/usr/include"
    "/usr/local/include")
    
set(_GraphicsMagick_ENV_ROOT_DIR "$ENV{GLM_ROOT_DIR}")    

if(NOT _GraphicsMagick_ROOT_DIR AND _GraphicsMagick_ENV_ROOT_DIR)
    set(GraphicsMagick_ROOT_DIR "${_GraphicsMagick_ENV_ROOT_DIR}")
endif(NOT _GraphicsMagick_ROOT_DIR AND _GraphicsMagick_ENV_ROOT_DIR)

if(GraphicsMagick_ROOT_DIR)
    set(_GraphicsMagick_HEADER_SEARCH_DIRS "${GraphicsMagick_ROOT_DIR}"
                                "${GraphicsMagick_ROOT_DIR}/include"
                                 ${_GraphicsMagick_HEADER_SEARCH_DIRS})
endif(GraphicsMagick_ROOT_DIR)                                

find_path(GraphicsMagick_INCLUDE_DIR "GraphicsMagick/Magick++.h"
    PATHS ${_GraphicsMagick_HEADER_SEARCH_DIRS})
     
# find libraries
set(_GraphicsMagick_LIBRARY_SEARCH_DIRS,
    "/usr/lib"
    "/usr/local/lib")
    
if(GraphicsMagick_ROOT_DIR)
    set(_GraphicsMagick_LIBRARY_SEARCH_DIRS  "${GraphicsMagick_ROOT_DIR}"
                                "${GraphicsMagick_ROOT_DIR}/lib"                               
                                 ${_GraphicsMagick_LIBRARY_SEARCH_DIRS})
endif(GraphicsMagick_ROOT_DIR)
                              
find_library(GraphicsMagick_LIBRARY NAMES libGraphicsMagick.so
             PATHS "${_GraphicsMagick_LIBRARY_SEARCH_DIRS}")
             
message(STATUS "GraphicsMagick_LIBRARY = ${GraphicsMagick_LIBRARY}")
message(STATUS "_GraphicsMagick_LIBRARY_SEARCH_DIRS = ${_GraphicsMagick_LIBRARY_SEARCH_DIRS}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GraphicsMagick DEFAULT_MSG
                                  GraphicsMagick_LIBRARY 
                                  GraphicsMagick_INCLUDE_DIR)
                                  
if(GraphicsMagick_FOUND)
    set(GraphicsMagick_INCLUDE_DIRS "${GraphicsMagick_INCLUDE_DIR}")   
    message(STATUS "GraphicsMagick_INCLUDE_DIR = ${GraphicsMagick_INCLUDE_DIR}")
endif(GraphicsMagick_FOUND)
