set(LVGL_SOURCES 
    lv_init.c
)

# Create the main static library for this directory
add_library(lvgl STATIC ${LVGL_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(SUBDIRS 
    core
    display
    draw
    drivers
    font
    indev
    layouts
    libs
    misc
    osal
    others
    tick
    widgets
)

# Add each subdirectory and link its library to lvgl
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl PRIVATE ${subdir})
endforeach()
