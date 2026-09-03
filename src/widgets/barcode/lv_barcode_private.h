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
    char * data;                    /*Copy of the payload, kept so the bitmap can be regenerated on a property change*/
    /*The encoded bars, one byte each - 0 for a light bar, 0xFF for a dark one - handed
     *from the sizing pass to the fill so that one regeneration never encodes the payload
     *twice. NULL when the fill has to encode it itself.*/
    uint8_t * pattern;
    int32_t bar_count;              /*Bars `data` encodes to; 0 when it is not known and has to be encoded*/
    uint16_t scale;                 /*Pixel width of a single bar*/
    lv_dir_t direction;
    lv_barcode_encoding_t encoding;
    uint8_t tiled : 1;              /*Draw a one bar wide bitmap and let the image tiling repeat it*/
    uint8_t update_mode : 1;        /*lv_barcode_update_mode_t: when a property change is regenerated*/
    uint8_t needs_update : 1;       /*The bitmap is out of date; filled in on the next redraw (deferred mode)*/
    uint8_t render_valid : 1;       /*No generation attempt is known to have failed; a change re-arms it*/
    uint8_t fitting : 1;            /*Guard against the re-entrant resize our own reallocation triggers*/
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
