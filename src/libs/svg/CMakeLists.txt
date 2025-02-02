set(LVGL_LIBS_SVG_SOURCES 
    lv_svg.c
    lv_svg_parser.c
    lv_svg_render.c
    lv_svg_token.c
)

# Create the main static library for this directory
add_library(lvgl_libs_svg STATIC ${LVGL_LIBS_SVG_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_svg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
