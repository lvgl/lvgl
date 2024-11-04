/**
 * @file lv_nxp_elcdif.h
 * Driver for NXP's ELCD
 */

#ifndef LV_NXP_ELCDIF_DRIVER_H
#define LV_NXP_ELCDIF_DRIVER_H

#include "../../../lvgl.h"

#if LV_USE_NXP_ELCDIF == 1

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "fsl_elcdif.h"
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
 * API to create display for NXP processors by default. Sets internal buffers and callbacks and everything accordingly
 * @param config : NXP eLCD config
 * @param frame_buffer1 : handle to buffer 1
 * @param frame_buffer2 : handle to buffer 2
 * @param buf_size : frame buffer size in bytes
 * @param mode : LV_DISPLAY_RENDER_MODE (FULL, PARTIAL, DIRECT)
 * @param init_cb : LCD init callback : HW dependent
 */
lv_display_t * lv_nxp_display_elcdif_create(elcdif_rgb_mode_config_t config, void * frame_buffer1,
                                            void * frame_buffer2, size_t buf_size, lv_display_render_mode_t mode,
                                            void (init_cb)(void), lv_display_flush_cb_t flush_cb, lv_display_flush_wait_cb_t wait_cb);


/**
 * API to create a display in direct mode. Does NOT set the callbacks and does not initialize the LCD.
 * This function is pure LVGL
 * @param config : NXP eLCD config object
 * @param frame_buffer1 : handle to buffer 1
 * @param frame_buffer2 : handle to buffer 2
 * @param buf_size : buffer size in bytes
 */
lv_display_t * lv_nxp_display_elcdif_create_direct(elcdif_rgb_mode_config_t config, void * frame_buffer1,
                                                   void * frame_buffer2, size_t buf_size);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_NXP_ELCDIF*/

#endif /* LV_NXP_ELCDIF_DRIVER_H */
