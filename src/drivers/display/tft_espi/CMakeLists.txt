set(SOURCE_FILES lv_tft_espi.cpp)
# Create the main static library for this directory
add_library(lvgl_drivers_display_tft_espi STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_display_tft_espi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
