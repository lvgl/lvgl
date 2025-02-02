if(CONFIG_LV_USE_LINUX_DRM)
	set(SOURCE_FILES lv_linux_drm.c)
	
	# Create the main static library for this directory
	add_library(lvgl_drivers_display_drm STATIC ${SOURCE_FILES})
	
	# Add the current directory as an include directory
	target_include_directories(lvgl_drivers_display_drm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
