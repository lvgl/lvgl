/**
 * @file lv_conf_kconfig.h
 * Configs that needs special handling when LVGL is used with Kconfig
 */

#ifndef LV_CONF_KCONFIG_H
#define LV_CONF_KCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************
 * THEME SELECTION
 *******************/
 
#ifndef LV_THEME_DEFAULT_INIT
 #if defined (CONFIG_LV_THEME_DEFAULT_INIT_EMPTY)
  #define LV_THEME_DEFAULT_INIT               lv_theme_empty_init
 #elif defined (CONFIG_LV_THEME_DEFAULT_INIT_TEMPLATE)
  #define LV_THEME_DEFAULT_INIT               lv_theme_template_init
 #elif defined (CONFIG_LV_THEME_DEFAULT_INIT_MATERIAL)
  #define LV_THEME_DEFAULT_INIT               lv_theme_material_init
 #elif defined (CONFIG_LV_THEME_DEFAULT_INIT_MONO)
  #define LV_THEME_DEFAULT_INIT               lv_theme_mono_init
 #endif
#endif

/*******************
 * COLOR SELECTION
 *******************/
 
#ifdef LV_THEME_DEFAULT_COLOR_PRIMARY
 #ifdef CONFIG_LV_THEME_DEFAULT_COLOR_PRIMARY
  #define LV_THEME_DEFAULT_COLOR_PRIMARY lv_color_hex(CONFIG_LV_THEME_DEFAULT_COLOR_PRIMARY) 
 #endif
#endif
 
#ifdef LV_THEME_DEFAULT_COLOR_SECONDARY
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
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_8
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_10
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_10
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_12
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_12
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_14
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_14
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_16
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_16
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_18
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_18
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_20
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_20
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_22
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_22
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_24
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_24
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_26
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_26
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_28
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_28
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_30
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_30
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_32
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_32
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_34
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_34
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_36
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_36
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_38
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_38
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_40
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_40
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_42
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_42
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_44
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_44
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_46
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_46
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT_48
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_48
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_UNSCII_8
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_unscii_8
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT12SUBPX
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_12_subpx
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_MONTSERRAT28COMPRESSED
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_28_compressed
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_DEJAVU_16_PERSIAN_HEBREW
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_dejavu_16_persian_hebrew
 #elif defined CONFIG_LV_FONT_DEFAULT_SMALL_SIMSUN_16_CJK
  #define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_simsun_16_cjk
 #endif
#endif

/*------------------
 * NORMAL FONT
 *-----------------*/
#ifndef LV_THEME_DEFAULT_FONT_NORMAL
 #if defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_8
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_8
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_10
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_10
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_12
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_12
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_14
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_14
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_16
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_16
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_18
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_18
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_20
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_20
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_22
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_22
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_24
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_24
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_26
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_26
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_28
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_28
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_30
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_30
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_32
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_32
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_34
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_34
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_36
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_36
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_38
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_38
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_40
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_40
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_42
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_42
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_44
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_44
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_46
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_46
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT_48
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_48
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_UNSCII_8
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_unscii_8
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT12SUBPX
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_12_subpx
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_MONTSERRAT28COMPRESSED
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_montserrat_28_compressed
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_DEJAVU_16_PERSIAN_HEBREW
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_dejavu_16_persian_hebrew
 #elif defined CONFIG_LV_FONT_DEFAULT_NORMAL_SIMSUN_16_CJK
  #define LV_THEME_DEFAULT_FONT_NORMAL         &lv_font_simsun_16_cjk
 #endif
#endif

/*------------------
 * SUBTITLE FONT
 *-----------------*/
#ifndef LV_THEME_DEFAULT_FONT_SUBTITLE
 #if defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_8
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_8
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_10
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_10
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_12
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_12
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_14
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_14
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_16
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_16
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_18
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_18
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_20
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_20
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_22
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_22
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_24
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_24
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_26
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_26
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_28
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_28
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_30
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_30
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_32
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_32
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_34
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_34
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_36
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_36
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_38
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_38
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_40
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_40
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_42
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_42
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_44
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_44
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_46
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_46
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT_48
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_48
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_UNSCII_8
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_unscii_8
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT12SUBPX
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_12_subpx
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_MONTSERRAT28COMPRESSED
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_montserrat_28_compressed
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_DEJAVU_16_PERSIAN_HEBREW
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_dejavu_16_persian_hebrew
 #elif defined CONFIG_LV_FONT_DEFAULT_SUBTITLE_SIMSUN_16_CJK
  #define LV_THEME_DEFAULT_FONT_SUBTITLE         &lv_font_simsun_16_cjk
 #endif
#endif

/*------------------
 * TITLE FONT
 *-----------------*/
#ifndef LV_THEME_DEFAULT_FONT_TITLE
 #if defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_8
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_8
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_10
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_10
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_12
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_12
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_14
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_14
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_16
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_16
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_18
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_18
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_20
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_20
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_22
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_22
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_24
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_24
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_26
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_26
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_28
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_28
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_30
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_30
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_32
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_32
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_34
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_34
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_36
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_36
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_38
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_38
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_40
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_40
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_42
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_42
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_44
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_44
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_46
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_46
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT_48
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_48
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_UNSCII_8
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_unscii_8
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT12SUBPX
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_12_subpx
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_MONTSERRAT28COMPRESSED
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_28_compressed
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_DEJAVU_16_PERSIAN_HEBREW
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_dejavu_16_persian_hebrew
 #elif defined CONFIG_LV_FONT_DEFAULT_TITLE_SIMSUN_16_CJK
  #define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_simsun_16_cjk
 #endif
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CONF_KCONFIG_H*/
