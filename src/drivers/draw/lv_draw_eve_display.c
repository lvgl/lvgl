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
#include "../../draw/eve/lv_draw_eve_target.h"
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

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_draw_eve_display_create(void)
{
    EVE_init();
    EVE_memWrite8(REG_PWM_DUTY, EVE_BACKLIGHT_PWM); /* 0 = off, 0x80 = max */

    EVE_start_cmd_burst();
    EVE_cmd_dl_burst(CMD_DLSTART); /* start the display list */
    EVE_cmd_dl_burst(DL_CLEAR_COLOR_RGB | 0x000000);
    EVE_cmd_dl_burst(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
    EVE_cmd_dl_burst(VERTEX_FORMAT(0));

    /* The buffer is not used, so just set something. */
    static lv_draw_buf_t draw_buf;
    static uint8_t dummy_buf; /* It won't be used as it will send commands instead of draw pixels. */
    lv_draw_buf_init(&draw_buf, EVE_HSIZE, EVE_VSIZE, LV_COLOR_FORMAT_NATIVE,
                     EVE_HSIZE * LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_NATIVE),
                     &dummy_buf, EVE_HSIZE * EVE_VSIZE * LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_NATIVE));

    lv_display_t * disp = lv_display_create(EVE_HSIZE, EVE_VSIZE);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_draw_buffers(disp, &draw_buf, NULL);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_FULL); /* recreate the full display list each refresh */

    return disp;
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

#endif /*LV_USE_DRAW_EVE*/
