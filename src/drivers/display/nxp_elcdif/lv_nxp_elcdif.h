/**
 * @file lv_nxp_elcdif.h
 * Driver for NXP's ELCD
 */

#ifndef LV_NXP_ELCDIF_DRIVER_H
#define LV_NXP_ELCDIF_DRIVER_H

#include "../../../display/lv_display.h"

#if LV_USE_NXP_ELCDIF == 1

#include "../../../../../drivers/fsl_elcdif.h"

#ifdef __cplusplus
extern "C" {
#endif

lv_display_t * lv_nxp_elcdif_create_from_config(elcdif_rgb_mode_config_t * config, void * frame_buffer1,
                                                void * frame_buffer2, uint32_t buf_size, const lv_display_render_mode_t mode, lv_display_flush_cb_t flush_cb,
                                                lv_display_set_flush_wait_cb_t wait_cb);
lv_display_t * lv_nxp_elcdif_create_partial(elcdif_rgb_mode_config_t * config, void * frame_buffer1,
                                            void * frame_buffer2, uint32_t buf_size, lv_display_flush_cb_t flush_cb, lv_display_set_flush_wait_cb_t wait_cb);
lv_display_t * lv_nxp_elcdif_create_direct(elcdif_rgb_mode_config_t * config, void * frame_buffer1,
                                           void * frame_buffer2, uint32_t buf_size, lv_display_flush_cb_t flush_cb, lv_display_set_flush_wait_cb_t wait_cb);
lv_display_t * lv_nxp_elcdif_create_full(elcdif_rgb_mode_config_t * config, void * frame_buffer1, void * frame_buffer2,
                                         uint32_t buf_size, lv_display_flush_cb_t flush_cb, lv_display_set_flush_wait_cb_t wait_cb);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_NXP_ELCDIF*/

#endif /* LV_NXP_ELCDIF_DRIVER_H */
