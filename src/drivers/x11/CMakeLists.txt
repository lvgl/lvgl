set(SOURCE_FILES lv_x11_display.c
lv_x11_input.c)
# Create the main static library for this directory
add_library(lvgl_drivers_x11 STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_x11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})