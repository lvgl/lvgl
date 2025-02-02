set(SOURCE_FILES)
if(CONFIG_LV_USE_ILI9341)
	list(APPEND SOURCE_FILES lv_ili9341.c)
endif()

if(NOT SOURCE_FILES)
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.c "")
	list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/empty.c)
endif()

# Create the object library for this directory
add_library(lvgl_drivers_display_ili9341 STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_drivers_display_ili9341 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
