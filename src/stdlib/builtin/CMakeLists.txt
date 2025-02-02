set(SOURCE_FILES)

if(CONFIG_LV_USE_BUILTIN_STRING)
    list(APPEND SOURCE_FILES lv_string_builtin.c)
endif()

if(CONFIG_LV_USE_BUILTIN_MALLOC)
    list(APPEND SOURCE_FILES 
        lv_mem_core_builtin.c
        lv_tlsf.c
    )
endif()

if(CONFIG_LV_USE_BUILTIN_SPRINTF)
    list(APPEND SOURCE_FILES lv_sprintf_builtin.c)
endif()

# Create the main static library for this directory
add_library(lvgl_stdlib_builtin STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_stdlib_builtin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
