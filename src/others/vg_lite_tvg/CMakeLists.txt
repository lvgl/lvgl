set(SOURCE_FILES vg_lite_matrix.c)
# Create the object library for this directory
add_library(lvgl_others_vg_lite_tvg OBJECT ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_vg_lite_tvg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
