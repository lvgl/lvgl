set(SOURCE_FILES lv_calendar.c)

if(CONFIG_LV_USE_CALENDAR_CHINESE)
	list(APPEND SUBDIRS lv_calendar_chinese.c)
endif()

if(CONFIG_LV_USE_CALENDAR_HEADER_ARROW)
	list(APPEND SUBDIRS lv_calendar_header_arrow.c)
endif()

if(CONFIG_LV_USE_CALENDAR_HEADER_DROPDOWN)
	list(APPEND SUBDIRS lv_calendar_header_dropwdown.c)
endif()

# Create the main static library for this directory
add_library(lvgl_widget_calendar STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_calendar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
