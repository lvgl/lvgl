set(SOURCE_FILES 
	lv_blend_neon.S)

# Create the main static library for this directory
add_library(lvgl_draw_sw_blend_neon STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_sw_blend_neon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
