set(SOURCE_FILES lv_ime_pinyin.c)
# Create the main static library for this directory
add_library(lvgl_others_ime STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_ime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})