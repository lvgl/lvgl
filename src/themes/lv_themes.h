/**
 * @file lv_themes.h
 *
 */

#ifndef LV_THEMES_H
#define LV_THEMES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if LV_USE_THEME_DEFAULT
#include "default/lv_theme_default.h"
#endif /* LV_USE_THEME_DEFAULT */
#if LV_USE_THEME_MONO
#include "mono/lv_theme_mono.h"
#endif /* LV_USE_THEME_MONO */
#if LV_USE_THEME_BASIC
#include "basic/lv_theme_basic.h"
#endif /* LV_USE_THEME_BASIC */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_THEMES_H*/
