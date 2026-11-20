/**
 * @file lv_image_encoder_private.h
 *
 */

#ifndef LV_IMAGE_ENCODER_PRIVATE_H
#define LV_IMAGE_ENCODER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the image encoder module
 */
void lv_image_encoder_init(void);

/**
 * Deinitialize the image encoder module
 */
void lv_image_encoder_deinit(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_ENCODER_PRIVATE_H*/
