/**
 * @file lv_nuttx_mouse.h
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef LV_NUTTX_MOUSE_H
#define LV_NUTTX_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_public_api.h"

#if LV_USE_NUTTX

#if LV_USE_NUTTX_MOUSE

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
 * Initialize indev with specified input device.
 * @param dev_path      path of input device
 */
lv_indev_t * lv_nuttx_mouse_create(const char * dev_path);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX_MOUSE */

#endif /* LV_USE_NUTTX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_MOUSE_H */
