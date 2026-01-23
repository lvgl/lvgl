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

static void read_touch(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_lovyan_gfx_t * dsc = (lv_lovyan_gfx_t *)lv_indev_get_driver_data(indev);
    uint16_t x, y;
    bool touched = dsc->tft->getTouch(&x, &y);

    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    else {
        data->state = LV_INDEV_STATE_PRESSED;

        /* LovyanGFX returns coordinates relative to the current software rotation.
         * LVGL will also rotate the input coordinates based on the display rotation.
         * To avoid "double rotation", we must reverse the LovyanGFX rotation
         * and pass raw (Physical/Rotation 0) coordinates to LVGL.
         */
        uint8_t rotation = dsc->tft->getRotation();

        /* Note: width() and height() return dimensions for the *current* rotation */
        int32_t w = dsc->tft->width();
        int32_t h = dsc->tft->height();

        switch(rotation) {
            case 1: /* Landscape (90 deg CW) */
                /* Rot 0 (Phys) Top-Left becomes Rot 1 Top-Right.
                 * Phys X = Rot1 Y
                 * Phys Y = Rot1 Width - 1 - Rot1 X */
                data->point.x = y;
                data->point.y = w - 1 - x;
                break;

            case 2: /* Portrait Inverted (180 deg) */
                /* Phys X = Rot2 Width - 1 - Rot2 X
                 * Phys Y = Rot2 Height - 1 - Rot2 Y */
                data->point.x = w - 1 - x;
                data->point.y = h - 1 - y;
                break;

            case 3: /* Landscape Inverted (270 deg CW) */
                /* Phys X = Rot3 Height - 1 - Rot3 Y
                 * Phys Y = Rot3 X */
                data->point.x = h - 1 - y;
                data->point.y = x;
                break;

            default: /* Portrait (0 deg) */
                /* Pass through raw coordinates */
                data->point.x = x;
                data->point.y = y;
                break;
        }
    }
}

#endif /*LV_USE_LOVYAN_GFX*/
