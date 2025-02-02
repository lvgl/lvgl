set(SOURCE_FILES lv_observer.c)
# Create the main static library for this directory
add_library(lvgl_others_observer STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_others_observer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})