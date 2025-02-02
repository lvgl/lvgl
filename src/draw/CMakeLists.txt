set(SOURCE_FILES lv_image_decoder.c
		lv_draw_arc.c
		lv_draw_line.c
		lv_draw_mask.c
		lv_draw_triangle.c
		lv_draw_buf.c
		lv_draw_rect.c
		lv_draw.c
		lv_draw_image.c
		lv_draw_label.c
		)

if(CONFIG_LV_USE_VECTOR_GRAPHIC)
	list(APPEND SOURCE_FILES lv_draw_vector.c)
endif()

# Create the main static library for this directory
add_library(lvgl_draw STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_draw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(SUBDIRS nxp renesas)

if(CONFIG_LV_USE_DRAW_SDL)
	list(APPEND SUBDIRS sdl)
endif()

if(CONFIG_LV_USE_DRAW_SW)
	list(APPEND SUBDIRS sw)
endif()

if(CONFIG_LV_USE_DRAW_VG_LITE)
	list(APPEND SUBDIRS vg_lite)
endif()

if(CONFIG_LV_USE_DRAW_DMA2D)
	list(APPEND SUBDIRS dma2d)
endif()

if(CONFIG_LV_USE_DRAW_NEMA_GFX)
	list(APPEND SUBDIRS nema_gfx)
endif()

if(CONFIG_LV_USE_DRAW_OPENGLES)
	list(APPEND SUBDIRS opengles)
endif()

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
   add_subdirectory(${subdir})
   target_link_libraries(lvgl_draw PRIVATE ${subdir})
endforeach()
