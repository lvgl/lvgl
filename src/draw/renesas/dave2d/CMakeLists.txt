set(SOURCE_FILES lv_draw_dave2d_mask_rectangle.c
   lv_draw_dave2d.c
   lv_draw_dave2d_arc.c
   lv_draw_dave2d_border.c
   lv_draw_dave2d_fill.c
   lv_draw_dave2d_image.c
   lv_draw_dave2d_label.c
   lv_draw_dave2d_line.c
   lv_draw_dave2d_triangle.c
   lv_draw_dave2d_utils.c)

# Create the main static library for this directory
add_library(lvgl_draw_renesas_dave2d STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_renesas_dave2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
