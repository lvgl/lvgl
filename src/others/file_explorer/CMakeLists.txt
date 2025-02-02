set(SOURCE_FILES lv_file_explorer.c)
# Create the main static library for this directory
add_library(lvgl_others_file_explorer STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_file_explorer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})