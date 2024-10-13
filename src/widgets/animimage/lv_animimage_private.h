/**
 * @file lv_animimage_private.h
 *
 */

#ifndef LV_ANIMIMAGE_PRIVATE_H
#define LV_ANIMIMAGE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../image/lv_image_private.h"
#include "../../misc/lv_anim_private.h"
#include "lv_animimage.h"

#if LV_USE_ANIMIMG != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Data of the animimage */
struct _lv_animimg_t {
    lv_image_t img;
    lv_anim_t anim;
    /* picture sequence */
    const void ** dsc;
    int8_t  pic_count;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_ANIMIMG != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIMIMAGE_PRIVATE_H*/
