set(SUBDIRS fb drm lcd)


if(CONFIG_LV_USE_ILI9341)
	list(APPEND SUBDIRS ili9341)
endif()

if(CONFIG_LV_USE_ST7735)
	list(APPEND SUBDIRS st7735)
endif()

if(CONFIG_LV_USE_ST7789)
	list(APPEND SUBDIRS st7789)
endif()

if(CONFIG_LV_USE_ST7796)
	list(APPEND SUBDIRS st7796)
endif()

if(CONFIG_LV_USE_TFT_ESPI)
	list(APPEND SUBDIRS tft_espi)
endif()

if(CONFIG_LV_USE_RENESAS_GLCDC)
	list(APPEND SUBDIRS renesas_glcdc)
endif()

if(CONFIG_LV_USE_ST_LTDC)
	list(APPEND SUBDIRS st_ltdc)
endif()

if(CONFIG_LV_USE_OPENGLES)
	list(APPEND SUBDIRS glfw)
endif()


set(SOURCE_FILES)

if(NOT SOURCE_FILES)
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.c "")
	list(APPEND SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/empty.c)
endif()

add_library(lvgl_drivers_display OBJECT ${SOURCE_FILES})

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl_drivers_display PRIVATE lvgl_drivers_display_${subdir})
endforeach()

