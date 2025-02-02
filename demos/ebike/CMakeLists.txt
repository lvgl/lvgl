set(SOURCE_FILES
	lv_demo_ebike.c
	lv_demo_ebike_settings.c
	lv_demo_ebike_home.c
	lv_demo_ebike_stats.c)

# Create the object library for this directory
add_library(lvgl_demo_ebike STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_demo_ebike PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
