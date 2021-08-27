/** * @file lv_conf_kconfig.h * Configs that need special handling when LVGL is used with Kconfig */

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
#if defined (CONFIG_LV_MEM_SIZE_KILOBYTES)
#define CONFIG_LV_MEM_SIZE    (CONFIG_LV_MEM_SIZE_KILOBYTES * 1024U)
#endif
#endif

/*******************
 * LV COLOR CHROMA KEY
 *******************/

#ifndef LV_COLOR_CHROMA_KEY
#if defined (CONFIG_LV_COLOR_CHROMA_KEY_HEX)
#define CONFIG_LV_COLOR_CHROMA_KEY  lv_color_hex(CONFIG_LV_COLOR_CHROMA_KEY_HEX)
#else
#define CONFIG_LV_COLOR_CHROMA_KEY  lv_color_hex(0x00ff00)  /* pure green */
#endif
#endif

/********************
 * FONT SELECTION
 *******************/

/**
 * NOTE: In Kconfig instead of `LV_THEME_DEFAULT_FONT`
 *       `CONFIG_LV_THEME_DEFAULT_FONT_<font_name>` is defined
 *       hence the large selection with if-s
 */

/*------------------
 * DEFAULT FONT
 *-----------------*/
#ifndef LV_THEME_DEFAULT_FONT
#if defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_8
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_8
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_10
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_10
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_12
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_12
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_14
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_14
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_16
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_16
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_18
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_18
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_20
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_20
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_22
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_22
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_24
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_24
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_26
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_26
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_28
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_28
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_30
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_30
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_32
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_32
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_34
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_34
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_36
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_36
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_38
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_38
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_40
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_40
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_42
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_42
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_44
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_44
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_46
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_46
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT_48
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_48
#elif defined CONFIG_LV_FONT_DEFAULT_UNSCII_8
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_unscii_8
#elif defined CONFIG_LV_FONT_DEFAULT_UNSCII_16
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_unscii_16
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT12SUBPX
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_12_subpx
#elif defined CONFIG_LV_FONT_DEFAULT_MONTSERRAT28COMPRESSED
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_montserrat_28_compressed
#elif defined CONFIG_LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_dejavu_16_persian_hebrew
#elif defined CONFIG_LV_FONT_DEFAULT_SIMSUN_16_CJK
#define CONFIG_LV_THEME_DEFAULT_FONT         &lv_font_simsun_16_cjk
#endif
#endif

/*------------------
 * TEXT ENCODING
 *-----------------*/
#ifndef LV_TEXT_ENC
#ifdef CONFIG_LV_TXT_ENC_UTF8
#define CONFIG_LV_TXT_ENC LV_TXT_ENC_UTF8
#elif defined CONFIG_LV_TXT_ENC_ASCII
#define CONFIG_LV_TXT_ENC LV_TXT_ENC_ASCII
#endif
#endif

/*------------------
 * BIDI DIRECTION
 *-----------------*/

#ifndef LV_BIDI_BASE_DIR_DEF
#ifdef CONFIG_LV_BASE_DIR_LTR
#define CONFIG_LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_LTR
#elif defined CONFIG_LV_BASE_DIR_RTL
#define CONFIG_LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_RTL
#elif defined CONFIG_LV_BASE_DIR_AUTO
#define CONFIG_LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CONF_KCONFIG_H*/
