set(SOURCE_FILES 
    lv_init.c
)

# Create the main static library for this directory
add_library(lvgl_main STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

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
    target_link_libraries(lvgl_main PRIVATE ${subdir})
endforeach()
