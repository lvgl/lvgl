set(SOURCE_FILES)

if(CONFIG_LV_UEFI_USE_MEMORY_SERVICES AND CONFIG_USE_LV_STDLIB_CUSTOM)
    list(APPEND SOURCE_FILES lv_mem_core_uefi.c)
endif()

# Create the object library for this directory
add_library(lvgl_stdlib_uefi STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_stdlib_uefi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
