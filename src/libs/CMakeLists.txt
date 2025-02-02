set(SUBDIRS 
    bin_decoder
    fsdrv
)

if(CONFIG_LV_USE_BARCODE)
    list(APPEND LVGL_LIBS_SUBDIRS barcode)
endif()

if(CONFIG_LV_USE_BMP)
    list(APPEND LVGL_LIBS_SUBDIRS bmp)
endif()

if(CONFIG_LV_USE_XML)
    list(APPEND LVGL_LIBS_SUBDIRS expat)
endif()

if(CONFIG_LV_USE_FREETYPE)
    list(APPEND LVGL_LIBS_SUBDIRS freetype)
endif()

if(CONFIG_LV_USE_GIF)
    list(APPEND LVGL_LIBS_SUBDIRS gif)
endif()

if(CONFIG_LV_USE_LIBJPEG_TURBO)
    list(APPEND LVGL_LIBS_SUBDIRS libjpeg_turbo)
endif()

if(CONFIG_LV_USE_LIBPNG)
    list(APPEND LVGL_LIBS_SUBDIRS libpng)
endif()

if(CONFIG_LV_USE_LODEPNG)
    list(APPEND LVGL_LIBS_SUBDIRS lodepng)
endif()

if(CONFIG_LV_USE_LZ4_INTERNAL)
    list(APPEND LVGL_LIBS_SUBDIRS lz4)
endif()

if(CONFIG_LV_USE_QRCODE)
    list(APPEND LVGL_LIBS_SUBDIRS qrcode)
endif()

if(CONFIG_LV_USE_RLE)
    list(APPEND LVGL_LIBS_SUBDIRS rle)
endif()

if(CONFIG_LV_USE_RLOTTIE)
    list(APPEND LVGL_LIBS_SUBDIRS rlottie)
endif()

if(CONFIG_LV_USE_SVG)
    list(APPEND LVGL_LIBS_SUBDIRS svg)
endif()

if(CONFIG_LV_USE_THORVG_INTERNAL)
    list(APPEND LVGL_LIBS_SUBDIRS thorvg)
endif()

if(CONFIG_LV_USE_TINY_TTF)
    list(APPEND LVGL_LIBS_SUBDIRS tiny_ttf)
endif()

if(CONFIG_LV_USE_TJPGD)
    list(APPEND LVGL_LIBS_SUBDIRS tjpgd)
endif()


add_library(lvgl_libs INTERFACE)

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl_libs INTERFACE ${subdir})
endforeach()
