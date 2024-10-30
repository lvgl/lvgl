/**
 * @file lv_nxp_elcdif.c
 *
 * Driver for NXP's ELCD
 */

#include "lv_nxp_elcdif.h"

#if LV_USE_NXP_ELCDIF == 1

lv_indev_t * lv_nxp_indev_create(void (indev_init_cb)(void), lv_indev_type_t indev_type, lv_indev_read_cb_t read_cb)
{
    lv_indev_t * indev;

    indev = lv_indev_create();
    if(!indev) {
        LV_LOG_ERROR("Malloc failed: lv_nxp_indev_create")
        return NULL;
    }
    lv_indev_set_type(indev, indev_type);

    /*Set callbacks*/
    if(read_cb)
        lv_indev_set_read_cb(indev, read_cb);

    if(indev_init_cb)
        indev_init_cb();

    return indev;
}

lv_indev_t * lv_nxp_touchpad_create(void (touchpad_init_cb)(void), lv_indev_read_cb_t touchpad_read_cb)
{
    /*Handle to touchpad*/
    lv_indev_t * touchpad;

    touchpad = lv_nxp_indev_create(touchpad_init_cb, LV_INDEV_TYPE_POINTER, touchpad_read_cb);
    return touchpad;
}

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

lv_nxp_lcd_object_t * lv_nxp_lcd_create(elcdif_rgb_mode_config_t config, void * frame_buffer1,
                                        void * frame_buffer2, size_t buf_size, lv_display_render_mode_t mode,
                                        void (init_cb)(void), lv_display_flush_cb_t flush_cb, lv_display_flush_wait_cb_t wait_cb,
                                        void (indev_init_cb)(void), lv_indev_type_t indev_type, lv_indev_read_cb_t read_cb)
{
    lv_nxp_lcd_object_t * lcd_display;

    //Create the display
    lcd_display->display = lv_nxp_display_elcdif_create(config, frame_buffer1, frame_buffer2, buf_size, mode, init_cb,
                                                        flush_cb, wait_cb);
    //Create the indev
    lcd_display->indev = lv_nxp_indev_create(indev_init_cb, indev_type, read_cb);

    return lcd_display;
}

lv_display_t * lv_nxp_display_obj_create(lv_nxp_display_obj_params_t display_params)
{
    lv_display_t * display;

    display = lv_nxp_display_elcdif_create(display_params.config, display_params.frame_buffer1,
                                           display_params.frame_buffer2,
                                           display_params.buf_size, display_params.mode, display_params.init_cb,
                                           display_params.flush_cb, display_params.wait_cb);
    return display;
}

lv_indev_t * lv_nxp_indev_obj_create(lv_nxp_indev_obj_params_t indev_params)
{
    lv_indev_t * indev;

    indev = lv_nxp_indev_create(indev_params.indev_init_cb, indev_params.indev_type, indev_params.read_cb);
    return indev;
}

lv_nxp_lcd_object_t * lv_nxp_lcd_obj_create(lv_nxp_lcd_obj_params_t lcd_params)
{
    lv_nxp_lcd_object_t * lcd_obj;

    lcd_obj->display = lv_nxp_display_obj_create(lcd_params.display_params);
    lcd_obj->indev = lv_nxp_indev_obj_create(lcd_params.indev_params);

    return lcd_obj;
}

#endif /*LV_USE_NXP_ELCDIF*/
