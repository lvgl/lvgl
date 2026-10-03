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
    char * data;                    /*Copy of the text, so a property change can generate the bitmap again*/
    /*The encoded bars, one byte each: 0 for a light bar, 0xFF for a dark bar. The sizing
     *pass gives them to the fill, so one generation encodes the text one time. NULL if the
     *fill must encode the text.*/
    uint8_t * pattern;
    int32_t bar_count;              /*Bars `data` encodes to; 0 when it is not known and has to be encoded*/
    uint16_t scale;                 /*Pixel width of a single bar*/
    lv_dir_t direction;
    lv_barcode_encoding_t encoding;
    uint8_t tiled : 1;              /*Draw a one bar wide bitmap and let the image tiling repeat it*/
    uint8_t update_mode : 1;        /*lv_barcode_update_mode_t: when a property change is regenerated*/
    uint8_t needs_update : 1;       /*The bitmap is out of date; filled in on the next redraw (deferred mode)*/
    uint8_t render_valid : 1;       /*No generation failed since the last change or resize*/
    uint8_t fitting : 1;            /*Ignore the resize event that the reallocation of the canvas causes*/
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
