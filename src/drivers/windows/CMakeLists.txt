set(SOURCE_FILES lv_windows_display.c
lv_windows_input.c
lv_windows_context.c)
# Create the main static library for this directory
add_library(lvgl_drivers_windows STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_windows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})