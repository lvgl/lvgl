set(SOURCE_FILES lv_sysmon.c)
# Create the main static library for this directory
add_library(lvgl_others_sysmon STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_sysmon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})