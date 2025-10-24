/**
 * @file lv_draw_eve_display.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_eve_display.h"
#if LV_USE_DRAW_EVE

#include "../../../draw/eve/lv_eve.h"
#include "../../../draw/eve/lv_draw_eve.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_text_private.h"

#include "../../../libs/FT800-FT813/EVE_commands.h"


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
static void resolution_changed_cb(lv_event_t * e);
static void render_start_cb(lv_event_t * e);
static void render_ready_cb(lv_event_t * e);
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
    static uint32_t dummy_buf; /* It won't be used as it will send commands instead of draw pixels. */

    lv_display_t * disp = lv_display_create(params->hor_res, params->ver_res);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, &dummy_buf, NULL,
                           params->hor_res * params->ver_res * LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_NATIVE),
                           LV_DISPLAY_RENDER_MODE_FULL); /* recreate the full display list each refresh */
    lv_display_add_event_cb(disp, resolution_changed_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(disp, render_start_cb, LV_EVENT_RENDER_START, NULL);
    lv_display_add_event_cb(disp, render_ready_cb, LV_EVENT_RENDER_READY, NULL);
    lv_display_set_driver_data(disp, user_data);

    lv_draw_eve_set_display_data(disp, params, op_cb);

    EVE_init();
    EVE_memWrite8(REG_PWM_DUTY, EVE_BACKLIGHT_PWM); /* 0 = off, 0x80 = max */

    return disp;
}

void * lv_draw_eve_display_get_user_data(lv_display_t * disp)
{
    return lv_display_get_driver_data(disp);
}

lv_indev_t * lv_draw_eve_touch_create(lv_display_t * disp)
{
    lv_indev_t * indev = lv_indev_create();

    lv_indev_set_display(indev, disp);
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read_cb);

    return indev;
}

void lv_draw_eve_pre_upload_image(lv_display_t * disp, const void * src)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to do an LVGL EVE pre-upload without a draw_eve display");

    if(!lv_draw_eve_image_src_check(src)) {
        return;
    }

    uint32_t ramg_addr = lv_draw_eve_image_upload_image(false, src);
    if(ramg_addr == LV_DRAW_EVE_RAMG_OUT_OF_RAMG) {
        LV_LOG_WARN("Could not pre-upload image because space could not be allocated in RAM_G.");
    }
}

void lv_draw_eve_pre_upload_font_range(lv_display_t * disp, const lv_font_t * font, uint32_t unicode_range_start,
                                       uint32_t unicode_range_end)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to do an LVGL EVE pre-upload without a draw_eve display");

    if(!lv_draw_eve_label_font_check(font)) {
        return;
    }

    for(uint32_t i = unicode_range_start; i <= unicode_range_end; i++) {
        lv_font_glyph_dsc_t glyph_dsc;
        bool found = lv_font_get_glyph_dsc_fmt_txt(font, &glyph_dsc, i, '\0');
        if(!found) {
            LV_LOG_INFO("Could not pre-upload glyph with unicode code point '0x%"LV_PRIX32"' "
                        "because it is not part of the font", i);
            continue;
        }
        uint32_t ramg_addr = lv_draw_eve_label_upload_glyph(false, font->dsc, glyph_dsc.gid.index);
        if(ramg_addr == LV_DRAW_EVE_RAMG_OUT_OF_RAMG) {
            LV_LOG_WARN("Could not pre-upload glyph because space could not be allocated in RAM_G.");
            /* don't return in case there are smaller glyphs that there is space for */
        }
    }
}

void lv_draw_eve_pre_upload_font_text(lv_display_t * disp, const lv_font_t * font, const char * text)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to do an LVGL EVE pre-upload without a draw_eve display");

    if(!lv_draw_eve_label_font_check(font)) {
        return;
    }

    for(uint32_t i = 0; text[i];) {
        uint32_t unicode_letter;
        uint32_t unicode_letter_next;
        lv_text_encoded_letter_next_2(text, &unicode_letter, &unicode_letter_next, &i);
        lv_font_glyph_dsc_t glyph_dsc;
        bool found = lv_font_get_glyph_dsc_fmt_txt(font, &glyph_dsc, unicode_letter, unicode_letter_next);
        if(!found) {
            LV_LOG_INFO("Could not pre-upload glyph with unicode code point '0x%"LV_PRIX32"' "
                        "because it is not part of the font", unicode_letter);
            continue;
        }
        uint32_t ramg_addr = lv_draw_eve_label_upload_glyph(false, font->dsc, glyph_dsc.gid.index);
        if(ramg_addr == LV_DRAW_EVE_RAMG_OUT_OF_RAMG) {
            LV_LOG_WARN("Could not pre-upload glyph because space could not be allocated in RAM_G.");
            /* don't return in case there are smaller glyphs that there is space for */
        }
    }
}

uint8_t lv_draw_eve_memread8(lv_display_t * disp, uint32_t address)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to use an LVGL EVE command without a draw_eve display");
    return EVE_memRead8(address);
}

uint16_t lv_draw_eve_memread16(lv_display_t * disp, uint32_t address)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to use an LVGL EVE command with a non-draw_eve display");
    return EVE_memRead16(address);
}

uint32_t lv_draw_eve_memread32(lv_display_t * disp, uint32_t address)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to use an LVGL EVE command with a non-draw_eve display");
    return EVE_memRead32(address);
}

void lv_draw_eve_memwrite8(lv_display_t * disp, uint32_t address, uint8_t data)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to use an LVGL EVE command with a non-draw_eve display");
    EVE_memWrite8(address, data);
}

void lv_draw_eve_memwrite16(lv_display_t * disp, uint32_t address, uint16_t data)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to use an LVGL EVE command with a non-draw_eve display");
    EVE_memWrite16(address, data);
}

void lv_draw_eve_memwrite32(lv_display_t * disp, uint32_t address, uint32_t data)
{
    LV_ASSERT_MSG(disp->flush_cb == flush_cb, "tried to use an LVGL EVE command with a non-draw_eve display");
    EVE_memWrite32(address, data);
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
    }

    lv_display_flush_ready(disp);
}

static void resolution_changed_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);

    lv_display_rotation_t rotation = lv_display_get_rotation(disp);
    uint32_t cmd_value;
    switch(rotation) {
        case LV_DISPLAY_ROTATION_0:
            cmd_value = 0;
            break;
        case LV_DISPLAY_ROTATION_90:
            cmd_value = 2;
            break;
        case LV_DISPLAY_ROTATION_180:
            cmd_value = 1;
            break;
        case LV_DISPLAY_ROTATION_270:
            cmd_value = 3;
            break;
        default:
            return;
    }

    /* no need to rotate the touch coordinates with CMD_SETROTATE, as LVGL
     * already rotates the input coordinates.
     */
    EVE_memWrite8(REG_ROTATE, cmd_value);
}

static void render_start_cb(lv_event_t * e)
{
    EVE_start_cmd_burst();

    EVE_cmd_dl_burst(CMD_DLSTART); /* start the display list */
    EVE_cmd_dl_burst(DL_CLEAR_COLOR_RGB | 0x000000);
    EVE_cmd_dl_burst(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
    EVE_cmd_dl_burst(VERTEX_FORMAT(0));
}

static void render_ready_cb(lv_event_t * e)
{
    EVE_end_cmd_burst();
}

static void touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_display_t * disp = lv_indev_get_display(indev);

    if(disp == NULL || disp->flush_cb != flush_cb) return;

    uint32_t xy = EVE_memRead32(REG_TOUCH_SCREEN_XY);
    uint16_t x = xy >> 16;
    uint16_t y = xy & 0xffff;

    int32_t disp_w = lv_display_get_original_horizontal_resolution(disp);
    int32_t disp_h = lv_display_get_original_vertical_resolution(disp);

    if(x < disp_w && y < disp_h) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = x;
        data->point.y = y;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

#endif /*LV_USE_DRAW_EVE*/
