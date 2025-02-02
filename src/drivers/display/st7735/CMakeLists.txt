if(CONFIG_LV_USE_ST7735)
	set(SOURCE_FILES lv_st7735.c)
	# Create the main static library for this directory
	add_library(lvgl_drivers_display_st7735 STATIC ${SOURCE_FILES})
	
	# Add the current directory as an include directory
	target_include_directories(lvgl_drivers_display_st7735 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
