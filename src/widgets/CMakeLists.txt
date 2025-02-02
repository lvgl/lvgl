set(SUBDIRS)

if(CONFIG_LV_USE_ANIMING AND LV_USE_IMAGE)
	list(APPEND SUBDIRS animimage)
endif()

if(CONFIG_LV_USE_ARC)
	list(APPEND SUBDIRS arc)
endif()

if(CONFIG_LV_USE_BAR)
	list(APPEND SUBDIRS bar)
endif()

if(CONFIG_LV_USE_BUTTON)
	list(APPEND SUBDIRS button)
endif()

if(CONFIG_LV_USE_BUTTONMATRIX)
	list(APPEND SUBDIRS buttonmatrix)
endif()

if(CONFIG_LV_USE_CALENDAR)
	list(APPEND SUBDIRS calendar)
endif()

if(CONFIG_LV_USE_CANVAS)
	list(APPEND SUBDIRS canvas)
endif()

if(CONFIG_LV_USE_CHART)
	list(APPEND SUBDIRS chart)
endif()

if(CONFIG_LV_USE_CHECKBOX)
	list(APPEND SUBDIRS checkbox)
endif()

if(CONFIG_LV_USE_DROPDOWN)
	list(APPEND SUBDIRS dropdown)
endif()

if(CONFIG_LV_USE_IMAGE)
	list(APPEND SUBDIRS image)
endif()

if(CONFIG_LV_USE_IMAGEBUTTON)
	list(APPEND SUBDIRS imagebutton)
endif()

if(CONFIG_LV_USE_KEYBOARD)
	list(APPEND SUBDIRS keyboard)
endif()

if(CONFIG_LV_USE_LABEL)
	list(APPEND SUBDIRS label)
endif()

if(CONFIG_LV_USE_LED)
	list(APPEND SUBDIRS led)
endif()

if(CONFIG_LV_USE_LINE)
	list(APPEND SUBDIRS line)
endif()

if(CONFIG_LV_USE_LIST)
	list(APPEND SUBDIRS list)
endif()

if(CONFIG_LV_USE_MENU)
	list(APPEND SUBDIRS menu)
endif()

if(CONFIG_LV_USE_MSGBOX)
	list(APPEND SUBDIRS msgbox)
endif()

if(CONFIG_LV_USE_OBJX_TEMPL)
	list(APPEND SUBDIRS objx_templ)
endif()

if(CONFIG_LV_USE_ROLLER)
	list(APPEND SUBDIRS roller)
endif()

if(CONFIG_LV_USE_SCALE)
	list(APPEND SUBDIRS scale)
endif()

if(CONFIG_LV_USE_SLIDER)
	list(APPEND SUBDIRS slider)
endif()

if(CONFIG_LV_USE_SPAN)
	list(APPEND SUBDIRS span)
endif()

if(CONFIG_LV_USE_SPINBOX)
	list(APPEND SUBDIRS spinbox)
endif()

if(CONFIG_LV_USE_SPINNER)
	list(APPEND SUBDIRS spinner)
endif()

if(CONFIG_LV_USE_SWITCH)
	list(APPEND SUBDIRS switch)
endif()

if(CONFIG_LV_USE_TABLE)
	list(APPEND SUBDIRS table)
endif()

if(CONFIG_LV_USE_TABVIEW)
	list(APPEND SUBDIRS tabview)
endif()

if(CONFIG_LV_USE_TEXTAREA)
	list(APPEND SUBDIRS textarea)
endif()

if(CONFIG_LV_USE_TILEVIEW)
	list(APPEND SUBDIRS tileview)
endif()

if(CONFIG_LV_USE_WIN)
	list(APPEND SUBDIRS win)
endif()

if(CONFIG_LV_USE_LOTTIE)
	list(APPEND SUBDIRS lottie)
endif()

if(CONFIG_LV_USE_PROPERTY)
	list(APPEND SUBDIRS property)
endif()

if("${SOURCE_FILES}" STREQUAL "") 
	add_library(lvgl_widgets INTERFACE)
else()
	add_library(lvgl_widgets STATIC ${SOURCE_FILES})
endif()

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
	add_subdirectory(${subdir})
	if("${SOURCE_FILES}" STREQUAL "") 
		target_link_libraries(lvgl_widgets INTERFACE ${subdir})
	else()
		target_link_libraries(lvgl_widgets PRIVATE ${subdir})
	endif()
endforeach()
