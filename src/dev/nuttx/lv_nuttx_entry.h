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
    bool need_wait_vsync;
} lv_nuttx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the lv_nuttx_t structure with default values for the NuttX port of LVGL.
 * @param info Pointer to the lv_nuttx_t structure to be initialized.
 */
void lv_nuttx_info_init(lv_nuttx_t * info);

/**
 * Initialize the LVGL display driver for NuttX using the provided configuration information.
 * @param info Pointer to the lv_nuttx_t structure containing the configuration information for the display driver.
 * @return Pointer to the lv_display_t structure representing the initialized display driver.
 */
lv_display_t * lv_nuttx_init(const lv_nuttx_t * info);

#if LV_USE_NUTTX_CUSTOM_INIT
/**
 * Initialize the LVGL display driver for NuttX using the provided custom configuration information.
 * @param info Pointer to the lv_nuttx_t structure containing the custom configuration information for the display driver.
 * @return Pointer to the lv_display_t structure representing the initialized display driver.
 */
lv_display_t * lv_nuttx_init_custom(const lv_nuttx_t * info);
#endif /* LV_USE_NUTTX_CUSTOM_INIT */

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_ENTRY_H */
