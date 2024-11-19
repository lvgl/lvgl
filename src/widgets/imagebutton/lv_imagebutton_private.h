/**
 * @file lv_imagebutton_private.h
 *
 */

#ifndef LV_IMAGEBUTTON_PRIVATE_H
#define LV_IMAGEBUTTON_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_imagebutton.h"

#if LV_USE_IMAGEBUTTON != 0
#include "../../core/lv_obj_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_imagebutton_src_info_t {
    const void * img_src;
    lv_image_header_t header;
};

/** Data of image button */
struct _lv_imagebutton_t {
    lv_obj_t obj;
    lv_imagebutton_src_info_t src_mid[LV_IMAGEBUTTON_STATE_NUM];   /**< Store center images to each state */
    lv_imagebutton_src_info_t src_left[LV_IMAGEBUTTON_STATE_NUM];  /**< Store left side images to each state */
    lv_imagebutton_src_info_t src_right[LV_IMAGEBUTTON_STATE_NUM]; /**< Store right side images to each state */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_IMAGEBUTTON != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGEBUTTON_PRIVATE_H*/
