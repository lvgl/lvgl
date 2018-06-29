/**
 * @file lv_font.h
 *
 */

#ifndef LV_DEFAULT_FONTS_H
#define LV_DEFAULT_FONTS_H

#ifdef __cplusplus
extern "C" {
#endif

void lv_font_add_default_fonts(void);

/******************************
 *  FONT DECLARATION INCLUDES
 *****************************/

/*10 px */
#if USE_LV_FONT_DEJAVU_10
LV_FONT_DECLARE(lv_font_dejavu_10);
#endif

#if USE_LV_FONT_DEJAVU_10_LATIN_SUP
LV_FONT_DECLARE(lv_font_dejavu_10_latin_sup);
#endif

#if USE_LV_FONT_DEJAVU_10_CYRILLIC
LV_FONT_DECLARE(lv_font_dejavu_10_cyrillic);
#endif

#if USE_LV_FONT_SYMBOL_10
LV_FONT_DECLARE(lv_font_symbol_10);
#endif

/*20 px */
#if USE_LV_FONT_DEJAVU_20
LV_FONT_DECLARE(lv_font_dejavu_20);
#endif

#if USE_LV_FONT_DEJAVU_20_LATIN_SUP
LV_FONT_DECLARE(lv_font_dejavu_20_latin_sup);
#endif

#if USE_LV_FONT_DEJAVU_20_CYRILLIC
LV_FONT_DECLARE(lv_font_dejavu_20_cyrillic);
#endif

#if USE_LV_FONT_SYMBOL_20
LV_FONT_DECLARE(lv_font_symbol_20);
#endif

/*30 px */
#if USE_LV_FONT_DEJAVU_30
LV_FONT_DECLARE(lv_font_dejavu_30);
#endif

#if USE_LV_FONT_DEJAVU_30_LATIN_SUP
LV_FONT_DECLARE(lv_font_dejavu_30_latin_sup);
#endif

#if USE_LV_FONT_DEJAVU_30_CYRILLIC
LV_FONT_DECLARE(lv_font_dejavu_30_cyrillic);
#endif

#if USE_LV_FONT_SYMBOL_30
LV_FONT_DECLARE(lv_font_symbol_30);
#endif

/*40 px */
#if USE_LV_FONT_DEJAVU_40
LV_FONT_DECLARE(lv_font_dejavu_40);
#endif

#if USE_LV_FONT_DEJAVU_40_LATIN_SUP
LV_FONT_DECLARE(lv_font_dejavu_40_latin_sup);
#endif

#if USE_LV_FONT_DEJAVU_40_CYRILLIC
LV_FONT_DECLARE(lv_font_dejavu_40_cyrillic);
#endif

#if USE_LV_FONT_SYMBOL_40
LV_FONT_DECLARE(lv_font_symbol_40);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
