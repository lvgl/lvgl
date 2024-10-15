/**
 * @file lv_nxp_elcdif.c
 *
 * Driver for NXP's ELCD
 */

#include "lv_nxp_elcdif.h"

#if LV_USE_NXP_ELCDIF == 1

/*TODO [Q4][kissa96] : Add required headers as discussed with NXP engineers*/

static lv_color_format_t lv_nxp_elcdif_to_lvgl_color_converter(elcdif_rgb_mode_config_t * config)
{
    /*Handle color format conversion*/
    lv_color_format_t color_format;

    switch(config->pixelFormat) {
        case kELCDIF_PixelFormatRAW8 :
            color_format = LV_COLOR_FORMAT_RAW;
            break;
        case kELCDIF_PixelFormatRGB565 :
            color_format = LV_COLOR_FORMAT_RGB565;
            break;
        case kELCDIF_PixelFormatXRGB8888 :
            color_format = LV_COLOR_FORMAT_XRGB8888;
            break;
        case kELCDIF_PixelFormatRGB888 :
            color_format = LV_COLOR_FORMAT_RGB888;
            break;
        /*
        There are some color formats in ELCDIF which LVGL does not support.
        For these, use unknown format and drop a msg for the user
        */
        default :
            color_format = LV_COLOR_FORMAT_UNKNOWN;
            LV_LOG("Not supported color format in ELCDIF. Using LV_UNKNOWN!")
    }
    return color_format;
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    /*TODO [Q4][kissa96] : Implement task after discussing it with NXP engineers*/
    lv_disp_flush_ready(disp);
}

static void wait_cb(lv_display_t * disp)
{
    /*TODO [Q4][kissa96] : Implement task after discussing it with NXP engineers*/
}

lv_display_t * lv_nxp_elcdif_create_from_config(elcdif_rgb_mode_config_t * config, void * frame_buffer1,
                                                void * frame_buffer2, uint32_t buf_size, const lv_display_render_mode_t mode)
{
    static lv_display_t * disp;

    //Create the display, or return NULL if invalid mode
    switch(mode) {
        case LV_DISPLAY_RENDER_MODE_DIRECT:
        case LV_DISPLAY_RENDER_MODE_FULL:
        case LV_DISPLAY_RENDER_MODE_PARTIAL:
            disp = lv_display_create(config->panelWidth, config->panelHeight);
        default :
            return NULL;
    }

    /*Set color format and buffers*/
    lv_display_set_color_format(disp, lv_nxp_elcdif_to_lvgl_color_converter(config));
    lv_display_set_buffers(disp, frame_buffer1, frame_buffer2, buf_size, mode);

    /*Set callbacks*/
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_flush_wait_cb(disp, wait_cb);

    return disp;
}

#endif /*LV_USE_NXP_ELCDIF*/
