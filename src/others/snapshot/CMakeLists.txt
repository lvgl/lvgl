set(SOURCE_FILES lv_snapshot.c)
# Create the main static library for this directory
add_library(lvgl_others_snapshot STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_snapshot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})