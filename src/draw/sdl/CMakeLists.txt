set(SOURCE_FILES lv_draw_sdl.c)
# Create the main static library for this directory
add_library(lvgl_draw_sdl STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_sdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
