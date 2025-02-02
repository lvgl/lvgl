set(SOURCE_FILES 
	lv_draw_sw_utils.c
	lv_draw_sw.c
	lv_draw_sw_border.c
	lv_draw_sw_fill.c
	lv_draw_sw_img.c
	lv_draw_sw_letter.c
	lv_draw_sw_line.c
	lv_draw_sw_transform.c
	lv_draw_sw_triangle.c
)


if(CONFIG_LV_DRAW_SW_COMPLEX)
	list(APPEND SOURCE_FILES lv_draw_sw_arc.c)
	list(APPEND SOURCE_FILES lv_draw_sw_box_shadow.c)
	list(APPEND SOURCE_FILES lv_draw_sw_mask.c)
	list(APPEND SOURCE_FILES lv_draw_sw_mask_rect.c)
endif()
if(CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS)
	list(APPEND SOURCE_FILES lv_draw_sw_gradient.c)
endif()
if(CONFIG_LV_USE_VECTOR_GRAPHIC AND CONFIG_LV_USE_THORVG)
	list(APPEND SOURCE_FILES lv_draw_sw_vector.c)
endif()

# Create the main static library for this directory
add_library(lvgl_draw_sw STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw_sw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(SUBDIRS blend arm2d)

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
   add_subdirectory(${subdir})
   target_link_libraries(lvgl_draw_sw PRIVATE ${subdir})
endforeach()
