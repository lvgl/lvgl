/**
 * @file lv_evdev_private.h
 *
 */

#ifndef LV_EVDEV_PRIVATE_H
#define LV_EVDEV_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/


#include "../../lvgl_public.h"

#if LV_USE_EVDEV

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_evdev_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_EVDEV*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_EVDEV_PRIVATE_H*/
