set(SOURCE_FILES
	lv_demo_music_list.c
	lv_demo_music.c
	lv_demo_music_main.c)

# Create the object library for this directory
add_library(lvgl_demo_music STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_demo_music PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
