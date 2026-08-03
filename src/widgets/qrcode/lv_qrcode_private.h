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
    uint8_t * data;                 /*Copy of the payload; strings are stored with their trailing NUL, binary as-is*/
    /*The setters cap the stored length at qrcodegen_BUFFER_LEN_MAX (3918), so 12 bits (max 4095)
     *cover it and leave room to pack the flags into the same word.*/
    uint16_t data_len : 12;         /*Stored length in bytes (a string includes its NUL terminator)*/
    uint16_t quiet_zone : 1;        /*Add the QR spec's blank margin around the code (boolean toggle)*/
    uint16_t auto_update : 1;       /*Regenerate the bitmap right away on every change (default: true)*/
    uint16_t needs_update : 1;      /*The bitmap is out of date and must be regenerated (manual mode only)*/
    uint16_t render_failed : 1;     /*The last bitmap generation failed (or none has run yet)*/
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
