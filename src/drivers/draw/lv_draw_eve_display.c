/**
 * @file lv_draw_eve_display.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_eve_display.h"
#if LV_USE_DRAW_EVE

#include "../../draw/eve/lv_eve.h"
#include "../../draw/eve/lv_draw_eve.h"
#include "../../display/lv_display_private.h"
#include "../../draw/lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_draw_eve_display_create(const lv_draw_eve_parameters_t * params, lv_draw_eve_operation_cb_t op_cb,
                                          void * user_data)
{
    /* The buffer is not used, so just set something. */
    static lv_draw_buf_t draw_buf;
    static uint8_t dummy_buf; /* It won't be used as it will send commands instead of draw pixels. */
    lv_draw_buf_init(&draw_buf, params->hor_res, params->ver_res, LV_COLOR_FORMAT_NATIVE,
                     params->hor_res * LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_NATIVE),
                     &dummy_buf, params->hor_res * params->ver_res * LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_NATIVE));

    lv_display_t * disp = lv_display_create(params->hor_res, params->ver_res);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_draw_buffers(disp, &draw_buf, NULL);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_FULL); /* recreate the full display list each refresh */
    lv_display_set_driver_data(disp, user_data);

    lv_draw_eve_set_display_data(disp, params, op_cb);

    EVE_init();
    EVE_memWrite8(REG_PWM_DUTY, EVE_BACKLIGHT_PWM); /* 0 = off, 0x80 = max */

    EVE_start_cmd_burst();
    EVE_cmd_dl_burst(CMD_DLSTART); /* start the display list */
    EVE_cmd_dl_burst(DL_CLEAR_COLOR_RGB | 0x000000);
    EVE_cmd_dl_burst(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
    EVE_cmd_dl_burst(VERTEX_FORMAT(0));

    return disp;
}

void * lv_draw_eve_display_get_user_data(lv_display_t * disp)
{
    return lv_display_get_driver_data(disp);
}

lv_indev_t * lv_draw_eve_touch_create(void)
{
    lv_indev_t * indev = lv_indev_create();

    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read_cb);

    return indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    if(lv_display_flush_is_last(disp)) {
        EVE_cmd_dl_burst(DL_DISPLAY); /* instruct the co-processor to show the list */
        EVE_cmd_dl_burst(CMD_SWAP);   /* make this list active */
        EVE_end_cmd_burst();
        EVE_execute_cmd();

        EVE_start_cmd_burst();
        EVE_cmd_dl_burst(CMD_DLSTART);
        EVE_cmd_dl_burst(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
        EVE_cmd_dl_burst(VERTEX_FORMAT(0));
    }

    lv_display_flush_ready(disp);
}

static void touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_display_t * disp = lv_indev_get_display(indev);

    if(disp == NULL || disp->flush_cb != flush_cb) return;

    EVE_end_cmd_burst();

    uint32_t xy = EVE_memRead32(REG_TOUCH_SCREEN_XY);
    uint16_t x = xy >> 16;
    uint16_t y = xy & 0xffff;

    int32_t disp_w = lv_display_get_original_horizontal_resolution(disp);
    int32_t disp_h = lv_display_get_original_vertical_resolution(disp);

    LV_LOG_USER("%u %u", (unsigned) x, (unsigned) y);

    if (x < disp_w && y < disp_h) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = x;
        data->point.y = y;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    EVE_start_cmd_burst();
}

#endif /*LV_USE_DRAW_EVE*/
