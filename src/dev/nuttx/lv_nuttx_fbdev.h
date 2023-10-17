/**
 * @file lv_nuttx_fbdev_h
 *
 */

#ifndef LV_NUTTX_FBDEV_H
#define LV_NUTTX_FBDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_nuttx_entry.h"

#if LV_USE_NUTTX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_display_t * lv_nuttx_fbdev_create(void);

int lv_nuttx_fbdev_set_file(lv_display_t * disp, const char * file);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_FBDEV_H */
