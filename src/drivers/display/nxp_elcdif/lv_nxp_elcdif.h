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
 * Attach LVGL to ELCDIF using DIRECT rendering mode.
 * ELCDIF should be already initialized.
 * @param config            NXP eLCD config object
 * @param frame_buffer1     pointer the first frame buffers
 * @param frame_buffer2     pointer the second frame buffers
 * @param buf_size          size of a buffer in bytes
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
