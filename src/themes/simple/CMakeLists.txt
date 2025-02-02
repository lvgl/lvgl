set(SOURCE_FILES lv_theme_simple.c)

# Create the main static library for this directory
add_library(lvgl_theme_simple STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_theme_simple PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
