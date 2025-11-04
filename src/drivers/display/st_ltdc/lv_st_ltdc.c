/**
 * @file lv_st_ltdc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_ST_LTDC

#include "lv_st_ltdc.h"
#include "../../../display/lv_display_private.h"
#include "../../../draw/sw/lv_draw_sw.h"
#include "../../../osal/lv_os_private.h"
#include "main.h"

#if LV_ST_LTDC_USE_DMA2D_FLUSH
    #if LV_USE_DRAW_DMA2D
        #error cannot use LV_ST_LTDC_USE_DMA2D_FLUSH with LV_USE_DRAW_DMA2D
    #endif /*LV_USE_DRAW_DMA2D*/

    extern DMA2D_HandleTypeDef hdma2d;
#endif /*LV_ST_LTDC_USE_DMA2D_FLUSH*/

extern LTDC_HandleTypeDef hltdc;

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_OS != LV_OS_NONE
    typedef lv_thread_sync_t sync_t;
#else
    typedef volatile bool sync_t;
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_display_t * create(void * buf1, void * buf2, uint32_t buf_size, uint32_t layer_idx,
                             lv_display_render_mode_t mode);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void flush_wait_cb(lv_display_t * disp);
static lv_color_format_t get_lv_cf_from_layer_cf(uint32_t cf);
static void reload_event_callback(LTDC_HandleTypeDef * hltdc);
static void clean_dcache(void);

#if LV_ST_LTDC_USE_DMA2D_FLUSH
    static void transfer_complete_callback(DMA2D_HandleTypeDef * hdma2d);
    static uint32_t get_dma2d_output_cf_from_layer_cf(uint32_t cf);
    static uint32_t get_dma2d_input_cf_from_lv_cf(uint32_t cf);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

static struct {
    bool disp_flushed_in_flush_cb[MAX_LAYER];
    sync_t sync[MAX_LAYER];
    volatile bool layer_interrupt_is_owned[MAX_LAYER];
#if LV_ST_LTDC_USE_DMA2D_FLUSH
    volatile uint32_t dma2d_interrupt_owner; /*layer_idx + 1, or 0 for none*/
#endif
} g_data;

/**********************
 *      MACROS
 **********************/

#if LV_USE_OS != LV_OS_NONE
    #define SYNC_INIT(layer_idx) lv_thread_sync_init(&g_data.sync[layer_idx])
    #define SYNC_WAIT(layer_idx) lv_thread_sync_wait(&g_data.sync[layer_idx])
    #define SYNC_SIGNAL_ISR(layer_idx) lv_thread_sync_signal_isr(&g_data.sync[layer_idx])
#else
    #define SYNC_INIT(layer_idx) do { g_data.sync[layer_idx] = false; } while(0)
    #define SYNC_WAIT(layer_idx) do { while(!g_data.sync[layer_idx]); g_data.sync[layer_idx] = false; } while(0)
    #define SYNC_SIGNAL_ISR(layer_idx) do { g_data.sync[layer_idx] = true; } while(0)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_st_ltdc_create_direct(void * fb_adr_1, void * fb_adr_2, uint32_t layer_idx)
{
    return create(fb_adr_1, fb_adr_2, 0, layer_idx, LV_DISPLAY_RENDER_MODE_DIRECT);
}

lv_display_t * lv_st_ltdc_create_partial(void * render_buf_1, void * render_buf_2, uint32_t buf_size,
                                         uint32_t layer_idx)
{
    return create(render_buf_1, render_buf_2, buf_size, layer_idx, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_display_t * create(void * buf1, void * buf2, uint32_t buf_size, uint32_t layer_idx,
                             lv_display_render_mode_t mode)
{
    LTDC_LayerCfgTypeDef * layer_cfg = &hltdc.LayerCfg[layer_idx];
    uint32_t layer_width = layer_cfg->ImageWidth;
    uint32_t layer_height = layer_cfg->ImageHeight;
    uint32_t layer_cf = layer_cfg->PixelFormat;
    lv_color_format_t cf = get_lv_cf_from_layer_cf(layer_cf);

    lv_display_t * disp = lv_display_create(layer_width, layer_height);
    lv_display_set_color_format(disp, cf);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_flush_wait_cb(disp, flush_wait_cb);
    lv_display_set_driver_data(disp, (void *)(uintptr_t)layer_idx);

    if(mode == LV_DISPLAY_RENDER_MODE_DIRECT) {
        uint32_t cf_size = lv_color_format_get_size(cf);
        lv_display_set_buffers(disp, buf1, buf2, layer_width * layer_height * cf_size, LV_DISPLAY_RENDER_MODE_DIRECT);

        if(buf1 != NULL && buf2 != NULL) {
            HAL_LTDC_RegisterCallback(&hltdc, HAL_LTDC_RELOAD_EVENT_CB_ID, reload_event_callback);
            SYNC_INIT(layer_idx);
        }
    }
    else {
        lv_display_set_buffers(disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

#if LV_ST_LTDC_USE_DMA2D_FLUSH
        hdma2d.XferCpltCallback = transfer_complete_callback;
        SYNC_INIT(layer_idx);
#endif
    }

    return disp;
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    uint32_t layer_idx = (uint32_t)(uintptr_t)lv_display_get_driver_data(disp);
    g_data.disp_flushed_in_flush_cb[layer_idx] = false;

    if(disp->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT) {
        bool flush_is_last = lv_display_flush_is_last(disp);
        if(flush_is_last) {
            /* there is no ideal time to clean the cache (if present)
               for **single-buffered** direct mode because the active buffer is drawn to
               while LTDC is scanning it. Clean it in the last flush, at least,
               but not every flush because it's expensive for not much visual improvement. */
            clean_dcache();
        }
        if(flush_is_last && lv_display_is_double_buffered(disp)) {
            HAL_LTDC_SetAddress_NoReload(&hltdc, (uint32_t)px_map, layer_idx);
            g_data.layer_interrupt_is_owned[layer_idx] = true;
            HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
        }
        else {
            g_data.disp_flushed_in_flush_cb[layer_idx] = true;
        }
    }
    else {
        LTDC_LayerCfgTypeDef * layer_cfg = &hltdc.LayerCfg[layer_idx];

        lv_color_format_t cf = lv_display_get_color_format(disp);
        int32_t disp_width = disp->hor_res;

        uint8_t * fb = (uint8_t *) layer_cfg->FBStartAdress;
        uint32_t px_size = lv_color_format_get_size(cf);
        uint32_t fb_stride = px_size * disp_width;
        lv_area_t rotated_area = *area;
        lv_display_rotate_area(disp, &rotated_area);
        uint8_t * first_pixel = fb + fb_stride * rotated_area.y1 + px_size * rotated_area.x1;

        int32_t area_width = lv_area_get_width(area);
        int32_t area_height = lv_area_get_height(area);

        lv_display_rotation_t rotation = lv_display_get_rotation(disp);
        if(rotation == LV_DISPLAY_ROTATION_0) {
#if LV_ST_LTDC_USE_DMA2D_FLUSH
            clean_dcache();

            uint32_t dma2d_input_cf = get_dma2d_input_cf_from_lv_cf(cf);
            uint32_t dma2d_output_cf = get_dma2d_output_cf_from_layer_cf(layer_cfg->PixelFormat);

            while(DMA2D->CR & DMA2D_CR_START);
            DMA2D->FGPFCCR = dma2d_input_cf;
            DMA2D->FGMAR = (uint32_t)px_map;
            DMA2D->FGOR = 0;
            DMA2D->OPFCCR = dma2d_output_cf;
            DMA2D->OMAR = (uint32_t)first_pixel;
            DMA2D->OOR = disp_width - area_width;
            DMA2D->NLR = (area_width << DMA2D_NLR_PL_Pos) | (area_height << DMA2D_NLR_NL_Pos);
            g_data.dma2d_interrupt_owner = layer_idx + 1;
            DMA2D->CR = DMA2D_CR_START | DMA2D_CR_TCIE | (0x1U << DMA2D_CR_MODE_Pos); /* memory-to-memory with PFC */
#else
            uint32_t area_stride = px_size * area_width;
            uint8_t * fb_p = first_pixel;
            uint8_t * px_map_p = px_map;
            for(int i = 0; i < area_height; i++) {
                lv_memcpy(fb_p, px_map_p, area_stride);
                fb_p += fb_stride;
                px_map_p += area_stride;
            }
            clean_dcache();
            g_data.disp_flushed_in_flush_cb[layer_idx] = true;
#endif
        }
        else {
            uint32_t area_stride = px_size * area_width;
            lv_draw_sw_rotate(px_map, first_pixel, area_width, area_height, area_stride, fb_stride, rotation, cf);
            clean_dcache();
            g_data.disp_flushed_in_flush_cb[layer_idx] = true;
        }
    }
}

static void flush_wait_cb(lv_display_t * disp)
{
    uint32_t layer_idx = (uint32_t)(uintptr_t)lv_display_get_driver_data(disp);
    if(!g_data.disp_flushed_in_flush_cb[layer_idx]) {
        SYNC_WAIT(layer_idx);
    }
}

static lv_color_format_t get_lv_cf_from_layer_cf(uint32_t cf)
{
    switch(cf) {
        case LTDC_PIXEL_FORMAT_ARGB8888:
            return LV_COLOR_FORMAT_ARGB8888;
        case LTDC_PIXEL_FORMAT_RGB888:
            return LV_COLOR_FORMAT_RGB888;
        case LTDC_PIXEL_FORMAT_RGB565:
            return LV_COLOR_FORMAT_RGB565;
        case LTDC_PIXEL_FORMAT_L8:
            return LV_COLOR_FORMAT_L8;
        case LTDC_PIXEL_FORMAT_AL88:
            return LV_COLOR_FORMAT_AL88;
        default:
            LV_ASSERT_MSG(0, "the LTDC color format is not supported");
    }
}

static void reload_event_callback(LTDC_HandleTypeDef * hltdc)
{
    uint32_t i;
    for(i = 0; i < MAX_LAYER; i++) {
        if(g_data.layer_interrupt_is_owned[i]) {
            g_data.layer_interrupt_is_owned[i] = false;
            SYNC_SIGNAL_ISR(i);
        }
    }
}

static void clean_dcache(void)
{
#if defined(__CORTEX_M) && __CORTEX_M == 7
    SCB_CleanDCache();
#elif defined(__CORTEX_A) && __CORTEX_A == 7
    L1C_CleanDCacheAll();
#endif
}

#if LV_ST_LTDC_USE_DMA2D_FLUSH
static void transfer_complete_callback(DMA2D_HandleTypeDef * hdma2d)
{
    DMA2D->IFCR = 0x3FU;
    uint32_t owner = g_data.dma2d_interrupt_owner;
    if(owner) {
        g_data.dma2d_interrupt_owner = 0;
        owner -= 1;
        SYNC_SIGNAL_ISR(owner);
    }
}

static uint32_t get_dma2d_output_cf_from_layer_cf(uint32_t cf)
{
    switch(cf) {
        case LTDC_PIXEL_FORMAT_ARGB8888:
            return DMA2D_OUTPUT_ARGB8888;
        case LTDC_PIXEL_FORMAT_RGB888:
            return DMA2D_OUTPUT_RGB888;
        case LTDC_PIXEL_FORMAT_RGB565:
            return DMA2D_OUTPUT_RGB565;
        default:
            LV_ASSERT_MSG(0, "DMA2D cannot output to the LTDC color format");
    }
}

static uint32_t get_dma2d_input_cf_from_lv_cf(uint32_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
            return DMA2D_INPUT_ARGB8888;
        case LV_COLOR_FORMAT_RGB888:
            return DMA2D_INPUT_RGB888;
        case LV_COLOR_FORMAT_RGB565:
            return DMA2D_INPUT_RGB565;
        case LV_COLOR_FORMAT_L8:
            return DMA2D_INPUT_L8;
        case LV_COLOR_FORMAT_AL88:
            return DMA2D_INPUT_AL88;
        case LV_COLOR_FORMAT_A8:
            return DMA2D_INPUT_A8;
        default:
            LV_ASSERT_MSG(0, "the LVGL color format is not a DMA2D input color format");
    }
}
#endif /*LV_ST_LTDC_USE_DMA2D_FLUSH*/

#endif /*LV_USE_ST_LTDC*/
