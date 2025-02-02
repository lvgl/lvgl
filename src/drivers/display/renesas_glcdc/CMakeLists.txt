if(CONFIG_LV_USE_RENESAS_GLCDC)
	set(SOURCE_FILES lv_renesas_glcdc.c)
	# Create the main static library for this directory
	add_library(lvgl_drivers_display_renesas_glcdc STATIC ${SOURCE_FILES})
	
	# Add the current directory as an include directory
	target_include_directories(lvgl_drivers_display_renesas_glcdc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
