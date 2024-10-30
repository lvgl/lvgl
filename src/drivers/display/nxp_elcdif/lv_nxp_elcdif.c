/**
 * @file lv_nxp_elcdif.c
 *
 * Driver for NXP's ELCD
 */

#include "lv_nxp_elcdif.h"

#if LV_USE_NXP_ELCDIF == 1

static lv_color_format_t lv_nxp_elcdif_to_lvgl_color_converter(elcdif_rgb_mode_config_t config)
{
    /*Handle color format conversion*/
    lv_color_format_t color_format;

    switch(config.pixelFormat) {
        case kELCDIF_PixelFormatRAW8 :
            color_format = LV_COLOR_FORMAT_L8;
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
            LV_LOG_WARN("Not supported color format in ELCDIF. Using LV_UNKNOWN!");
    }
    return color_format;
}

lv_display_t * lv_nxp_display_elcdif_create(elcdif_rgb_mode_config_t config, void * frame_buffer1,
                                            void * frame_buffer2, size_t buf_size, lv_display_render_mode_t mode,
                                            void (init_cb)(void), lv_display_flush_cb_t flush_cb, lv_display_flush_wait_cb_t wait_cb)
{
    /*A handle for the display*/
    lv_display_t * disp;

    /*Create the display, or return NULL if invalid mode*/
    switch(mode) {
        case LV_DISPLAY_RENDER_MODE_DIRECT:
        case LV_DISPLAY_RENDER_MODE_FULL:
        case LV_DISPLAY_RENDER_MODE_PARTIAL:
            disp = lv_display_create(config.panelWidth, config.panelHeight);
            break;
        default :
            LV_LOG_ERROR("Display mode not supported. NULL returned!")
            return NULL;
    }

    if(!disp) {
        LV_LOG_ERROR("Malloc failed : lv_nxp_display_elcdif_create")
        return NULL;
    }
    /*Set color format and buffers*/
    lv_color_format_t color_format = lv_nxp_elcdif_to_lvgl_color_converter(config);
    lv_display_set_color_format(disp, color_format);
    lv_display_set_buffers(disp, frame_buffer1, frame_buffer2, buf_size, mode);

    /*Set callbacks*/
    if(flush_cb)
        lv_display_set_flush_cb(disp, flush_cb);
    if(wait_cb)
        lv_display_set_flush_wait_cb(disp, wait_cb);

    /*HW dependent initialization callback*/
    if(init_cb)
        init_cb();

    return disp;
}

lv_display_t * lv_nxp_display_elcdif_create_direct(elcdif_rgb_mode_config_t config, void * frame_buffer1,
                                                   void * frame_buffer2, size_t buf_size)
{
    /*A handle for the display*/
    lv_display_t * disp;

    /*Create display, fixed parameters*/
    disp = lv_nxp_display_elcdif_create(config, frame_buffer1, frame_buffer2, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT, NULL,
                                        NULL, NULL);
    return disp;
}

#endif /*LV_USE_NXP_ELCDIF*/
