set(SOURCE_FILES 
    lv_init.c
)

# Create the object library for this directory
add_library(lvgl STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(lvgl PUBLIC ${AUTO_CONF_DIR})

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
    themes
    stdlib
)

# Add each subdirectory and link its library to lvgl
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl PRIVATE lvgl_${subdir})
endforeach()
