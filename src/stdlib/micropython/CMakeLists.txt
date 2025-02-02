set(SOURCE_FILES)

if(CONFIG_LV_USE_MICROPYTHON_MALLOC)
    list(APPEND SOURCE_FILES lv_mem_core_micropython.c)
endif()

if(NOT "${SOURCE_FILES}" STREQUAL "")
    # Create the object library for this directory
    add_library(lvgl_stdlib_micropython OBJECT ${SOURCE_FILES})
    
    # Add the current directory as an include directory
    target_include_directories(lvgl_stdlib_micropython PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
