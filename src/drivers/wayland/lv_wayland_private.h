/*******************************************************************
 *
 * @file lv_wayland_private.h - Private functions of the 
 * LVGL Wayland client
 *
 * Based on the original file from the repository.
 *
 * Porting to LVGL 9.1
 * 2024 EDGEMTech Ltd.
 *
 * See LICENCE.txt for details
 *
 * Author(s): EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/
#ifndef LV_WAYLAND_PRIVATE_H
#define LV_WAYLAND_PRIVATE_H

#ifndef _WIN32

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_WAYLAND

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
 * Initializes the wayland client
 * @note must be called after lv_init
 */
void lv_wayland_init(void);

/**
 * De-Initializes the wayland client
 * @note Must be called on application exit
 */
void lv_wayland_deinit(void);

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _WIN32 */
#endif /* LV_WAYLAND_PRIVATE_H */
