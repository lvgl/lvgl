/**
 * @file lv_canvas_private.h
 *
 */

#ifndef LV_CANVAS_PRIVATE_H
#define LV_CANVAS_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../image/lv_image_private.h"
#include "lv_canvas.h"

#if LV_USE_CANVAS != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Canvas data */
struct lv_canvas_t {
    lv_image_t img;
    lv_draw_buf_t * draw_buf;
    lv_draw_buf_t static_buf;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_CANVAS != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CANVAS_PRIVATE_H*/
