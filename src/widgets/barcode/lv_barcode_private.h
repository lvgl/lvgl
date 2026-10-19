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

    /** Copy of the data, kept so the bitmap can be regenerated on a property change*/
    char * data;

    /** LV_DIR_HOR or LV_DIR_VER*/
    lv_dir_t direction;

    /** Which barcode encoding to use */
    lv_barcode_encoding_t encoding;

    /** Width of each bar in pixels*/
    uint16_t scale;
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
