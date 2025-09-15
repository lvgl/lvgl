/**
 * @file lv_lovyan_gfx.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_lovyan_gfx.h"
#if LV_USE_LOVYAN_GFX

#include LV_LGFX_USER_INCLUDE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    LGFX * tft;
} lv_lovyan_gfx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void resolution_changed_event_cb(lv_event_t * e);
static void read_touch(lv_indev_t * indev_driver, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_lovyan_gfx_create(uint32_t hor_res, uint32_t ver_res, void * buf, uint32_t buf_size_bytes, bool touch)
{
    lv_lovyan_gfx_t * dsc = (lv_lovyan_gfx_t *)lv_malloc_zeroed(sizeof(lv_lovyan_gfx_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }

    dsc->tft = new LGFX();
    dsc->tft->init();     /* TFT init */
    dsc->tft->initDMA();
    dsc->tft->setRotation(0);
    dsc->tft->setBrightness(255);
    dsc->tft->startWrite();
    dsc->tft->fillScreen(0x00000);

    lv_display_set_driver_data(disp, (void *)dsc);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
    lv_display_add_event_cb(disp, resolution_changed_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_set_buffers(disp, (void *)buf, NULL, buf_size_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);

    if(touch) {
        /* Register an input device when touch is enabled */
        lv_indev_t * lv_input = lv_indev_create();
        lv_indev_set_driver_data(lv_input, (void *)dsc);
        lv_indev_set_type(lv_input, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(lv_input, read_touch);
    }

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_lovyan_gfx_t * dsc = (lv_lovyan_gfx_t *)lv_display_get_driver_data(disp);

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);


    if(dsc->tft->getStartCount() == 0) {
        dsc->tft->endWrite();
    }
    dsc->tft->pushImageDMA(area->x1, area->y1, w, h, (uint16_t *)px_map);
    dsc->tft->waitDMA();

    lv_display_flush_ready(disp);

}

static void resolution_changed_event_cb(lv_event_t * e)
{
    lv_display_t * disp = (lv_display_t *)lv_event_get_target(e);
    lv_lovyan_gfx_t * dsc = (lv_lovyan_gfx_t *)lv_display_get_driver_data(disp);
    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);
    lv_display_rotation_t rot = lv_display_get_rotation(disp);

    /* handle rotation */
    switch(rot) {
        case LV_DISPLAY_ROTATION_0:
            dsc->tft->setRotation(0);   /* Portrait orientation */
            break;
        case LV_DISPLAY_ROTATION_90:
            dsc->tft->setRotation(1);   /* Landscape orientation */
            break;
        case LV_DISPLAY_ROTATION_180:
            dsc->tft->setRotation(2);   /* Portrait orientation, flipped */
            break;
        case LV_DISPLAY_ROTATION_270:
            dsc->tft->setRotation(3);   /* Landscape orientation, flipped */
            break;
    }
}

static void read_touch(lv_indev_t * indev_driver, lv_indev_data_t * data)
{
    lv_lovyan_gfx_t * dsc = (lv_lovyan_gfx_t *)lv_indev_get_driver_data(indev_driver);
    uint16_t x;
    uint16_t y;
    bool touched = dsc->tft->getTouch(&x, &y);
    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    else {
        data->state = LV_INDEV_STATE_PRESSED;
        /*Set the coordinates*/
        data->point.x = x;
        data->point.y = y;
    }
}

#endif /*LV_USE_LOVYAN_GFX*/
