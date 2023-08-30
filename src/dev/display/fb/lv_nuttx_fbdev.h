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

#include "../../../display/lv_display.h"

#if LV_USE_NUTTX_FBDEV

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

#endif /* LV_USE_NUTTX_FBDEV */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_FBDEV_H */
