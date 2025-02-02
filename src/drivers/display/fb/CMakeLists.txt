set(SOURCE_FILES)

if(CONFIG_LV_USE_LINUX_FBDEV)
	list(APPEND SOURCE_FILES lv_linux_fbdev.c)
endif()

if(NOT SOURCE_FILES)
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.c "")
	list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/empty.c)
endif()

# Create the object library for this directory
add_library(lvgl_drivers_display_fb OBJECT ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_display_fb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
