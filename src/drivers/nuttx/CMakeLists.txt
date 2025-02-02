set(SOURCE_FILES lv_nuttx_cache.c
lv_nuttx_entry.c
lv_nuttx_fbdev.c
lv_nuttx_lcd.c
lv_nuttx_touchscreen.c
lv_nuttx_libuv.c
lv_nuttx_profiler.c
lv_nuttx_image_cache.c)
# Create the main static library for this directory
add_library(lvgl_drivers_nuttx STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_nuttx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})