/**
 * @file lv_dev_stm32_fb.h
 *
 */

#ifndef LV_DEV_STM32_FB_H
#define LV_DEV_STM32_FB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../hal/lv_hal_disp.h"
#include "../../../drv/lv_drv.h"

#if LV_USE_DRV_SDL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_drv_sdl_disp_priv_t;
typedef struct {
    lv_coord_t hor_res;
    lv_coord_t ver_res;
    lv_drv_t * drv;
} lv_dev_fb_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_dev_stm32_fb_init(lv_dev_fb_t * dev);

lv_disp_t * lv_dev_stm32_fb_create(lv_dev_fb_t * dev);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DEV_STM32_FB */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DEV_STM32_FB_H */
