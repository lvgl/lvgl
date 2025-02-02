set(LVGL_LAYOUT_SOURCES 
    lv_layout.c
)

# Create the main static library for this directory
add_library(lvgl_layout STATIC ${LVGL_LAYOUT_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_layout PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(LVGL_LAYOUT_SUBDIRS 
    flex
    grid
)

# Add each subdirectory and link its library
foreach(subdir ${LVGL_LAYOUT_SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl_layout PRIVATE ${subdir})
endforeach()
