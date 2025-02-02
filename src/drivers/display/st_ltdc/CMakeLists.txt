set(SOURCE_FILES lv_st_ltdc.c)
# Create the main static library for this directory
add_library(lvgl_drivers_display_st_ltdc STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_display_st_ltdc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})