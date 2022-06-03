/**
 * @file lv_dev_stm32_fb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_dev_fb.h"
#if LV_USE_DEV_FB

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_dev_fb_init(lv_dev_fb_t * dev)
{
    lv_memset_00(dev, sizeof(lv_dev_fb_t));
}

lv_disp_t * lv_dev_fb_create(lv_dev_fb_t * dev, lv_disp_drv_t * disp_drv)
{
    disp_drv->flush_cb = flush_cb;
    disp_drv->hor_res = dev->hor_res;
    disp_drv->ver_res = dev->ver_res;
    lv_disp_t * disp = lv_disp_drv_register(disp_drv);
    disp->user_data = dev;

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_drv_t * drv = disp_drv->user_data;

    lv_drv_send_image(drv, disp_drv, area, color_p);
}

#endif /*LV_DRV_FB*/
