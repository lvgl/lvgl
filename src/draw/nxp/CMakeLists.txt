set(SUBDIRS)

if(CONFIG_LV_USE_PXP)
	list(APPEND SUBDIRS pxp)
endif()

if(CONFIG_LV_USE_DRAW_VGLITE)
	list(APPEND SUBDIRS vglite)
endif()

foreach(subdir ${SUBDIRS})
   add_subdirectory(${subdir})
   target_link_libraries(lvgl_draw_nxp PRIVATE ${subdir})
endforeach()
