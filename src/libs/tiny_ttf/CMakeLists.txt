set(LVGL_LIBS_TINY_TTF_SOURCES 
    lv_tinny_ttf.c
)

# Create the main static library for this directory
add_library(lvgl_libs_tinny_ttf STATIC ${LVGL_LIBS_TINY_TTF_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_tinny_ttf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
