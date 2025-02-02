set(SOURCE_FILES)

if(CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW)
	list(APPEND SOURCE_FILES lv_font_dejavu_16_persian_hebrew.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_10)
	list(APPEND SOURCE_FILES lv_font_montserrat_10.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_12)
	list(APPEND SOURCE_FILES lv_font_montserrat_12.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_14)
	list(APPEND SOURCE_FILES lv_font_montserrat_14.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_16)
	list(APPEND SOURCE_FILES lv_font_montserrat_16.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_18)
	list(APPEND SOURCE_FILES lv_font_montserrat_18.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_20)
	list(APPEND SOURCE_FILES lv_font_montserrat_20.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_22)
	list(APPEND SOURCE_FILES lv_font_montserrat_22.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_24)
	list(APPEND SOURCE_FILES lv_font_montserrat_24.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_26)
	list(APPEND SOURCE_FILES lv_font_montserrat_26.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_28)
	list(APPEND SOURCE_FILES lv_font_montserrat_28.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED)
	list(APPEND SOURCE_FILES lv_font_montserrat_28_compressed.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_30)
	list(APPEND SOURCE_FILES lv_font_montserrat_30.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_32)
	list(APPEND SOURCE_FILES lv_font_montserrat_32.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_34)
	list(APPEND SOURCE_FILES lv_font_montserrat_34.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_36)
	list(APPEND SOURCE_FILES lv_font_montserrat_36.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_38)
	list(APPEND SOURCE_FILES lv_font_montserrat_38.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_40)
	list(APPEND SOURCE_FILES lv_font_montserrat_40.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_42)
	list(APPEND SOURCE_FILES lv_font_montserrat_42.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_44)
	list(APPEND SOURCE_FILES lv_font_montserrat_44.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_46)
	list(APPEND SOURCE_FILES lv_font_montserrat_46.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_48)
	list(APPEND SOURCE_FILES lv_font_montserrat_48.c)
endif()

if(CONFIG_LV_FONT_MONTSERRAT_8)
	list(APPEND SOURCE_FILES lv_font_montserrat_8.c)
endif()

if(CONFIG_LV_FONT_UNSCII_16)
	list(APPEND SOURCE_FILES lv_font_unscii_16.c)
endif()

if(CONFIG_LV_FONT_UNSCII_8)
	list(APPEND SOURCE_FILES lv_font_unscii_8.c)
endif()

if(CONFIG_LV_BINFONT_LOADER)
	list(APPEND SOURCE_FILES lv_binfont_loader.c)
endif()

if(CONFIG_LV_FONT_SIMSUN_14_CJK)
	list(APPEND SOURCE_FILES lv_font_simsun_14_cjk.c)
endif()

if(CONFIG_LV_FONT_SIMSUN_16_CJK)
	list(APPEND SOURCE_FILES lv_font_simsun_16_cjk.c)
endif()

if(CONFIG_LV_FONT)
	list(APPEND SOURCE_FILES lv_font.c)
endif()

if(CONFIG_LV_FONT_FMT_TXT)
	list(APPEND SOURCE_FILES lv_font_fmt_txt.c)
endif()


# Create the main static library for this directory
add_library(lvgl_fonts STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_fonts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
