set(SUBDIRS display)

if(CONFIG_LV_USE_EVDEV)
	list(APPEND SUBDIRS evdev)
endif()

if(CONFIG_LV_USE_LIBINPUT)
	list(APPEND SUBDIRS libinput)
endif()

if(CONFIG_LV_USE_NUTTX)
	list(APPEND SUBDIRS nuttx)
endif()

if(CONFIG_LV_USE_SDL)
	list(APPEND SUBDIRS sdl)
endif()

if(CONFIG_LV_USE_WINDOWS)
	list(APPEND SUBDIRS windows)
endif()

if(CONFIG_LV_USE_X11)
	list(APPEND SUBDIRS x11)
endif()

if(CONFIG_LV_USE_QNX)
	list(APPEND SUBDIRS qnx)
endif()

if(CONFIG_LV_USE_WAYLAND)
	list(APPEND SUBDIRS wayland)
endif()

if(CONFIG_LV_USE_OPENGLES)
	list(APPEND SUBDIRS glfw)
endif()


add_library(lvgl_drivers INTERFACE)

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
	add_subdirectory(${subdir})
	target_link_libraries(lvgl_drivers INTERFACE ${subdir})
endforeach()
