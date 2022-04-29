/**
 * @file lv_dev_stm32_fb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_dev_fb.h"
#if LV_USE_DRV_SDL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*These 3 functions are needed by LittlevGL*/
static void ex_disp_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

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
    dev->hor_res = 800;
    dev->ver_res = 480;
}

lv_disp_t * lv_dev_fb_create(lv_dev_fb_t * dev)
{
    lv_disp_drv_t * disp_drv = lv_mem_alloc(sizeof(lv_disp_drv_t));
    LV_ASSERT_MALLOC(disp_drv);

    lv_disp_draw_buf_t * draw_buf = lv_mem_alloc(sizeof(lv_disp_draw_buf_t));
    LV_ASSERT_MALLOC(draw_buf);

    if(disp_drv == NULL || draw_buf == NULL) {
        lv_mem_free(disp_drv);
        lv_mem_free(draw_buf);
        return NULL;
    }

    lv_disp_draw_buf_init(draw_buf, dev->buf1, dev->buf2, dev->buf_size);

    lv_disp_drv_init(disp_drv);
    disp_drv->draw_buf = draw_buf;
    disp_drv->hor_res = dev->hor_res;
    disp_drv->ver_res = dev->ver_res;
    disp_drv->flush_cb = flush_cb;
    disp_drv->user_data = dev;
    lv_disp_t * disp = lv_disp_drv_register(disp_drv);

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_drv_t * drv = disp_drv->user_data;

    lv_drv_disp_flush(drv, disp_drv, area, color_p);
}

#endif /*LV_DRV_FB*/
