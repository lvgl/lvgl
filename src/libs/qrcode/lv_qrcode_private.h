/**
 * @file lv_qrcode_private.h
 *
 */

#ifndef LV_QRCODE_PRIVATE_H
#define LV_QRCODE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../widgets/canvas/lv_canvas_private.h"
#include "lv_qrcode.h"

#if LV_USE_QRCODE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of qrcode*/
struct lv_qrcode_t {
    lv_canvas_t canvas;
    lv_color_t dark_color;
    lv_color_t light_color;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_QRCODE */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_QRCODE_PRIVATE_H*/
