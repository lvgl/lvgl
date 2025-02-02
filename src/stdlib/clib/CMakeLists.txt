set(SOURCE_FILES)

if(CONFIG_LV_USE_CLIB_STRING)
    list(APPEND SOURCE_FILES lv_string_clib.c)
endif()

if(CONFIG_LV_USE_CLIB_MALLOC)
    list(APPEND SOURCE_FILES lv_mem_core_clib.c)
endif()

if(CONFIG_LV_USE_CLIB_SPRINTF)
    list(APPEND SOURCE_FILES lv_sprintf_clib.c)
endif()

# Create the main static library for this directory
add_library(lvgl_stdlib_clib STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_stdlib_clib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
