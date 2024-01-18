/**
 * @file lv_linux_fb_dev_h
 *
 */

#ifndef LV_LINUX_FB_DEV_H
#define LV_LINUX_FB_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../display/lv_display.h"

#if LV_USE_LINUX_FBDEV

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_display_t * lv_linux_fbdev_create(void);

void lv_linux_fbdev_set_file(lv_display_t * disp, const char * file);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LINUX_FBDEV */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_LINUX_FB_DEV_H */
