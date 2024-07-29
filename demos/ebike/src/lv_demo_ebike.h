/**
 * @file lv_demo_ebike.h
 *
 */

#ifndef LV_DEMO_EBIKE_H
#define LV_DEMO_EBIKE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define EBIKE_COLOR_TURQUOISE   lv_color_hex(0x55FFEB)
#define EBIKE_COLOR_LIME        lv_color_hex(0x91FF3B)
#define EBIKE_PORTRAIT          0    /*0: for 480x270..480x320, 1: for 480x800..720x1280*/

#if EBIKE_PORTRAIT
LV_FONT_DECLARE(font_ebike_inter_20);
LV_FONT_DECLARE(font_ebike_trump_40);
LV_FONT_DECLARE(font_ebike_trump_72);
#define EBIKE_FONT_SMALL        &font_ebike_inter_20
#define EBIKE_FONT_MEDIUM       &font_ebike_trump_40
#define EBIKE_FONT_LARGE        &font_ebike_trump_72
#else
LV_FONT_DECLARE(font_ebike_inter_14);
LV_FONT_DECLARE(font_ebike_trump_24);
LV_FONT_DECLARE(font_ebike_trump_48);
#define EBIKE_FONT_SMALL        &font_ebike_inter_14
#define EBIKE_FONT_MEDIUM       &font_ebike_trump_24
#define EBIKE_FONT_LARGE        &font_ebike_trump_48
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_demo_ebike_create(void);

/**********************
 * GLOBAL VARIABLES
 **********************/
extern lv_subject_t ebike_subject_language;

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_EBIKE_H*/
