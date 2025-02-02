set(SOURCE_FILES 
    lv_mem.c
)

# Create the main static library for this directory
add_library(lvgl_stdlib STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_stdlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(SUBDIRS 
    builtin
    clib
    micropython
    rtthread
)

# Add each subdirectory and link its library to lvgl
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl_stdlib PRIVATE ${subdir})
endforeach()
