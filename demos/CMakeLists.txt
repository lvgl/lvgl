set(SUBDIRS)

if(CONFIG_LV_USE_DEMO_BENCHMARK)
	list(APPEND SUBDIRS benchmark)
endif()

if(CONFIG_LV_USE_DEMO_FLEX_LAYOUT)
	list(APPEND SUBDIRS flex_layout)
endif()

if(CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER)
	list(APPEND SUBDIRS keypad_encoder)
endif()

if(CONFIG_LV_USE_DEMO_MULTILANG)
	list(APPEND SUBDIRS multilang)
endif()

if(CONFIG_LV_USE_DEMO_MUSIC)
	list(APPEND SUBDIRS music)
endif()

if(CONFIG_LV_USE_DEMO_RENDER)
	list(APPEND SUBDIRS render)
endif()

if(CONFIG_LV_USE_DEMO_SCROLL)
	list(APPEND SUBDIRS scroll)
endif()

if(CONFIG_LV_USE_DEMO_STRESS)
	list(APPEND SUBDIRS stress)
endif()

if(CONFIG_LV_USE_DEMO_TRANSFORM)
	list(APPEND SUBDIRS transform)
endif()

if(CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC)
	list(APPEND SUBDIRS vector_graphic)
endif()

if(CONFIG_LV_USE_DEMO_WIDGETS)
	list(APPEND SUBDIRS widgets)
endif()

if(CONFIG_LV_USE_DEMO_EBIKE)
	list(APPEND SUBDIRS ebike)
endif()

if(CONFIG_LV_USE_DEMO_HIGH_RES)
	list(APPEND SUBDIRS high_res)
endif()

if(CONFIG_LV_USE_DEMO_SMARTWATCH)
	list(APPEND SUBDIRS smartwatch)
endif()

set(SOURCE_FILES lv_demos.c)

add_library(lvgl_demos STATIC ${SOURCE_FILES})

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl_demos PRIVATE lvgl_demo_${subdir})
endforeach()
    
