/**
 * @file lv_nxp_elcdif.h
 * Driver for NXP's ELCD
 */

#ifndef LV_NXP_ELCDIF_DRIVER_H
#define LV_NXP_ELCDIF_DRIVER_H

#include "../../../display/lv_display.h"

#if LV_USE_NXP_ELCDIF == 1

/*TODO [Q4][kissa96] : Add other headers*/
#include "../../../../../drivers/fsl_elcdif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * API to create display from elcdif_rgb_mode_config_t and other parameters
 * @param config        NXP display config object
 * @param frame_buffer1 Frame buffer1 or smaller render buffer1
 * @param frame_buffer2 Frame buffer2 or smaller render buffer2 (Can be NULL)
 * @param buf_size      Size of render buffers in bytes
 * @param mode          LVGL render mode
 */
lv_display_t * lv_nxp_elcdif_create_from_config(elcdif_rgb_mode_config_t * config, void * frame_buffer1,
                                                void * frame_buffer2, uint32_t buf_size, const lv_display_render_mode_t mode);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_NXP_ELCDIF*/

#endif /* LV_NXP_ELCDIF_DRIVER_H */
