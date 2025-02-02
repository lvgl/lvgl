set(SOURCE_FILES
	lv_demo_smartwatch.c
	lv_demo_smartwatch_control.c
	lv_demo_smartwatch_easter_egg.c
	lv_demo_smartwatch_home.c
	lv_demo_smartwatch_list.c
	lv_demo_smartwatch_notifications.c
	lv_demo_smartwatch_qr.c
	lv_demo_smartwatch_settings.c
	lv_demo_smartwatch_weather.c)

# Create the object library for this directory
add_library(lvgl_demo_smartwatch STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_demo_smartwatch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
