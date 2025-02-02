set(SOURCE_FILES lv_draw_opengles.c)
# Create the main static library for this directory
add_library(lvgl_draw_opengles STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_opengles PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})