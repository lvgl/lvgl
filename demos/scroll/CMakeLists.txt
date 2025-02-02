set(SOURCE_FILES
	lv_demo_scroll.c)

# Create the object library for this directory
add_library(lvgl_demo_scroll STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_demo_scroll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
