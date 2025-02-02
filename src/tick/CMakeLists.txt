set(SOURCE_FILES lv_tick.c)

# Create the main static library for this directory
add_library(lvgl_tick STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_tick PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
