set(SOURCE_FILES lv_gridnav.c)
# Create the main static library for this directory
add_library(lvgl_others_gridnav STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_gridnav PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})