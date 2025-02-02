set(SOURCE_FILES 
    lv_indev.c
    lv_indev_scroll.c
)

if(CONFIG_LV_USE_GESTURE_RECOGNITION)
    list(APPEND SOURCE_FILES lv_indev_gesture.c)
endif()

# Create the main library for this directory
add_library(lvgl_indev OBJECT ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_indev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
