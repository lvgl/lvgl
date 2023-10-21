/**
 * @file lv_nuttx_libuv.h
 *
 */

#ifndef LV_NUTTX_LIBUV_H
#define LV_NUTTX_LIBUV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_NUTTX

#if LV_USE_NUTTX_LIBUV

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void * loop;
    lv_display_t * disp;
    lv_indev_t * indev;
} lv_nuttx_uv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the uv_loop using the provided configuration information.
 * @param uv_info Pointer to the lv_nuttx_uv_t structure to be initialized.
 */
void * lv_nuttx_uv_init(lv_nuttx_uv_t * uv_info);

/**
 * Deinitialize the uv_loop configuration for NuttX porting layer.
 * @param data Pointer to user data.
 */
void lv_nuttx_uv_deinit(void ** data);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_NUTTX_LIBUV*/

#endif /*LV_USE_NUTTX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_NUTTX_LIBUV_H*/
