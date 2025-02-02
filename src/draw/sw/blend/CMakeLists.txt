set(SOURCE_FILES 
	lv_draw_sw_blend.c 
	lv_draw_sw_blend_to_i1.c)

if(CONFIG_LV_DRAW_SW_SUPPORT_AL88)
	list(APPEND SOURCE_FILES lv_draw_sw_blend_to_al88.c)
endif()

if(CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888)
	list(APPEND SOURCE_FILES lv_draw_sw_blend_to_argb8888.c)
endif()

if(CONFIG_LV_DRAW_SW_SUPPORT_L8)
	list(APPEND SOURCE_FILES lv_draw_sw_blend_to_l8.c)
endif()

if(CONFIG_LV_DRAW_SW_SUPPORT_RGB565)
	list(APPEND SOURCE_FILES lv_draw_sw_blend_to_rgb565.c)
endif()

if(CONFIG_LV_DRAW_SW_SUPPORT_RGB888 OR LV_DRAW_SW_SUPPORT_XRGB8888)
	list(APPEND SOURCE_FILES lv_draw_sw_blend_to_rgb888.c)
endif()

# Create the main static library for this directory
add_library(lvgl_draw_sw_blend STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_sw_blend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


set(SUBDIRS helium neon arm2d)

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
   add_subdirectory(${subdir})
   target_link_libraries(lvgl_draw_sw_blend PRIVATE ${subdir})
endforeach()
