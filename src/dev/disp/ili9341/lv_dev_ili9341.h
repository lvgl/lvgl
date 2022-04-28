/**
 * @file lv_dev_ili9341.h
 *
 */

#ifndef LV_DEV_ILI9341_H
#define LV_DEV_ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../drv/lv_drv.h"
#if LV_USE_DEV_ILI9341

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_drv_ili9341_dsc_priv_t;
typedef struct {
    lv_drv_t * drv;
    lv_coord_t hor_res;
    lv_coord_t ver_res;
    void  * buf1;
    void  * buf2;
    uint32_t buf_size;
} lv_dev_ili9341_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_dev_ili9341_init(lv_dev_ili9341_t * dev);

lv_disp_t * lv_dev_ili9341_create(lv_dev_ili9341_t * dev);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRV_SDL_DISP_H */
