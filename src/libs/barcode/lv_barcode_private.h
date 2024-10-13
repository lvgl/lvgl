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

#include "../../widgets/canvas/lv_canvas_private.h"
#include "lv_barcode.h"

#if LV_USE_BARCODE

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
    uint16_t scale;
    lv_dir_t direction;
    bool tiled;
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
