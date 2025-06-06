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
#include "../../../display/lv_display_private.h"
#include "fsl_video_common.h"
#include "fsl_elcdif.h"
#include "fsl_cache.h"

/*********************
 *      DEFINES
 *********************/
#if (defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && (0 != FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET))
    #define ELCDIF_ADDR_IP_2_CPU(addr) (MEMORY_ConvertMemoryMapAddress((uint32_t)(addr), kMEMORY_DMA2Local))
#else
    #define ELCDIF_ADDR_IP_2_CPU(addr) (addr)
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static void flush_partial_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static lv_color_format_t lv_nxp_elcdif_to_lvgl_color_converter(elcdif_rgb_mode_config_t * config);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_nxp_display_elcdif_create_direct(LCDIF_Type * base, const elcdif_rgb_mode_config_t * config,
                                                   void * frame_buffer1,
                                                   void * frame_buffer2, size_t buf_size)
{
    LV_ASSERT(base);
    LV_ASSERT(config);

    lv_display_t * disp = lv_display_create(config->panelWidth, config->panelHeight);
    LV_ASSERT(disp);

    lv_color_format_t color_format = lv_nxp_elcdif_to_lvgl_color_converter((elcdif_rgb_mode_config_t *)config);
    lv_display_set_color_format(disp, color_format);
    lv_display_set_buffers(disp, frame_buffer1, frame_buffer2, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_user_data(disp, base);

    ELCDIF_EnableInterrupts(base, kELCDIF_CurFrameDoneInterruptEnable);
    NVIC_EnableIRQ(eLCDIF_IRQn);

    return disp;
}

lv_display_t * lv_nxp_display_elcdif_create_partial(LCDIF_Type * base, const elcdif_rgb_mode_config_t * config,
                                                    void * frame_buffer1,
                                                    void * frame_buffer2, size_t buf_size)
{
    LV_ASSERT(base);
    LV_ASSERT(config);

    /* Create a direct mode display and then update the buffers to be set in partial mode */
    lv_display_t * disp = lv_nxp_display_elcdif_create_direct(base, config, frame_buffer1, frame_buffer2, buf_size);
    ELCDIF_DisableInterrupts(base, kELCDIF_CurFrameDoneInterruptEnable);
    NVIC_DisableIRQ(eLCDIF_IRQn);

    lv_display_set_flush_cb(disp, flush_partial_cb);
    lv_display_set_buffers(disp, frame_buffer1, frame_buffer2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    ELCDIF_EnableInterrupts(base, kELCDIF_CurFrameDoneInterruptEnable);
    NVIC_EnableIRQ(eLCDIF_IRQn);

    return disp;
}

void lv_nxp_display_elcdif_event_handler(const lv_display_t * disp)
{
    if(disp == NULL) {
        /* Just return since no valid display has been set yet */
        return;
    }

    LCDIF_Type * base = (LCDIF_Type *)lv_display_get_user_data((lv_display_t *)disp);
    uint32_t intStatus = ELCDIF_GetInterruptStatus(base);

    ELCDIF_ClearInterruptStatus(base, intStatus);

    if(intStatus & kELCDIF_CurFrameDone) {
        /* flush ready is ISR safe and atomic, so calling inside of the
         * framebuffer interrupt is safe and makes the flush chain
         * non blocking even in bare metal systems.
         */
        lv_disp_flush_ready((lv_display_t *)disp);
    }

    SDK_ISR_EXIT_BARRIER;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LCDIF_Type * base = (LCDIF_Type *)lv_display_get_user_data(disp);

    DCACHE_CleanInvalidateByRange((uint32_t)color_p, lv_display_get_draw_buf_size(disp));

    if(!lv_display_flush_is_last(disp)) {
        lv_disp_flush_ready(disp);
        return;
    }
    ELCDIF_SetNextBufferAddr(base, (uint32_t)color_p);
}

static void flush_partial_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LCDIF_Type * base = (LCDIF_Type *)lv_display_get_user_data(disp);

    DCACHE_CleanInvalidateByRange((uint32_t)color_p, lv_display_get_draw_buf_size(disp));

    uint8_t * fb = (uint8_t *)ELCDIF_ADDR_IP_2_CPU(base->CUR_BUF);
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    int32_t disp_w = lv_display_get_horizontal_resolution(disp);
    int32_t disp_h = lv_display_get_vertical_resolution(disp);
    int32_t bytes_per_pixel = LV_COLOR_FORMAT_GET_SIZE(lv_display_get_color_format(disp));
    int32_t i;

    fb = fb + area->y1 * disp_h;
    fb = fb + area->x1;

    for(i = 0; i < h; i++) {
        lv_memcpy(fb, color_p, w * bytes_per_pixel);
        fb += disp_h;
        color_p += w;
    }
}

static lv_color_format_t lv_nxp_elcdif_to_lvgl_color_converter(elcdif_rgb_mode_config_t * config)
{
    /*Handle color format conversion*/
    lv_color_format_t color_format;

    switch(config->pixelFormat) {
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
