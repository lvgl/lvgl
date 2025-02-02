set(SOURCE_FILES
	lv_demo_high_res_api_example.c
	lv_demo_high_res_app_ev_charging.c
	lv_demo_high_res_app_smart_home.c
	lv_demo_high_res_app_smart_meter.c
	lv_demo_high_res_app_thermostat.c
	lv_demo_high_res_home.c
	lv_demo_high_res_top_margin.c
	lv_demo_high_res.c
	lv_demo_high_res_app_about.c
	lv_demo_high_res_util.c)

# Create the object library for this directory
add_library(lvgl_demo_high_res STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_demo_high_res PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
