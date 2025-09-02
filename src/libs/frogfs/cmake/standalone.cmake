cmake_minimum_required(VERSION 3.16)

include(${CMAKE_CURRENT_LIST_DIR}/files.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/functions.cmake)

add_library(frogfs
    ${libfrogfs_SRC}
)

target_include_directories(frogfs
PUBLIC
    ${libfrogfs_INC}
)

if("${CONFIG_FROGFS_USE_ZLIB}" STREQUAL "y")
target_link_libraries(frogfs
    z
)
endif()

get_cmake_property(_vars VARIABLES)
list(SORT _vars)
foreach(_var ${_vars})
    unset(MATCHED)
    string(REGEX MATCH "^CONFIG_" MATCHED ${_var})
    if(NOT MATCHED)
        continue()
    endif()
    if("${${_var}}" STREQUAL "y")
        target_compile_definitions(frogfs PUBLIC "${_var}=1")
    else()
        target_compile_definitions(frogfs PUBLIC "${_var}=${${_var}}")
    endif()
endforeach()
