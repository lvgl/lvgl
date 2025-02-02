set(SOURCE_FILES lv_wayland_smm.c
lv_wayland.c)
# Create the main static library for this directory
add_library(lvgl_drivers_wayland STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_wayland PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})