/**
 * @file lv_nuttx_entry.h
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef LV_NUTTX_ENTRY_H
#define LV_NUTTX_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_NUTTX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const char * fb_path;
    const char * input_path;
    const char * utouch_path;
} lv_nuttx_dsc_t;

typedef struct {
    lv_display_t * disp;
    lv_indev_t * indev;
    lv_indev_t * utouch_indev;
} lv_nuttx_result_t;

typedef struct _lv_nuttx_ctx_t {

#if LV_CACHE_DEF_SIZE > 0
    void * image_cache;
#endif

} lv_nuttx_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the lv_nuttx_dsc_t structure with default values for the NuttX port of LVGL.
 * @param dsc Pointer to the lv_nuttx_dsc_t structure to be initialized.
 */
void lv_nuttx_dsc_init(lv_nuttx_dsc_t * dsc);

/**
 * Initialize the LVGL display driver for NuttX using the provided configuration information.
 * @param dsc Pointer to the lv_nuttx_dsc_t structure containing the configuration information for the display driver.
 * @param result Pointer to the lv_nuttx_result_t structure containing display and input device handler.
 */
void lv_nuttx_init(const lv_nuttx_dsc_t * dsc, lv_nuttx_result_t * result);

#if LV_USE_NUTTX_CUSTOM_INIT
/**
 * Initialize the LVGL display driver for NuttX using the provided custom configuration information.
 * @param dsc Pointer to the lv_nuttx_dsc_t structure containing the custom configuration for the display driver.
 * @param result Pointer to the lv_nuttx_result_t structure containing display and input device handler.
 */
void lv_nuttx_init_custom(const lv_nuttx_dsc_t * dsc, lv_nuttx_result_t * result);

#endif /* LV_USE_NUTTX_CUSTOM_INIT */

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_ENTRY_H */
