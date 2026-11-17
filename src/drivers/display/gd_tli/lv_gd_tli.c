/**
 * @file lv_gd_tli.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"
#if LV_USE_GD_TLI

#include "../../../display/lv_display_private.h"
#include "../../../draw/sw/lv_draw_sw.h"
#include "../../../osal/lv_os_private.h"
#include LV_DRIVER_TLI_HAL_INCLUDE

#if LV_GD_TLI_USE_IPA_FLUSH && LV_USE_DRAW_IPA
    #error cannot use LV_GD_TLI_USE_IPA_FLUSH with LV_USE_DRAW_IPA
#endif

/*********************
 *      DEFINES
 *********************/

#define MAX_LAYER 2U

/*Must be numerically >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY because the ISRs call FromISR APIs*/
#define LV_GD_TLI_IRQ_PRIORITY 5U

#define LAYER_BASE(layer_idx) ((layer_idx) == 0U ? LAYER0 : LAYER1)

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
static void clean_dcache(void);

#if LV_GD_TLI_USE_IPA_FLUSH
    static uint32_t get_ipa_output_cf_from_layer_cf(uint32_t cf);
    static uint32_t get_ipa_input_cf_from_lv_cf(uint32_t cf);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

static struct {
    bool disp_flushed_in_flush_cb[MAX_LAYER];
    sync_t sync[MAX_LAYER];
    volatile bool layer_interrupt_is_owned[MAX_LAYER];
#if LV_GD_TLI_USE_IPA_FLUSH
    volatile uint32_t ipa_interrupt_owner; /*layer_idx + 1, or 0 for none*/
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

lv_display_t * lv_gd_tli_create_direct(void * fb_adr_1, void * fb_adr_2, uint32_t layer_idx)
{
    LV_CHECK_ARG(fb_adr_1 != NULL, return NULL);
    return create(fb_adr_1, fb_adr_2, 0, layer_idx, LV_DISPLAY_RENDER_MODE_DIRECT);
}

lv_display_t * lv_gd_tli_create_partial(void * render_buf_1, void * render_buf_2, uint32_t buf_size,
                                        uint32_t layer_idx)
{
    LV_CHECK_ARG(render_buf_1 != NULL, return NULL);
    return create(render_buf_1, render_buf_2, buf_size, layer_idx, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_display_t * create(void * buf1, void * buf2, uint32_t buf_size, uint32_t layer_idx,
                             lv_display_render_mode_t mode)
{
    LV_CHECK_ARG(layer_idx < MAX_LAYER, return NULL);

    uint32_t layer = LAYER_BASE(layer_idx);
    uint32_t hpos = TLI_LXHPOS(layer);

    uint32_t layer_width = ((hpos & TLI_LXHPOS_WRP) >> 16) - (hpos & TLI_LXHPOS_WLP) + 1;
    uint32_t layer_height = TLI_LXFTLN(layer) & TLI_LXFTLN_FTLN;
    uint32_t layer_cf = TLI_LXPPF(layer) & TLI_LXPPF_PPF;
    lv_color_format_t cf = get_lv_cf_from_layer_cf(layer_cf);
    if(cf == LV_COLOR_FORMAT_UNKNOWN) {
        LV_LOG_ERROR("the TLI layer color format %u is not supported", (unsigned int)layer_cf);
        return NULL;
    }

#if LV_GD_TLI_USE_IPA_FLUSH
    if(mode == LV_DISPLAY_RENDER_MODE_PARTIAL && (cf == LV_COLOR_FORMAT_L8 || cf == LV_COLOR_FORMAT_AL88)) {
        LV_LOG_ERROR("IPA flush cannot output to the TLI layer color format %u", (unsigned int)layer_cf);
        return NULL;
    }
#endif /*LV_GD_TLI_USE_IPA_FLUSH*/

    lv_display_t * disp = lv_display_create(layer_width, layer_height);
    lv_display_set_color_format(disp, cf);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_flush_wait_cb(disp, flush_wait_cb);
    lv_display_set_driver_data(disp, (void *)(uintptr_t)layer_idx);

    if(mode == LV_DISPLAY_RENDER_MODE_DIRECT) {
        uint32_t cf_size = lv_color_format_get_size(cf);
        lv_display_set_buffers(disp, buf1, buf2, layer_width * layer_height * cf_size, LV_DISPLAY_RENDER_MODE_DIRECT);

        if(buf1 != NULL && buf2 != NULL) {
            /* layer reload finished interrupt */
            TLI_INTC = TLI_INTC_LCRC;
            TLI_INTEN |= TLI_INTEN_LCRIE;
            nvic_irq_enable(TLI_IRQn, LV_GD_TLI_IRQ_PRIORITY, 0U);
            SYNC_INIT(layer_idx);
        }
    }
    else {
        lv_display_set_buffers(disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

#if LV_GD_TLI_USE_IPA_FLUSH
        RCU_AHB3EN |= RCU_AHB3EN_IPAEN;
        IPA_ITCTL = 0U;
        nvic_irq_enable(IPA_IRQn, LV_GD_TLI_IRQ_PRIORITY, 0U);
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
               while TLI is scanning it. Clean it in the last flush, at least,
               but not every flush because it's expensive for not much visual improvement. */
            clean_dcache();
        }
        if(flush_is_last && lv_display_is_double_buffered(disp)) {
            TLI_LXFBADDR(LAYER_BASE(layer_idx)) = (uint32_t)px_map;
            g_data.layer_interrupt_is_owned[layer_idx] = true;
            TLI_RL |= TLI_RL_FBR;
        }
        else {
            g_data.disp_flushed_in_flush_cb[layer_idx] = true;
        }
    }
    else {
        uint32_t layer = LAYER_BASE(layer_idx);

        lv_color_format_t cf = lv_display_get_color_format(disp);

        uint8_t * fb = (uint8_t *)TLI_LXFBADDR(layer);
        uint32_t px_size = lv_color_format_get_size(cf);
        uint32_t fb_stride = (TLI_LXFLLEN(layer) & TLI_LXFLLEN_STDOFF) >> 16;
        lv_area_t rotated_area = *area;
        lv_display_rotate_area(disp, &rotated_area);
        uint8_t * first_pixel = fb + fb_stride * rotated_area.y1 + px_size * rotated_area.x1;

        int32_t area_width = lv_area_get_width(area);
        int32_t area_height = lv_area_get_height(area);

        lv_rotation_t rotation = lv_display_get_rotation(disp);
        if(rotation == LV_ROTATION_0) {
#if LV_GD_TLI_USE_IPA_FLUSH
            clean_dcache();

            uint32_t ipa_input_cf = get_ipa_input_cf_from_lv_cf(cf);
            uint32_t ipa_output_cf = get_ipa_output_cf_from_layer_cf(TLI_LXPPF(layer) & TLI_LXPPF_PPF);

            while(IPA_CTL & IPA_CTL_TEN);
            IPA_FMADDR = (uint32_t)px_map;
            IPA_FLOFF = 0U;
            IPA_FPCTL = ipa_input_cf;
            IPA_DMADDR = (uint32_t)first_pixel;
            IPA_DLOFF = (fb_stride / px_size) - (uint32_t)area_width;
            IPA_DPCTL = ipa_output_cf;
            IPA_IMS = ((uint32_t)area_width << 16) | (uint32_t)area_height;
            g_data.ipa_interrupt_owner = layer_idx + 1;
            __DSB();
            IPA_CTL = IPA_CTL_TEN | IPA_FGTODE_PF_CONVERT | IPA_CTL_FTFIE;
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
#if LV_USE_DRAW_SW
            uint32_t area_stride = px_size * area_width;
            lv_draw_rotate(px_map, first_pixel, area_width, area_height, area_stride, fb_stride, rotation, cf);
            clean_dcache();
            g_data.disp_flushed_in_flush_cb[layer_idx] = true;
#else
            LV_LOG_WARN("LV_USE_DRAW_SW needs to be enabled to rotate the display's content");
            g_data.disp_flushed_in_flush_cb[layer_idx] = true;
#endif
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
        case LAYER_PPF_ARGB8888:
            return LV_COLOR_FORMAT_ARGB8888;
        case LAYER_PPF_RGB888:
            return LV_COLOR_FORMAT_RGB888;
        case LAYER_PPF_RGB565:
            return LV_COLOR_FORMAT_RGB565;
        case LAYER_PPF_L8:
            return LV_COLOR_FORMAT_L8;
        case LAYER_PPF_AL88:
            return LV_COLOR_FORMAT_AL88;
        default:
            LV_ASSERT_MSG(0, "the TLI color format is not supported");
    }
    return LV_COLOR_FORMAT_UNKNOWN;
}

void lv_gd_tli_irq_handler(void)
{
    if(TLI_INTF & TLI_INTF_LCRF) {
        TLI_INTC = TLI_INTC_LCRC;
        for(uint32_t i = 0; i < MAX_LAYER; i++) {
            if(g_data.layer_interrupt_is_owned[i]) {
                g_data.layer_interrupt_is_owned[i] = false;
                SYNC_SIGNAL_ISR(i);
            }
        }
    }
}

#if LV_GD_TLI_DEFINE_IRQ_HANDLERS
void TLI_IRQHandler(void)
{
    lv_gd_tli_irq_handler();
}
#endif

static void clean_dcache(void)
{
#if defined(__CORTEX_M) && ((__CORTEX_M == 7) || (__CORTEX_M == 55))
    SCB_CleanDCache();
#elif defined(__CORTEX_A) && __CORTEX_A == 7
    L1C_CleanDCacheAll();
#endif
}

#if LV_GD_TLI_USE_IPA_FLUSH
void lv_gd_tli_ipa_irq_handler(void)
{
    if(IPA_INTF & IPA_INTF_FTFIF) {
        IPA_INTC = IPA_INTC_FTFIFC;
        uint32_t owner = g_data.ipa_interrupt_owner;
        if(owner) {
            g_data.ipa_interrupt_owner = 0;
            owner -= 1;
            SYNC_SIGNAL_ISR(owner);
        }
    }
}

#if LV_GD_IPA_DEFINE_IRQ_HANDLERS
void IPA_IRQHandler(void)
{
    lv_gd_tli_ipa_irq_handler();
}
#endif

static uint32_t get_ipa_output_cf_from_layer_cf(uint32_t cf)
{
    switch(cf) {
        case LAYER_PPF_ARGB8888:
            return IPA_DPF_ARGB8888;
        case LAYER_PPF_RGB888:
            return IPA_DPF_RGB888;
        case LAYER_PPF_RGB565:
            return IPA_DPF_RGB565;
        default:
            LV_ASSERT_MSG(0, "IPA cannot output to the TLI color format");
    }
    return IPA_DPF_ARGB8888;
}

static uint32_t get_ipa_input_cf_from_lv_cf(uint32_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
            return FOREGROUND_PPF_ARGB8888;
        case LV_COLOR_FORMAT_RGB888:
            return FOREGROUND_PPF_RGB888;
        case LV_COLOR_FORMAT_RGB565:
            return FOREGROUND_PPF_RGB565;
        case LV_COLOR_FORMAT_L8:
            return FOREGROUND_PPF_L8;
        case LV_COLOR_FORMAT_AL88:
            return FOREGROUND_PPF_AL88;
        case LV_COLOR_FORMAT_A8:
            return FOREGROUND_PPF_A8;
        default:
            LV_ASSERT_MSG(0, "the LVGL color format is not an IPA input color format");
    }
    return FOREGROUND_PPF_ARGB8888;
}
#endif /*LV_GD_TLI_USE_IPA_FLUSH*/

#endif /*LV_USE_GD_TLI*/
