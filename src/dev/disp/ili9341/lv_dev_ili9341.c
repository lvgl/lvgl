/**
 * @file lv_dev_ili9341.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_dev_ili9341.h"
#if LV_USE_DEV_ILI9341

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static void dev_init(lv_dev_ili9341_t * dev);
static void update_cb(lv_disp_drv_t * disp_drv);
static void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * colorp);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_dev_ili9341_init(lv_dev_ili9341_t * dev)
{
    lv_memset_00(dev, sizeof(lv_dev_ili9341_t));
    dev->hor_res = 320;
    dev->ver_res = 240;
}

lv_disp_t * lv_dev_ili9341_create(lv_dev_ili9341_t * dev)
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

    dev_init(dev);

    lv_disp_draw_buf_init(draw_buf, dev->buf1, dev->buf2, dev->buf_size);

    lv_disp_drv_init(disp_drv);
    disp_drv->draw_buf = draw_buf;
    disp_drv->hor_res = dev->hor_res;
    disp_drv->ver_res = dev->ver_res;
    disp_drv->flush_cb = flush_cb;
    disp_drv->drv_update_cb = update_cb;
    disp_drv->user_data = dev;
    lv_disp_t * disp = lv_disp_drv_register(disp_drv);

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void dev_init(lv_dev_ili9341_t * dev)
{
    lv_drv_t * drv = dev->drv;

    lv_drv_reset_dev(drv);

    /*Example just to show the flow*/
    uint8_t cmd[4] = {1, 2, 3, 4};
    uint16_t params[4] = {30, 31, 32, 33};
    lv_drv_send_cmd(drv, cmd[0], &params[0], 2);
    lv_drv_send_cmd(drv, cmd[1], &params[1], 2);
    lv_drv_send_cmd(drv, cmd[2], &params[2], 2);
    lv_drv_send_cmd(drv, cmd[3], &params[3], 2);

}

static void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_dev_ili9341_t * dev = disp_drv->user_data;
    lv_drv_t * drv = dev->drv;

    /*Example just to show the flow*/
    uint16_t window[4] = {area->x1, area->y1, area->x2, area->y2};
    lv_drv_send_cmd(drv, 100, &window[0], 2);
    lv_drv_send_cmd(drv, 101, &window[1], 2);
    lv_drv_send_cmd(drv, 102, &window[2], 2);
    lv_drv_send_cmd(drv, 103, &window[3], 2);

    lv_drv_disp_flush(drv, disp_drv, area, color_p);
}


static void update_cb(lv_disp_drv_t * disp_drv)
{
    lv_dev_ili9341_t * dev = disp_drv->user_data;
    lv_drv_t * drv = dev->drv;

    /*Example just to show the flow*/
    if(disp_drv->rotated == LV_DISP_ROT_90) {
        uint16_t param = 234;
        lv_drv_send_cmd(drv, 400, param, 2);
    }
    else {
        uint16_t param = 234;
        lv_drv_send_cmd(drv, 401, param, 2);
    }
}

#endif
