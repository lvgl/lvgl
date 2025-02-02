set(SOURCE_FILES 
	lv_blend_helium.S)

# Create the main static library for this directory
add_library(lvgl_draw_sw_blend_helium STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_sw_blend_helium PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
