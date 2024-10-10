/**
 * @file lv_nxp_elcdif.c
 *
 * Driver for NXP's ELCD
 */

#include "lv_nxp_elcdif.h"

#ifdef LV_USE_NXP_ELCDIF
#if LV_USE_NXP_ELCDIF == 1

lv_color_format_t lv_nxp_elcdif_to_lvgl_color_converter(elcdif_rgb_mode_config_t * config)
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
        /*There are some color formats in ELCDIF which LVGL does not support.
          For these, use unknown format and drop a msg for the user*/
        default :
            color_format = LV_COLOR_FORMAT_UNKNOWN;
            LV_LOG("Not supported color format in ELCDIF. Using LV_UNKNOWN!")
    }
    return color_format;
}

lv_display_t * lv_nxp_elcdif_create_from_config(elcdif_rgb_mode_config_t * config, void * frame_buffer1,
                                                void * frame_buffer2, uint32_t buf_size, const lv_display_render_mode_t mode, lv_display_flush_cb_t flush_cb,
                                                lv_display_set_flush_wait_cb_t wait_cb)
{
    static lv_display_t * disp;

    switch(mode) {
        case LV_DISPLAY_RENDER_MODE_PARTIAL :
            disp = lv_nxp_elcdif_create_partial(config, frame_buffer1, frame_buffer2, buf_size, flush_cb, wait_cb);
            break;
        case LV_DISPLAY_RENDER_MODE_DIRECT :
            disp = lv_nxp_elcdif_create_direct(config, frame_buffer1, frame_buffer2, buf_size, flush_cb, wait_cb);
            break;
        case LV_DISPLAY_RENDER_MODE_FULL :
            disp = lv_nxp_elcdif_create_full(config, frame_buffer1, frame_buffer2, buf_size, flush_cb, wait_cb);
        default :
            return NULL;
    }
    /*Check for nullptr first*/
    if(disp == NULL)
        return NULL;
    /*Set callbacks*/
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_flush_wait_cb(disp, wait_cb);

    return disp;
}

lv_display_t * lv_nxp_elcdif_create_partial(elcdif_rgb_mode_config_t * config, void * frame_buffer1,
                                            void * frame_buffer2, uint32_t buf_size, lv_display_flush_cb_t flush_cb, lv_display_set_flush_wait_cb_t wait_cb)
{
    static lv_display_t * disp;

    disp = lv_display_create(config->panelWidth, config->panelHeight);
    /*If for some reason disp is NULL, return immediately*/
    if(disp == NULL)
        return NULL;
    /*We need to set the color format prior to setting the buffers*/
    lv_display_set_color_format(disp, lv_nxp_elcdif_to_lvgl_color_converter(config));
    lv_display_set_buffers(disp, frame_buffer1, frame_buffer2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    return disp;
}

lv_display_t * lv_nxp_elcdif_create_direct(elcdif_rgb_mode_config_t * config, void * frame_buffer1,
                                           void * frame_buffer2, uint32_t buf_size, lv_display_flush_cb_t flush_cb, lv_display_set_flush_wait_cb_t wait_cb)
{
    static lv_display_t * disp;

    disp = lv_display_create(config->panelWidth, config->panelHeight);
    /*If for some reason disp is NULL, return immediately*/
    if(disp == NULL)
        return NULL;
    /*We need to set the color format prior to setting the buffers*/
    lv_display_set_color_format(disp, lv_nxp_elcdif_to_lvgl_color_converter(config));
    lv_display_set_buffers(disp, frame_buffer1, frame_buffer2, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);

    return disp;
}

lv_display_t * lv_nxp_elcdif_create_full(elcdif_rgb_mode_config_t * config, void * frame_buffer1, void * frame_buffer2,
                                         uint32_t buf_size, lv_display_flush_cb_t flush_cb, lv_display_set_flush_wait_cb_t wait_cb)
{
    static lv_display_t * disp;

    disp = lv_display_create(config->panelWidth, config->panelHeight);
    /*If for some reason disp is NULL, return immediately*/
    if(disp == NULL)
        return NULL;
    /*We need to set the color format prior to setting the buffers*/
    lv_display_set_color_format(disp, lv_nxp_elcdif_to_lvgl_color_converter(config));
    lv_display_set_buffers(disp, frame_buffer1, frame_buffer2, buf_size, LV_DISPLAY_RENDER_MODE_FULL);

    return disp;
}
#endif /*LV_USE_NXP_ELCDIF*/
#endif