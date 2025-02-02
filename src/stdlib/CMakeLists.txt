set(SOURCE_FILES 
    lv_mem.c
)

# Create the object library for this directory
add_library(lvgl_stdlib STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_stdlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(SUBDIRS 
    builtin
    clib
    micropython
    rtthread
)

if(CONFIG_LV_USE_UEFI)
    list(APPEND SUBDIRS uefi)
endif()

# Add each subdirectory and link its library to lvgl
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    #target_sources(lvgl_stdlib PRIVATE $<TARGET_OBJECTS:lvgl_stdlib_${subdir}>)
    message(Adding lvgl_stdlib_${subdir})
    target_link_libraries(lvgl_stdlib PRIVATE lvgl_stdlib_${subdir})
endforeach()
