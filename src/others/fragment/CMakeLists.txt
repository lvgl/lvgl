set(SOURCE_FILES lv_fragment.c
lv_fragment_manager.c)
# Create the main static library for this directory
add_library(lvgl_others_fragment STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_fragment PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})