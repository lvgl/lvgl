/**
 * @file lv_renesas_glcdc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_renesas_glcdc.h"

#if LV_USE_RENESAS_GLCDC

#include "LVGL_thread.h"
#include <stdbool.h>
#include "../../../display/lv_display_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void glcdc_init(void);
static void flush_direct(lv_display_t * display, const lv_area_t * area, uint8_t * px_map);
static void flush_partial(lv_display_t * display, const lv_area_t * area, uint8_t * px_map);
static void flush_wait_direct(lv_display_t * display);
static void flush_wait_partial(lv_display_t * display);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_renesas_glcdc_direct_create(void)
{
    glcdc_init();

    lv_display_t * display = lv_display_create(DISPLAY_HSIZE_INPUT0, DISPLAY_VSIZE_INPUT0);
    lv_display_set_flush_cb(display, flush_direct);
    lv_display_set_flush_wait_cb(display, flush_wait_direct);
    lv_display_set_buffers(display, &fb_background[0][0], &fb_background[1][0], sizeof(fb_background[0]),
                           LV_DISPLAY_RENDER_MODE_DIRECT);

    return display;
}

lv_display_t * lv_renesas_glcdc_partial_create(void * buf1, void * buf2, size_t buf_size)
{
    glcdc_init();

    lv_display_t * display = lv_display_create(DISPLAY_HSIZE_INPUT0, DISPLAY_VSIZE_INPUT0);
    lv_display_set_flush_cb(display, flush_partial);
    lv_display_set_flush_wait_cb(display, flush_wait_partial);
    lv_display_set_buffers(display, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    return display;
}

/*This function is declared in and being used by FSP generated code modules*/
void glcdc_callback(display_callback_args_t * p_args)
{
    if(DISPLAY_EVENT_LINE_DETECTION == p_args->event) {
#if BSP_CFG_RTOS == 2               /*FreeRTOS*/
        BaseType_t context_switch;

        /*Set Vsync semaphore*/
        xSemaphoreGiveFromISR(_SemaphoreVsync, &context_switch);

        /*Return to the highest priority available task*/
        portYIELD_FROM_ISR(context_switch);
#else
#endif
    }
    else if(DISPLAY_EVENT_GR1_UNDERFLOW == p_args->event) {
        __BKPT(0); /*Layer 1 Underrun*/
    }
    else if(DISPLAY_EVENT_GR2_UNDERFLOW == p_args->event) {
        __BKPT(0); /*Layer 2 Underrun*/
    }
    else { /*DISPLAY_EVENT_FRAME_END*/
        __BKPT(0);
    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void glcdc_init(void)
{
    /* Fill the Frame buffer with black colour (0x0000 in RGB565), for a clean start after previous runs */
    lv_memzero(fb_background, sizeof(fb_background));

    /* Initalize GLCDC driver */
    uint8_t * p_fb = &fb_background[1][0];
    fsp_err_t err;

    err = R_GLCDC_Open(&g_display0_ctrl, &g_display0_cfg);
    if(FSP_SUCCESS != err) {
        __BKPT(0);
    }

    err = R_GLCDC_Start(&g_display0_ctrl);
    if(FSP_SUCCESS != err) {
        __BKPT(0);
    }

    do {
        err =
            R_GLCDC_BufferChange(&g_display0_ctrl,
                                 (uint8_t *) p_fb,
                                 (display_frame_layer_t) 0);
    } while(FSP_ERR_INVALID_UPDATE_TIMING == err);
}

static void flush_direct(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    FSP_PARAMETER_NOT_USED(area);
    /*Display the frame buffer pointed by px_map*/

    if(!lv_display_flush_is_last(display)) return;

#if defined(RENESAS_CORTEX_M85) && (BSP_CFG_DCACHE_ENABLED)
    /* Invalidate cache - so the HW can access any data written by the CPU */
    SCB_CleanInvalidateDCache_by_Addr(px_map, sizeof(fb_background[0]));
#endif

    R_GLCDC_BufferChange(&g_display0_ctrl,
                         (uint8_t *) px_map,
                         (display_frame_layer_t) 0);
}

static void flush_wait_direct(lv_display_t * display)
{
    if(!lv_display_flush_is_last(display)) return;

#if BSP_CFG_RTOS == 2              /*FreeRTOS*/
    /*If Vsync semaphore has already been set, clear it then wait to avoid tearing*/
    if(uxSemaphoreGetCount(_SemaphoreVsync)) {
        xSemaphoreTake(_SemaphoreVsync, 10);
    }

    xSemaphoreTake(_SemaphoreVsync, portMAX_DELAY);
#endif

}

static void flush_partial(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(display);

    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    uint16_t * fb = (uint16_t *)fb_background[1];
    uint16_t * img = (uint16_t *)px_map;

    fb = fb + area->y1 * DISPLAY_HSIZE_INPUT0;
    fb = fb + area->x1;

    int32_t i;
    for(i = 0; i < h; i++) {
        lv_memcpy(fb, img, w * 2);
#if defined(RENESAS_CORTEX_M85) && (BSP_CFG_DCACHE_ENABLED)
        SCB_CleanInvalidateDCache_by_Addr(fb, w * 2);
#endif
        fb += DISPLAY_HSIZE_INPUT0;
        img += w;
    }
}

static void flush_wait_partial(lv_display_t * display)
{
    LV_UNUSED(display);

    return;
}

#endif /*LV_USE_RENESAS_GLCDC*/
