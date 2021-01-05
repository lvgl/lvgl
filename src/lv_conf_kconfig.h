/**
 * @file lv_conf_kconfig.h
 * Configs that needs special handling when LVGL is used with Kconfig
 */

#ifndef LV_CONF_KCONFIG_H
#define LV_CONF_KCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LV_CONF_KCONFIG_EXTERNAL_INCLUDE
#  include LV_CONF_KCONFIG_EXTERNAL_INCLUDE
#else

#  if defined ESP_PLATFORM
#    include "sdkconfig.h"
#    include "esp_attr.h"
#  endif

#  ifdef __NuttX__
#    include <nuttx/config.h>
#  endif

#endif /*LV_CONF_KCONFIG_EXTERNAL_INCLUDE*/

/*******************
 * LV_MEM_SIZE
 *******************/

#ifndef LV_MEM_SIZE
#if defined (CONFIG_LV_MEM_SIZE_BYTES)
#define CONFIG_LV_MEM_SIZE    (CONFIG_LV_MEM_SIZE_BYTES * 1024U)
#endif
#endif

/*******************
 * LV COLOR TRANSP
 *******************/

#ifndef LV_COLOR_TRANSP
#if defined (CONFIG_LV_COLOR_TRANSP_HEX)
#define CONFIG_LV_COLOR_TRANSP    lv_color_hex(CONFIG_LV_COLOR_TRANSP_HEX)
#else
#define CONFIG_LV_COLOR_TRANSP    LV_COLOR_LIME
#endif
#endif

/*******************
 * THEME SELECTION
 *******************/

#ifndef LV_THEME_DEFAULT_INIT
#if defined (CONFIG_LV_THEME_DEFAULT_INIT_EMPTY)
#define CONFIG_LV_THEME_DEFAULT_INIT               lv_theme_empty_init
#elif defined (CONFIG_LV_THEME_DEFAULT_INIT_TEMPLATE)
#define CONFIG_LV_THEME_DEFAULT_INIT               lv_theme_template_init
#elif defined (CONFIG_LV_THEME_DEFAULT_INIT_MATERIAL)
#define CONFIG_LV_THEME_DEFAULT_INIT               lv_theme_material_init
#elif defined (CONFIG_LV_THEME_DEFAULT_INIT_MONO)
#define LV_THEME_DEFAULT_INIT                      lv_theme_mono_init
#endif
#endif

/*******************
 * COLOR SELECTION
 *******************/

#ifndef LV_THEME_DEFAULT_COLOR_PRIMARY
#ifdef CONFIG_LV_THEME_DEFAULT_COLOR_PRIMARY
#define LV_THEME_DEFAULT_COLOR_PRIMARY lv_color_hex(CONFIG_LV_THEME_DEFAULT_COLOR_PRIMARY)
#endif
#endif

#ifndef LV_THEME_DEFAULT_COLOR_SECONDARY
#ifdef CONFIG_LV_THEME_DEFAULT_COLOR_SECONDARY
#define LV_THEME_DEFAULT_COLOR_SECONDARY lv_color_hex(CONFIG_LV_THEME_DEFAULT_COLOR_SECONDARY)
#endif
#endif

/********************
 * FONT SELECTION
 *******************/

/* NOTE: In Kconfig instead of `LV_THEME_DEFAULT_FONT_SMALL`
 *       `CONFIG_LV_THEME_DEFAULT_FONT_SMALL_<font_name>` is defined
 *       hence the large selection with if-s
 */

/*------------------
 * SMALL FONT
 *-----------------*/
#ifndef LV_THEME_DEFAULT_FONT_SMALL
#if defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_8
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_8
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_10
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_10
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_12
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_12
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_14
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_14
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_16
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_16
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_18
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_18
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_20
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_20
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_22
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_22
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_24
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_24
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_26
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_26
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_28
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_28
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_30
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_30
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_32
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_32
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_34
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_34
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_36
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_36
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_38
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_38
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_40
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_40
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_42
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_42
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_44
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_44
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_46
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_46
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_48
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_48
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_UNSCII_8
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_unscii_8
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_UNSCII_16
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_unscii_16
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT12SUBPX
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_12_subpx
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT28COMPRESSED
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_28_compressed
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_DEJAVU_16_PERSIAN_HEBREW
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_dejavu_16_persian_hebrew
#elif defined CONFIG_LV_FONT_DEFAULT_SMALL_SIMSUN_16_CJK
#define CONFIG_LV_THEME_DEFAULT_FONT_SMALL         &lv_font_simsun_16_cjk
#endif
#endif

/*------------------
 * NORMAL FONT
 *-----------------*/
#ifndef LV_THEME_DEFAULT_FONT_NORMAL
#if defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_8
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_8
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_10
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_10
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_12
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_12
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_14
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_14
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_16
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_16
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_18
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_18
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_20
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_20
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_22
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_22
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_24
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_24
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_26
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_26
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_28
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_28
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_30
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_30
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_32
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_32
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_34
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_34
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_36
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_36
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_38
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_38
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_40
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_40
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_42
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_42
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_44
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_44
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_46
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_46
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_48
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_48
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_UNSCII_8
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_unscii_8
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_UNSCII_16
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_unscii_16
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT12SUBPX
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_12_subpx
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT28COMPRESSED
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_28_compressed
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_DEJAVU_16_PERSIAN_HEBREW
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_dejavu_16_persian_hebrew
#elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_SIMSUN_16_CJK
#define CONFIG_LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_simsun_16_cjk
#endif
#endif

/*------------------
 * SUBTITLE FONT
 *-----------------*/
#ifndef LV_THEME_DEFAULT_FONT_SUBTITLE
#if defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_8
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_8
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_10
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_10
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_12
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_12
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_14
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_14
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_16
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_16
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_18
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_18
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_20
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_20
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_22
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_22
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_24
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_24
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_26
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_26
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_28
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_28
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_30
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_30
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_32
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_32
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_34
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_34
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_36
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_36
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_38
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_38
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_40
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_40
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_42
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_42
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_44
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_44
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_46
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_46
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_48
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_48
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_UNSCII_8
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_unscii_8
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_UNSCII_16
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_unscii_16
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT12SUBPX
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_12_subpx
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT28COMPRESSED
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_28_compressed
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_DEJAVU_16_PERSIAN_HEBREW
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_dejavu_16_persian_hebrew
#elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_SIMSUN_16_CJK
#define CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_simsun_16_cjk
#endif
#endif

/*------------------
 * TITLE FONT
 *-----------------*/
#ifndef LV_THEME_DEFAULT_FONT_TITLE
#if defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_8
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_8
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_10
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_10
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_12
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_12
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_14
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_14
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_16
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_16
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_18
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_18
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_20
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_20
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_22
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_22
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_24
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_24
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_26
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_26
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_28
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_28
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_30
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_30
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_32
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_32
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_34
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_34
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_36
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_36
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_38
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_38
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_40
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_40
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_42
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_42
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_44
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_44
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_46
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_46
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_48
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_48
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_UNSCII_8
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_unscii_8
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_UNSCII_16
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_unscii_16
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT12SUBPX
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_12_subpx
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT28COMPRESSED
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_28_compressed
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_DEJAVU_16_PERSIAN_HEBREW
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_dejavu_16_persian_hebrew
#elif defined CONFIG_LV_FONT_DEFAULT_TITLE_SIMSUN_16_CJK
#define CONFIG_LV_THEME_DEFAULT_FONT_TITLE         &lv_font_simsun_16_cjk
#endif
#endif

/*------------------
 * TEXT ENCODING
 *-----------------*/
#ifndef LV_TEXT_ENC
#ifdef CONFIG_LV_TXT_ENC_UTF8
#define CONFIG_LV_TXT_ENC  LV_TXT_ENC_UTF8
#elif defined CONFIG_LV_TXT_ENC_ASCII
#define CONFIG_LV_TXT_ENC  LV_TXT_ENC_ASCII
#endif
#endif

/*------------------
 * BIDI DIRECTION
 *-----------------*/

#ifndef LV_BIDI_BASE_DIR_DEF
#ifdef CONFIG_LV_BIDI_DIR_LTR
#define CONFIG_LV_BIDI_BASE_DIR_DEF LV_BIDI_DIR_LTR
#elif defined CONFIG_LV_BIDI_DIR_RTL
#define CONFIG_LV_BIDI_BASE_DIR_DEF LV_BIDI_DIR_RTL
#elif defined CONFIG_LV_BIDI_DIR_AUTO
#define CONFIG_LV_BIDI_BASE_DIR_DEF LV_BIDI_DIR_AUTO
#endif
#endif

/*------------------
 * EXT CLICK AREA
 *-----------------*/

#ifndef LV_USE_EXT_CLICK_AREA
#ifdef CONFIG_LV_USE_EXT_CLICK_AREA_OFF
#define CONFIG_LV_USE_EXT_CLICK_AREA  LV_EXT_CLICK_AREA_OFF
#elif defined CONFIG_LV_USE_EXT_CLICK_AREA_TINY
#define CONFIG_LV_USE_EXT_CLICK_AREA  LV_EXT_CLICK_AREA_TINY
#elif defined CONFIG_LV_USE_EXT_CLICK_AREA_FULL
#define CONFIG_LV_USE_EXT_CLICK_AREA  LV_EXT_CLICK_AREA_FULL
#endif
#endif

/*------------------
 * LINEMETER PRECISE
 *-----------------*/

#ifndef LV_LINEMETER_PRECISE
#ifdef CONFIG_LV_LINEMETER_PRECISE_NO_EXTRA_PRECISION
#define CONFIG_LV_LINEMETER_PRECISE   0
#elif defined CONFIG_LV_LINEMETER_PRECISE_SOME_EXTRA_PRECISION
#define CONFIG_LV_LINEMETER_PRECISE   1
#elif defined CONFIG_LV_LINEMETER_PRECISE_BEST_PRECISION
#define CONFIG_LV_LINEMETER_PRECISE   2
#endif
#endif


/*------------------
 * SPINNER DEF ANIM
 *-----------------*/

#ifndef LV_SPINNER_DEF_ANIM
#ifdef CONFIG_LV_SPINNER_TYPE_SPINNING_ARC
#define CONFIG_LV_SPINNER_DEF_ANIM   LV_SPINNER_TYPE_SPINNING_ARC
#elif defined CONFIG_LV_SPINNER_TYPE_FILLSPIN_ARC
#define CONFIG_LV_SPINNER_DEF_ANIM   LV_SPINNER_TYPE_FILLSPIN_ARC
#elif defined CONFIG_LV_SPINNER_TYPE_CONSTANT_ARC
#define CONFIG_LV_SPINNER_DEF_ANIM   LV_SPINNER_TYPE_CONSTANT_ARC
#endif
#endif

/*------------------
 * SPRINTF DISABLE FLOAT
 *-----------------*/

#if defined(CONFIG_LV_CONF_SKIP) || defined(LV_CONF_SKIP)
#  ifndef LV_SPRINTF_DISABLE_FLOAT
#    ifndef CONFIG_LV_SPRINTF_DISABLE_FLOAT
#      define LV_SPRINTF_DISABLE_FLOAT    0
#    else
#      define LV_SPRINTF_DISABLE_FLOAT    1
#    endif
#  endif
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CONF_KCONFIG_H*/
