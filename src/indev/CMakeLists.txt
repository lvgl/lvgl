set(LVGL_INDEV_SOURCES 
    lv_indev.c
    lv_indev_scroll.c
)

if(CONFIG_LV_USE_GESTURE_RECOGNITION)
    list(APPEND LVGL_INDEV_SOURCES lv_indev_gesture.c)
endif()

# Create the main static library for this directory
add_library(lvgl_indev STATIC ${LVGL_INDEV_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_indev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
