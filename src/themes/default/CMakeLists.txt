set(SOURCE_FILES lv_theme_default.c)

message("Hello there")
# Create the object library for this directory
add_library(lvgl_theme_default STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_theme_default PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
