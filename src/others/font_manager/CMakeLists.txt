set(SOURCE_FILES lv_font_manager_recycle.c
lv_font_manager_utils.c
lv_font_manager.c)
# Create the main static library for this directory
add_library(lvgl_others_font_manager STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_font_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})