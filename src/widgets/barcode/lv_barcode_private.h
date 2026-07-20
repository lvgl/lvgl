/**
 * @file lv_barcode_private.h
 *
 */

#ifndef LV_BARCODE_PRIVATE_H
#define LV_BARCODE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_BARCODE

#include "../../widgets/canvas/lv_canvas_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of barcode*/
struct _lv_barcode_t {
    lv_canvas_t canvas;
    lv_color_t dark_color;
    lv_color_t light_color;
    char * data;            /*Copy of the payload string (NUL terminated)*/
    lv_dir_t direction;
    lv_barcode_encoding_t encoding;
    /*`scale` is the integer pixel width of each bar - always a small number, so 12 bits
     *(max 4095) is far more than enough and leaves room to pack the flags into the same word.*/
    uint16_t scale : 12;
    uint16_t tiled : 1;
    uint16_t auto_update : 1;    /*Draw the bitmap right away on every change (default: true)*/
    uint16_t needs_draw : 1;     /*The canvas is sized but its pixels are out of date (draw pending)*/
    uint16_t resizing : 1;       /*Guard against re-entrant resize (resize -> SIZE_CHANGED -> resize)*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_BARCODE */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BARCODE_PRIVATE_H*/
