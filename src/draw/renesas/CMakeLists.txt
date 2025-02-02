set(SUBDIRS)

if(CONFIG_LV_USE_DRAW_DAVE2D)
	list(APPEND SUBDIRS dave2d)
endif()

set(SOURCE_FILES)

if(NOT SOURCE_FILES)
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.c "")
	list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/empty.c)
endif()

add_library(lvgl_draw_renesas STATIC ${SOURCE_FILES})

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
   add_subdirectory(${subdir})
   target_link_libraries(lvgl_draw_renesas PRIVATE lvgl_draw_renesas_${subdir})
endforeach()
