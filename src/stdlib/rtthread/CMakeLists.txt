set(SOURCE_FILES)

if(CONFIG_LV_USE_RTTHREAD_STRING)
    list(APPEND SOURCE_FILES lv_string_rtthread.c)
endif()

if(CONFIG_LV_USE_RTTHREAD_MALLOC)
    list(APPEND SOURCE_FILES lv_mem_core_rtthread.c)
endif()

if(CONFIG_LV_USE_RTTHREAD_SPRINTF)
    list(APPEND SOURCE_FILES lv_sprintf_rtthread.c)
endif()

# Create the main static library for this directory
add_library(lvgl_stdlib_rtthread STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_stdlib_rtthread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
