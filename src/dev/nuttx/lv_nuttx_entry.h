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

lv_display_t * lv_nuttx_init(lv_nuttx_t * info);

#if LV_USE_NUTTX_CUSTOM_INIT
lv_display_t * lv_nuttx_init_custom(lv_nuttx_t * info);
#endif

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_ENTRY_H */
