set(SUBDIRS)

if(CONFIG_LV_USE_PXP)
	list(APPEND SUBDIRS pxp)
endif()

if(CONFIG_LV_USE_DRAW_VGLITE)
	list(APPEND SUBDIRS vglite)
endif()

set(SOURCE_FILES)

if(NOT SOURCE_FILES)
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.c "")
	list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/empty.c)
endif()

add_library(lvgl_draw_nxp STATIC ${SOURCE_FILES})

foreach(subdir ${SUBDIRS})
   add_subdirectory(${subdir})
   target_link_libraries(lvgl_draw_nxp PRIVATE lvgl_draw_nxp_${subdir})
endforeach()
