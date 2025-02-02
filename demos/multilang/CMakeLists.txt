set(SOURCE_FILES
	lv_demo_multilang.c)

# Create the object library for this directory
add_library(lvgl_demo_multilang STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_demo_multilang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
