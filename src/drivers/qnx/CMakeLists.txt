set(SOURCE_FILES lv_qnx.c)
# Create the main static library for this directory
add_library(lvgl_drivers_qnx STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_qnx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})