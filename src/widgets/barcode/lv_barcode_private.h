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
    char * data;                    /*Copy of the data, kept so the bitmap can be regenerated on a property change*/
    int32_t size;                   /*Length of the canvas along the bars, in pixels*/
    uint16_t scale;                 /*Pixel width of a single bar; 0 fits the bars to `size`*/
    lv_dir_t direction;             /*LV_DIR_HOR or LV_DIR_VER*/
    lv_barcode_encoding_t encoding;
    uint8_t update_mode : 1;        /*lv_barcode_update_mode_t: when a property change is regenerated*/
    uint8_t needs_update : 1;       /*The bitmap is out of date; regenerated on the next redraw (deferred mode)*/
    uint8_t render_valid : 1;       /*No generation attempt is known to have failed; a change re-arms it*/
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
