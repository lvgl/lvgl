set(SUBDIRS)

if(CONFIG_LV_USE_DRAW_DAVE2D)
	list(APPEND SUBDIRS dave2d)
endif()

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
   add_subdirectory(${subdir})
   target_link_libraries(lvgl_draw_renesas PRIVATE ${subdir})
endforeach()
