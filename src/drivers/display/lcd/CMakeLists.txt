if(CONFIG_LV_USE_GENERIC_MIPI)
	set(SOURCE_FILES lv_lcd_generic_mipi.c)
	
	# Create the main static library for this directory
	add_library(lvgl_drivers_display_lcd STATIC ${SOURCE_FILES})
	
	# Add the current directory as an include directory
	target_include_directories(lvgl_drivers_display_lcd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
