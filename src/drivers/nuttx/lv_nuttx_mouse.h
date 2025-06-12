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

#include "../../lv_conf_internal.h"
#include "../../indev/lv_indev.h"

#ifdef CONFIG_LV_USE_NUTTX_MOUSE

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

#endif /* CONFIG_LV_USE_NUTTX_MOUSE */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_MOUSE_H */
