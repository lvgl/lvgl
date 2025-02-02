set(LVGL_LIBS_FREETYPE_SOURCES 
    lv_freetype.c
    lv_freetype_glyph.c
    lv_freetype_image.c
    lv_freetype_outline.c
    lv_ftsystem.c
)

# Create the main static library for this directory
add_library(lvgl_libs_freetype STATIC ${LVGL_LIBS_FREETYPE_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_freetype PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
