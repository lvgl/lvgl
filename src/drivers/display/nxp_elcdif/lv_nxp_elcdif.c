/**
 * @file lv_nxp_elcdif.c
 *
 * Driver for NXP's ELCD
 */

#include "lv_nxp_elcdif.h"

#if LV_USE_NXP_ELCDIF == 1
/*********************
 *      INCLUDES
 *********************/
#include "fsl_video_common.h"
#include "fsl_elcdif.h"
#include "fsl_lpi2c.h"
#include "fsl_gpio.h"
#include "fsl_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static lv_color_format_t lv_nxp_elcdif_to_lvgl_color_converter(elcdif_rgb_mode_config_t config);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool s_framePending;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_nxp_display_elcdif_create_direct(elcdif_rgb_mode_config_t config, void * frame_buffer1,
                                                   void * frame_buffer2, size_t buf_size)
{
    /*A handle for the display*/
    lv_display_t * disp = lv_display_create(config.panelWidth, config.panelHeight);

    /*Set color format and buffers*/
    lv_color_format_t color_format = lv_nxp_elcdif_to_lvgl_color_converter(config);
    lv_display_set_color_format(disp, color_format);
    lv_display_set_buffers(disp, frame_buffer1, frame_buffer2, buf_size, mode);
    lv_display_set_flush_cb(disp, flush_cb);

    ELCDIF_EnableInterrupts(LCDIF, kELCDIF_CurFrameDoneInterruptEnable);
    NVIC_EnableIRQ(LCDIF_IRQn);

    return disp;
}

void LCDIF_IRQHandler(void)
{
    uint32_t intStatus = ELCDIF_GetInterruptStatus(LCDIF);

    ELCDIF_ClearInterruptStatus(LCDIF, intStatus);

    if(s_framePending) {
        if(intStatus & kELCDIF_CurFrameDone) {
            s_framePending = false;
        }
    }
    SDK_ISR_EXIT_BARRIER;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
{
    DCACHE_CleanInvalidateByRange((uint32_t)color_p, DEMO_FB_SIZE);

    if(!lv_display_flush_is_last(disp)) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    ELCDIF_SetNextBufferAddr(LCDIF, (uint32_t)color_p);

    s_framePending = true;

    while(s_framePending);

    lv_disp_flush_ready(disp_drv);
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

#endif /*LV_USE_NXP_ELCDIF*/
