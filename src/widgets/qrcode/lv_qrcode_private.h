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

#include "../../lvgl_public.h"

#if LV_USE_QRCODE

#include "../../widgets/canvas/lv_canvas_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of qrcode*/
struct _lv_qrcode_t {
    lv_canvas_t canvas;
    lv_color_t dark_color;
    lv_color_t light_color;
    uint8_t * data;                 /*Copy of the payload, kept so the bitmap can be re-encoded on a property change*/
    /*The setters cap the payload at qrcodegen_BUFFER_LEN_MAX (3918), so 12 bits (up to
     *4095) cover any length and the four flags fill the rest of the 16-bit word.*/
    uint16_t data_len : 12;         /*Stored payload length in bytes*/
    uint16_t quiet_zone : 1;        /*Add the QR spec's blank margin around the code (boolean toggle)*/
    uint16_t update_mode : 1;       /*lv_qrcode_update_mode_t: when a property change is re-encoded*/
    uint16_t needs_update : 1;      /*The bitmap is out of date; re-encoded on the next redraw (deferred mode)*/
    uint16_t render_failed : 1;     /*The last encode attempt failed (or none has run yet)*/
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
