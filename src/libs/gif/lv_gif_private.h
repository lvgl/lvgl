/**
 * @file lv_gif_private.h
 *
 */

#ifndef LV_GIF_PRIVATE_H
#define LV_GIF_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../widgets/image/lv_image_private.h"
#include "lv_gif.h"

#if LV_USE_GIF

#include "AnimatedGIF/src/AnimatedGIF.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_gif_t {
    lv_image_t img;
    GIFIMAGE gif;
    lv_timer_t * timer;
    lv_image_dsc_t imgdsc;
    int32_t loop_count;
    uint32_t is_open : 1;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_GIF */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GIF_PRIVATE_H*/
