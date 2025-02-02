if(CONFIG_LV_USE_ILI9341)
	set(SOURCE_FILES lv_ili9341.c)
	# Create the main static library for this directory
	add_library(lvgl_drivers_display_ili9341 STATIC ${SOURCE_FILES})
	
	# Add the current directory as an include directory
	target_include_directories(lvgl_drivers_display_ili9341 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
