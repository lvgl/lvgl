/**
 * @file lv_draw_dave2d_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_dave2d.h"

#if LV_USE_DRAW_DAVE2D

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

d2_color  lv_draw_dave2d_lv_colour_to_d2_colour(lv_color_t color)
{
    uint8_t alpha, red, green, blue;

    alpha = 0x00;
    red = color.red ;
    green = color.green ;
    blue = color.blue;
    /*Color depth: 8 (A8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)*/
    switch(LV_COLOR_DEPTH) {
        case(8):
            LV_ASSERT(0);
            break;
        case(16):
            break;
        case(24):
            break;
        case(32):
            break;

        default:
            break;
    }

    return (alpha) << 24UL
           | (red) << 16UL
           | (green) << 8UL
           | (blue) << 0UL;
}

d2_s32 lv_draw_dave2d_cf_fb_get(void)
{
    d2_s32 d2_fb_mode = 0;
    switch(g_display0_cfg.input->format) {
        case    DISPLAY_IN_FORMAT_16BITS_RGB565: ///< RGB565,   16 bits
            d2_fb_mode = d2_mode_rgb565;
            break;
        case DISPLAY_IN_FORMAT_32BITS_ARGB8888: ///< ARGB8888, 32 bits
            d2_fb_mode = d2_mode_argb8888;
            break;
        case DISPLAY_IN_FORMAT_32BITS_RGB888: ///< RGB888,   32 bits
            d2_fb_mode = d2_mode_argb8888; //GLCDC ignores Alpha when configured for RGB888
            break;
        case  DISPLAY_IN_FORMAT_16BITS_ARGB4444: ///< ARGB4444, 16 bits
            d2_fb_mode = d2_mode_argb4444;
            break;
        case DISPLAY_IN_FORMAT_16BITS_ARGB1555: ///< ARGB1555, 16 bits
        case DISPLAY_IN_FORMAT_CLUT8 : ///< CLUT8
        case DISPLAY_IN_FORMAT_CLUT4  : ///< CLUT4
        case  DISPLAY_IN_FORMAT_CLUT1  : ///< CLUT1
            //Not supported as a FB format by Dave2D
            break;

        default:
            break;
    }

    return d2_fb_mode;
}

d2_u32 lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(lv_color_format_t colour_format)
{
    d2_u32 d2_lvgl_mode = 0;

    switch(colour_format) {
        case(LV_COLOR_FORMAT_I1):
            d2_lvgl_mode = d2_mode_i1;
            break;
        case(LV_COLOR_FORMAT_I2):
            d2_lvgl_mode = d2_mode_i2;
            break;
        case(LV_COLOR_FORMAT_I4):
            d2_lvgl_mode = d2_mode_i4;
            break;
        case(LV_COLOR_FORMAT_I8):
            d2_lvgl_mode = d2_mode_i4;
            break;
        case(LV_COLOR_FORMAT_A8):
            d2_lvgl_mode = d2_mode_alpha8;
            break;
        case(LV_COLOR_FORMAT_RGB565):
            d2_lvgl_mode = d2_mode_rgb565;
            break;
        case(LV_COLOR_FORMAT_RGB888):
            d2_lvgl_mode = d2_mode_rgb888;
            break;
        case(LV_COLOR_FORMAT_ARGB1555):
            d2_lvgl_mode = d2_mode_argb1555;
            break;
        case(LV_COLOR_FORMAT_ARGB4444):
            d2_lvgl_mode = d2_mode_argb4444;
            break;
        case(LV_COLOR_FORMAT_ARGB8888):
            d2_lvgl_mode = d2_mode_argb8888;
            break;
        case(LV_COLOR_FORMAT_XRGB8888):
            d2_lvgl_mode = d2_mode_argb8888;
            break;

        default:
            LV_ASSERT(0);
            break;

    }
    return d2_lvgl_mode;
}

void d2_framebuffer_from_layer(d2_device * handle, lv_layer_t * layer)
{
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    lv_area_t buffer_area = layer->buf_area;
    lv_area_move(&buffer_area, -layer->buf_area.x1, -layer->buf_area.y1);

    d2_framebuffer(handle, draw_buf->data,
                   (d2_s32) draw_buf->header.stride / lv_color_format_get_size(layer->color_format),
                   (d2_u32)lv_area_get_width(&buffer_area),
                   (d2_u32)lv_area_get_height(&buffer_area),
                   lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(layer->color_format));
}

bool lv_draw_dave2d_is_dest_cf_supported(lv_color_format_t cf)
{
    bool result;

    switch(cf) {
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB4444:
            result = true;
            break;

        default:
            result = false;
            break;
    }

    return result;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_DAVE2D*/
