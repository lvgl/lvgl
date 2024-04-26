/**
 * @file lv_palette.h
 *
 */

#ifndef LV_PALETTE_H
#define LV_PALETTE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_color.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_PALETTE_RED,
    LV_PALETTE_PINK,
    LV_PALETTE_PURPLE,
    LV_PALETTE_DEEP_PURPLE,
    LV_PALETTE_INDIGO,
    LV_PALETTE_BLUE,
    LV_PALETTE_LIGHT_BLUE,
    LV_PALETTE_CYAN,
    LV_PALETTE_TEAL,
    LV_PALETTE_GREEN,
    LV_PALETTE_LIGHT_GREEN,
    LV_PALETTE_LIME,
    LV_PALETTE_YELLOW,
    LV_PALETTE_AMBER,
    LV_PALETTE_ORANGE,
    LV_PALETTE_DEEP_ORANGE,
    LV_PALETTE_BROWN,
    LV_PALETTE_BLUE_GREY,
    LV_PALETTE_GREY,
    LV_PALETTE_LAST,
    LV_PALETTE_NONE = 0xff,
} lv_palette_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*Source: https://vuetifyjs.com/en/styles/colors/#material-colors*/

lv_color_t lv_palette_main(lv_palette_t p);
lv_color_t lv_palette_lighten(lv_palette_t p, uint8_t lvl);
lv_color_t lv_palette_darken(lv_palette_t p, uint8_t lvl);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PALETTE_H*/
