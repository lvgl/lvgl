set(SUBDIRS)

if(CONFIG_LV_USE_FILE_EXPLORER)
	list(APPEND SUBDIRS file_explorer)
endif()

if(CONFIG_LV_USE_FRAGMENT)
	list(APPEND SUBDIRS fragment)
endif()

if(CONFIG_LV_USE_GRIDNAV)
	list(APPEND SUBDIRS gridnav)
endif()

if(CONFIG_LV_USE_IME_PINYIN)
	list(APPEND SUBDIRS ime)
endif()

if(CONFIG_LV_USE_IMGFONT)
	list(APPEND SUBDIRS imgfont)
endif()

if(CONFIG_LV_USE_MONKEY)
	list(APPEND SUBDIRS monkey)
endif()

if(CONFIG_LV_USE_OBSERVER)
	list(APPEND SUBDIRS observer)
endif()

if(CONFIG_LV_USE_SNAPSHOT)
	list(APPEND SUBDIRS snapshot)
endif()

if(CONFIG_LV_USE_SYSMON)
	list(APPEND SUBDIRS sysmon)
endif()

if(CONFIG_LV_USE_VG_LITE_TVG)
	list(APPEND SUBDIRS vg_lite_tvg)
endif()

if(CONFIG_LV_USE_FONT_MANAGER)
	list(APPEND SUBDIRS font_manager)
endif()

if(CONFIG_LV_USE_XML)
	list(APPEND SUBDIRS xml)
endif()


set(SOURCE_FILES)

if(NOT SOURCE_FILES)
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.c "")
	list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/empty.c)
endif()

add_library(lvgl_others STATIC ${SOURCE_FILES})

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl_others PRIVATE lvgl_others_${subdir})
endforeach()
