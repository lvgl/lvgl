set(SOURCE_FILES lv_glfw_window.c
lv_opengles_debug.c
lv_opengles_texture.c
lv_opengles_driver.c)
# Create the main static library for this directory
add_library(lvgl_drivers_glfw STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_glfw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})