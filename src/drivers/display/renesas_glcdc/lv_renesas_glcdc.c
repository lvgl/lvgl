/**
 * @file lv_renesas_glcdc.c
 *
 */

/*********************
 *PLATFORM ABSTRACTION
 *********************/

#ifdef _RENESAS_RA_
    #define USE_FREE_RTOS (BSP_CFG_RTOS == 2)
#else // RX with SMC code generation
    #ifndef _RENESAS_RX_
        #define _RENESAS_RX_ 1
    #endif
    #define USE_FREE_RTOS 1
    #define DISPLAY_HSIZE_INPUT0 LCD_CH0_IN_GR2_HSIZE
    #define DISPLAY_VSIZE_INPUT0 LCD_CH0_IN_GR2_VSIZE
#endif /*_RENESAS_RA_*/

/*********************
 *      INCLUDES
 *********************/
#include "lv_renesas_glcdc.h"

#if LV_USE_RENESAS_GLCDC

#ifdef _RENESAS_RA_
    #include "LVGL_thread.h"
#else /* RX */
    #include "hal_data.h"
    #include "platform.h"
    #include "r_glcdc_rx_if.h"
    #include "r_glcdc_rx_pinset.h"
#endif /*_RENESAS_RA_*/

#include <stdbool.h>
#include "../../../display/lv_display_private.h"
#include "../../../draw/sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/
#define BYTES_PER_PIXEL 2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t * glcdc_create(void * buf1, void * buf2, uint32_t buf_size, lv_display_render_mode_t render_mode);
static void glcdc_init(void);
static void give_vsync_sem_and_yield(void);
static void flush_direct(lv_display_t * display, const lv_area_t * area, uint8_t * px_map);
static void flush_partial(lv_display_t * display, const lv_area_t * area, uint8_t * px_map);
static void flush_wait_direct(lv_display_t * display);
static void flush_wait_partial(lv_display_t * display);

#ifdef _RENESAS_RX_
    static void enable_dave2d_drw_interrupt(void);
#endif /*_RENESAS_RX_*/

/**********************
 *  STATIC VARIABLES
 **********************/

#ifdef _RENESAS_RX_
static uint8_t fb_background[2][LCD_CH0_IN_GR2_HSIZE * LCD_CH0_IN_GR2_VSIZE * BYTES_PER_PIXEL]__attribute__((
                                                                                                                section(".framebuffer"), aligned(64), used));
static SemaphoreHandle_t    _SemaphoreVsync = NULL;
static glcdc_cfg_t          g_config;
static glcdc_runtime_cfg_t  g_layer_change;

/* A global variable that Dave 2D driver relies on. (Being auto generated on RA platforms)*/
display_t g_display0_cfg;
#endif /*_RENESAS_RX_*/

static void * rotation_buffer = NULL;
static uint32_t partial_buffer_size = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_renesas_glcdc_direct_create(void)
{
    return glcdc_create(&fb_background[0][0], &fb_background[1][0], sizeof(fb_background[0]),
                        LV_DISPLAY_RENDER_MODE_DIRECT);
}

lv_display_t * lv_renesas_glcdc_partial_create(void * buf1, void * buf2, size_t buf_size)
{
    partial_buffer_size = buf_size;
    return glcdc_create(buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

/*This function is declared in and being used by FSP generated code modules*/
#ifdef _RENESAS_RA_
void glcdc_callback(display_callback_args_t * p_args)
{
    if(DISPLAY_EVENT_LINE_DETECTION == p_args->event) {
        give_vsync_sem_and_yield();
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
#else /* RX */
void glcdc_callback(glcdc_callback_args_t * p_args)
{
    if(GLCDC_EVENT_LINE_DETECTION == p_args->event) {
        give_vsync_sem_and_yield();
    }
    else if(GLCDC_EVENT_GR1_UNDERFLOW == p_args->event) {
        while(1); /*Layer 1 Underrun*/
    }
    else if(GLCDC_EVENT_GR2_UNDERFLOW == p_args->event) {
        while(1); /*Layer 2 Underrun*/
    }
    else {/*DISPLAY_EVENT_FRAME_END*/
        while(1);
    }
}
#endif /*_RENESAS_RA_*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_display_t * glcdc_create(void * buf1, void * buf2, uint32_t buf_size, lv_display_render_mode_t render_mode)
{
#ifdef _RENESAS_RA_
    glcdc_init();
#else
    g_display0_cfg.input->format = LCD_CH0_IN_GR2_FORMAT;
    _SemaphoreVsync = xSemaphoreCreateBinary();

    glcdc_init();
    enable_dave2d_drw_interrupt();
#endif /*_RENESAS_RA_*/

    lv_display_t * display = lv_display_create(DISPLAY_HSIZE_INPUT0, DISPLAY_VSIZE_INPUT0);

    if(render_mode == LV_DISPLAY_RENDER_MODE_DIRECT) {
        lv_display_set_flush_cb(display, flush_direct);
        lv_display_set_flush_wait_cb(display, flush_wait_direct);
    }
    else if(render_mode == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        lv_display_set_flush_cb(display, flush_partial);
        lv_display_set_flush_wait_cb(display, flush_wait_partial);
    }
    else {
        LV_ASSERT(0);
    }

    lv_display_set_buffers(display, buf1, buf2, buf_size, render_mode);

    return display;
}

static void give_vsync_sem_and_yield(void)
{
#if USE_FREE_RTOS
    BaseType_t context_switch;

    /*Set Vsync semaphore*/
    xSemaphoreGiveFromISR(_SemaphoreVsync, &context_switch);

    /*Return to the highest priority available task*/
    portYIELD_FROM_ISR(context_switch);
#else
#endif /*USE_FREE_RTOS*/
}

static void glcdc_init(void)
{
    /* Fill the Frame buffer with black colour (0x0000 in RGB565), for a clean start after previous runs */
    lv_memzero(fb_background, sizeof(fb_background));

#ifdef _RENESAS_RA_
    /* Initialize GLCDC driver */
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
#else /* RX */
    glcdc_err_t err;
    glcdc_runtime_cfg_t  layer_change;

    R_GLCDC_PinSet();

    err = R_GLCDC_Open(&g_config);
    if(GLCDC_SUCCESS != err) {
        while(1);
    }

    err = R_GLCDC_Control(GLCDC_CMD_START_DISPLAY, &g_config);
    if(GLCDC_SUCCESS != err) {
        while(1);
    }

    g_layer_change.input = g_config.input[GLCDC_FRAME_LAYER_2];
    g_layer_change.chromakey = g_config.chromakey[GLCDC_FRAME_LAYER_2];
    g_layer_change.blend = g_config.blend[GLCDC_FRAME_LAYER_2];

    layer_change.input.p_base = (uint32_t *)&fb_background[1][0];

    do {
        err = R_GLCDC_LayerChange(GLCDC_FRAME_LAYER_2, &g_layer_change);
    } while(GLCDC_ERR_INVALID_UPDATE_TIMING == err);
#endif /*_RENESAS_RA_*/
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

#ifdef _RENESAS_RA_
    R_GLCDC_BufferChange(&g_display0_ctrl,
                         (uint8_t *) px_map,
                         (display_frame_layer_t) 0);
#else /* RX */
    glcdc_err_t err;

    g_layer_change.input.p_base = (uint32_t *)px_map;

    do {
        err = R_GLCDC_LayerChange(GLCDC_FRAME_LAYER_2, &g_layer_change);
    } while(GLCDC_ERR_INVALID_UPDATE_TIMING == err);
#endif /*_RENESAS_RA_*/
}

static void flush_wait_direct(lv_display_t * display)
{
    if(!lv_display_flush_is_last(display)) return;

#if USE_FREE_RTOS
    /*If Vsync semaphore has already been set, clear it then wait to avoid tearing*/
    if(uxSemaphoreGetCount(_SemaphoreVsync)) {
        xSemaphoreTake(_SemaphoreVsync, 10);
    }

    xSemaphoreTake(_SemaphoreVsync, portMAX_DELAY);
#endif /*USE_FREE_RTOS*/

}

static void flush_partial(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    uint16_t * img = (uint16_t *)px_map;

    lv_area_t rotated_area;
    lv_color_format_t cf = lv_display_get_color_format(display);
    lv_display_rotation_t rotation = lv_display_get_rotation(display);

    if(rotation != LV_DISPLAY_ROTATION_0) {
        int32_t w = lv_area_get_width(area);
        int32_t h = lv_area_get_height(area);
        uint32_t w_stride = lv_draw_buf_width_to_stride(w, cf);
        uint32_t h_stride = lv_draw_buf_width_to_stride(h, cf);

        // only allocate if rotation is actually being used
        if(!rotation_buffer) {
            rotation_buffer = lv_malloc(partial_buffer_size);
            LV_ASSERT_MALLOC(rotation_buffer);
        }

        if(rotation == LV_DISPLAY_ROTATION_180)
            lv_draw_sw_rotate(img, rotation_buffer, w, h, w_stride, w_stride, rotation, cf);
        else /* 90 or 270 */
            lv_draw_sw_rotate(img, rotation_buffer, w, h, w_stride, h_stride, rotation, cf);

        img = rotation_buffer;

        rotated_area = *area;
        lv_display_rotate_area(display, &rotated_area);
        area = &rotated_area;
    }

    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    uint16_t * fb = (uint16_t *)fb_background[1];

    fb = fb + area->y1 * DISPLAY_HSIZE_INPUT0;
    fb = fb + area->x1;

    int32_t i;
    for(i = 0; i < h; i++) {
        lv_memcpy(fb, img, w * BYTES_PER_PIXEL);

#if defined(RENESAS_CORTEX_M85) && (BSP_CFG_DCACHE_ENABLED)
        SCB_CleanInvalidateDCache_by_Addr(fb, w * BYTES_PER_PIXEL);
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

#ifdef _RENESAS_RX_
extern void drw_int_isr(void);

static void enable_dave2d_drw_interrupt(void)
{
    bsp_int_ctrl_t grpal1;

    /* Specify the priority of the group interrupt. */
    grpal1.ipl = 5;

    /* Use the BSP API to register the interrupt handler for DRW2D. */
    R_BSP_InterruptWrite(BSP_INT_SRC_AL1_DRW2D_DRW_IRQ, (bsp_int_cb_t)drw_int_isr);

    /* Use the BSP API to enable the group interrupt. */
    R_BSP_InterruptControl(BSP_INT_SRC_AL1_DRW2D_DRW_IRQ, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, (void *)&grpal1);
}
#endif /*_RENESAS_RX_*/

#endif /*LV_USE_RENESAS_GLCDC*/
