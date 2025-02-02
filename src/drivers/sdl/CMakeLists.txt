set(SOURCE_FILES lv_sdl_window.c
lv_sdl_keyboard.c
lv_sdl_mouse.c
lv_sdl_mousewheel.c)
# Create the main static library for this directory
add_library(lvgl_drivers_sdl STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_sdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})