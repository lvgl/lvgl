/**
 * @file lv_draw_ipa.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_ipa_private.h"
#if LV_USE_DRAW_IPA

#include "../../sw/lv_draw_sw.h"
#include "../../sw/lv_draw_sw_private.h"
#include "../../../misc/lv_area_private.h"
#include "../../lv_draw_buf_private.h"

#if LV_USE_OS == LV_OS_FREERTOS && LV_USE_DRAW_IPA_INTERRUPT
    #include <FreeRTOS.h>
#endif

#if !LV_DRAW_IPA_ASYNC && LV_USE_DRAW_IPA_INTERRUPT
    #warning LV_USE_DRAW_IPA_INTERRUPT is 1 but has no effect because LV_USE_OS is LV_OS_NONE
#endif

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool is_cf_supported(lv_color_format_t cf);
static bool is_dest_supported(const lv_layer_t * layer);
static bool can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static int32_t delete_cb(lv_draw_unit_t * draw_unit);

#if LV_DRAW_IPA_ASYNC
    static int32_t wait_finish_cb(lv_draw_unit_t * draw_unit);
#endif

static bool draw_begin(lv_draw_ipa_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_area_t * coords);
static void draw_end(lv_draw_ipa_unit_t * u, lv_layer_t * layer);
static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);


#if LV_DRAW_IPA_CACHE
    static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
    static void flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = draw_fill,
    .can_draw_fill_cb = can_draw_fill,
    .draw_image_cb = draw_image,
    .can_draw_image_cb = can_draw_image,
    .draw_layer_cb = draw_layer,
    .can_draw_layer_cb = can_draw_layer,
};

#if LV_DRAW_IPA_ASYNC
    static lv_draw_ipa_unit_t * g_unit;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_buf_ipa_init_handlers(void)
{
#if LV_DRAW_IPA_CACHE
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    lv_draw_buf_handlers_t * font_handlers = lv_draw_buf_get_font_handlers();
    lv_draw_buf_handlers_t * image_handlers = lv_draw_buf_get_image_handlers();
    handlers->invalidate_cache_cb = invalidate_cache;
    handlers->flush_cache_cb = flush_cache;
    font_handlers->invalidate_cache_cb = invalidate_cache;
    font_handlers->flush_cache_cb = flush_cache;
    image_handlers->invalidate_cache_cb = invalidate_cache;
    image_handlers->flush_cache_cb = flush_cache;
#endif
}

void lv_draw_ipa_init(void)
{
    lv_draw_buf_ipa_init_handlers();
    lv_draw_ipa_unit_t * draw_ipa_unit = lv_draw_create_unit(sizeof(lv_draw_ipa_unit_t));
    draw_ipa_unit->base_unit.delete_cb = delete_cb;
#if LV_DRAW_IPA_ASYNC
    draw_ipa_unit->base_unit.wait_for_finish_cb = wait_finish_cb;
#endif
    draw_ipa_unit->base_unit.name = "IPA";
    draw_ipa_unit->base_unit.draw_cbs = &draw_cbs;
    draw_ipa_unit->base_unit.fallback = lv_draw_sw_get_draw_unit();

#if LV_DRAW_IPA_ASYNC
    g_unit = draw_ipa_unit;
    lv_thread_sync_init(&draw_ipa_unit->interrupt_signal);
#endif

    /* enable the IPA clock */
#if defined(GD32H7XX)
    RCU_AHB3EN |= RCU_AHB3EN_IPAEN;
#elif defined(GD32F527) || defined(GD32F470)
    RCU_AHB1EN |= RCU_AHB1EN_IPAEN;
#else
#error "Unsupported IPA target: clock control is implemented only for GD32H7XX, GD32F527 and GD32F470"
#endif

    /* disable dead time */
    IPA_ITCTL = 0;

#if LV_USE_DRAW_IPA_INTERRUPT
#if LV_USE_OS == LV_OS_FREERTOS
    nvic_irq_enable(IPA_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
#else
    /* enable the interrupt */
    nvic_irq_enable(IPA_IRQn, 14, 0);
#endif
#endif
}

void lv_draw_ipa_deinit(void)
{
    /* disable the interrupt */
    nvic_irq_disable(IPA_IRQn);

    /* disable the IPA clock */
#if defined(GD32H7XX)
    RCU_AHB3EN &= ~RCU_AHB3EN_IPAEN;
#elif defined(GD32F527) || defined(GD32F470)
    RCU_AHB1EN &= ~RCU_AHB1EN_IPAEN;
#else
#error "Unsupported IPA target: clock control is implemented only for GD32H7XX, GD32F527 and GD32F470"
#endif

#if LV_DRAW_IPA_ASYNC
    lv_result_t res = lv_thread_sync_delete(&g_unit->interrupt_signal);
    LV_ASSERT(res == LV_RESULT_OK);

    g_unit = NULL;
#endif
}

#if LV_USE_DRAW_IPA_INTERRUPT
void lv_draw_ipa_transfer_complete_interrupt_handler(void)
{
#if LV_DRAW_IPA_ASYNC
    lv_thread_sync_signal_isr(&g_unit->interrupt_signal);
#endif
}
#endif

lv_draw_ipa_output_cf_t lv_draw_cf_to_ipa_output_cf(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            return LV_DRAW_IPA_OUTPUT_CF_ARGB8888;
        case LV_COLOR_FORMAT_RGB888:
            return LV_DRAW_IPA_OUTPUT_CF_RGB888;
        case LV_COLOR_FORMAT_RGB565:
            return LV_DRAW_IPA_OUTPUT_CF_RGB565;
        case LV_COLOR_FORMAT_ARGB1555:
            return LV_DRAW_IPA_OUTPUT_CF_ARGB1555;
        case LV_COLOR_FORMAT_ARGB4444:
            return LV_DRAW_IPA_OUTPUT_CF_ARGB4444;
        default:
            LV_ASSERT_MSG(false, "unsupported output color format");
    }
    return LV_DRAW_IPA_OUTPUT_CF_RGB565;
}

uint32_t lv_draw_ipa_color_to_ipa_color(lv_draw_ipa_output_cf_t cf, lv_color_t color)
{
    switch(cf) {
        case LV_DRAW_IPA_OUTPUT_CF_ARGB8888:
        case LV_DRAW_IPA_OUTPUT_CF_RGB888:
            return lv_color_to_u32(color);
        case LV_DRAW_IPA_OUTPUT_CF_RGB565:
            return lv_color_to_u16(color);
        default:
            LV_ASSERT_MSG(false, "unsupported output color format");
    }
    return 0;
}

void lv_draw_ipa_configure_and_start_transfer(lv_draw_ipa_unit_t * u, const lv_draw_ipa_configuration_t * conf)
{
    /* Check that addresses are valid regarding to alignment constraints */
    if(((conf->output_cf == LV_DRAW_IPA_OUTPUT_CF_ARGB8888) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x03)) ||
       ((conf->output_cf == LV_DRAW_IPA_OUTPUT_CF_RGB888) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x03)) ||
       ((conf->output_cf == LV_DRAW_IPA_OUTPUT_CF_RGB565) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x01)) ||
       ((conf->output_cf == LV_DRAW_IPA_OUTPUT_CF_ARGB1555) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x01)) ||
       ((conf->output_cf == LV_DRAW_IPA_OUTPUT_CF_ARGB4444) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x01))) {
        LV_LOG_WARN("Incompatible output address %p and format 0x%x",
                    conf->output_address, conf->output_cf);
    }
    if(((conf->fg_cf == LV_DRAW_IPA_FGBG_CF_ARGB8888) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x03)) ||
       ((conf->fg_cf == LV_DRAW_IPA_FGBG_CF_RGB888) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x03)) ||
       ((conf->fg_cf == LV_DRAW_IPA_FGBG_CF_RGB565) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x01)) ||
       ((conf->fg_cf == LV_DRAW_IPA_FGBG_CF_ARGB1555) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x01)) ||
       ((conf->fg_cf == LV_DRAW_IPA_FGBG_CF_ARGB4444) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x01))) {
        LV_LOG_WARN("Incompatible foreground address %p and format 0x%x",
                    conf->fg_address, conf->fg_cf);
    }
    if(((conf->bg_cf == LV_DRAW_IPA_FGBG_CF_ARGB8888) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x03)) ||
       ((conf->bg_cf == LV_DRAW_IPA_FGBG_CF_RGB888) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x03)) ||
       ((conf->bg_cf == LV_DRAW_IPA_FGBG_CF_RGB565) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x01)) ||
       ((conf->bg_cf == LV_DRAW_IPA_FGBG_CF_ARGB1555) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x01)) ||
       ((conf->bg_cf == LV_DRAW_IPA_FGBG_CF_ARGB4444) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x01))) {
        LV_LOG_WARN("Incompatible background address %p and format 0x%x",
                    conf->bg_address, conf->bg_cf);
    }

    /* number of lines register */
    IPA_IMS     = ((uint32_t)conf->w  << 16) | (uint32_t)conf->h;
    /* output */

    /* output memory address register */
    IPA_DMADDR = (uint32_t)(uintptr_t) conf->output_address;
    /* output offset register */
    IPA_DLOFF = conf->output_offset;
    /* output pixel format converter control register */
    uint32_t temp = 0;
    temp = IPA_DPCTL;
    IPA_DPCTL = (temp & ~0x07U) | ((uint32_t)conf->output_cf & 0x07U);

    /* Fill color. Only for mode LV_DRAW_IPA_MODE_REGISTER_TO_MEMORY */
    IPA_DPV = conf->reg_to_mem_mode_color;

    /* foreground */

    /* foreground memory address register */
    IPA_FMADDR = (uint32_t)(uintptr_t) conf->fg_address;
    /* foreground offset register */
    IPA_FLOFF = conf->fg_offset;
    /* foreground color. only for mem-to-mem with blending and fixed-color foreground */
    IPA_FPV = conf->fg_color;
    /* foreground pixel format converter control register */
    IPA_FPCTL = (((uint32_t) conf->fg_cf) << 0)
                | (conf->fg_alpha << 24)
                | (conf->fg_alpha_mode << 16);

    /* background */

    IPA_BMADDR = (uint32_t)(uintptr_t) conf->bg_address;
    IPA_BLOFF = conf->bg_offset;
    IPA_BPV = conf->bg_color;
    IPA_BPCTL = (((uint32_t) conf->bg_cf) << 0)
                | (conf->bg_alpha << 24)
                | (conf->bg_alpha_mode << 16);

    /* ensure the IPA register values are observed before the start transfer bit is set */
    __DSB();

    /* start the transfer (also set mode and enable transfer complete interrupt) */
    IPA_CTL = IPA_CTL_TEN | (((uint32_t) conf->mode) << 16)
#if LV_USE_DRAW_IPA_INTERRUPT
              | IPA_CTL_FTFIE
#endif
              ;

    u->transfer_started = true;
}


#if LV_DRAW_IPA_CACHE
static void __invalidate_flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area,
                                     bool flush)
{
    LV_ASSERT(draw_buf != NULL);
    LV_ASSERT(area != NULL);

    const lv_image_header_t * header = &draw_buf->header;
    uint32_t stride = header->stride;
    lv_color_format_t cf = header->cf;

    uint32_t bpp = lv_color_format_get_bpp(cf);
    int32_t lines = lv_area_get_height(area);

    if(lines <= 0 || bpp == 0U || stride == 0U) {
        return;
    }

    /* As area coordinates, x1, x2 and y1 are always expected to be values > 0 */
    LV_ASSERT(area->x1 >= 0);
    LV_ASSERT(area->x2 >= 0);
    LV_ASSERT(area->y1 >= 0);
    LV_ASSERT(area->x2 >= area->x1);

    uint64_t start_bit = (uint64_t)(uint32_t)area->x1 * (uint64_t)bpp;
    uint64_t end_bit = (uint64_t)((uint32_t)area->x2 + 1U) * (uint64_t)bpp;
    uint32_t start_byte = (uint32_t)(start_bit >> 3);
    uint32_t end_byte = (uint32_t)((end_bit + 7U) >> 3);
    int32_t bytes_to_flush_per_line = (int32_t)(end_byte - start_byte);
    uint8_t * address = draw_buf->data + start_byte + (stride * (uint32_t)area->y1);
    int32_t i = 0;

    if(bytes_to_flush_per_line <= 0) {
        return;
    }

    for(i = 0; i < lines; i++) {
        if(SCB->CCR & SCB_CCR_DC_Msk) {
            if(flush) {
                SCB_CleanDCache_by_Addr(address, bytes_to_flush_per_line);
            }
            else {
                SCB_InvalidateDCache_by_Addr(address, bytes_to_flush_per_line);
            }
        }
        address += stride;
    }
}

static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    __invalidate_flush_cache(draw_buf, area, false);
}

static void flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    __invalidate_flush_cache(draw_buf, area, true);
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool is_cf_supported(lv_color_format_t cf)
{
    return cf == LV_COLOR_FORMAT_ARGB8888
           || cf == LV_COLOR_FORMAT_XRGB8888
           || cf == LV_COLOR_FORMAT_RGB888
           || cf == LV_COLOR_FORMAT_RGB565;
}

static bool is_dest_supported(const lv_layer_t * layer)
{
    if(!is_cf_supported(layer->color_format)) return false;

    uint32_t size = lv_color_format_get_size(layer->color_format);
    uint32_t stride = layer->draw_buf ? layer->draw_buf->header.stride :
                      lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), layer->color_format);
    return stride % size == 0;
}

static bool can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    return dsc->radius == 0
           && dsc->grad.dir == LV_GRAD_DIR_NONE
           && is_dest_supported(layer);
}

static bool can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    const lv_layer_t * source_layer = dsc->src;
    if(source_layer == NULL || dsc->clip_radius || dsc->bitmap_mask_src || dsc->tile ||
       dsc->blend_mode != LV_BLEND_MODE_NORMAL || dsc->recolor_opa > LV_OPA_MIN ||
       dsc->skew_x || dsc->skew_y || dsc->rotation ||
       dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
        return false;
    }
    if(!is_cf_supported(source_layer->color_format)) return false;
    uint32_t source_size = lv_color_format_get_size(source_layer->color_format);
    uint32_t source_stride = source_layer->draw_buf ? source_layer->draw_buf->header.stride :
                             lv_draw_buf_width_to_stride(lv_area_get_width(&source_layer->buf_area),
                                                         source_layer->color_format);
    if(source_stride % source_size) return false;

    return is_dest_supported(layer);
}

static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    if(!(dsc->header.cf < LV_COLOR_FORMAT_PROPRIETARY_START
         && dsc->clip_radius == 0
         && dsc->bitmap_mask_src == NULL
         && dsc->sup == NULL
         && dsc->tile == 0
         && dsc->blend_mode == LV_BLEND_MODE_NORMAL
         && dsc->recolor_opa <= LV_OPA_MIN
         && dsc->skew_y == 0
         && dsc->skew_x == 0
         && dsc->scale_x == LV_SCALE_NONE
         && dsc->scale_y == LV_SCALE_NONE
         && dsc->rotation == 0
         && lv_image_src_get_type(dsc->src) == LV_IMAGE_SRC_VARIABLE
         && (is_cf_supported(dsc->header.cf) || dsc->header.cf == LV_COLOR_FORMAT_ARGB1555))) {
        return false;
    }
    uint32_t source_size = lv_color_format_get_size(dsc->header.cf);
    if(dsc->header.stride % source_size) return false;
#if LV_DRAW_BUF_STRIDE_ALIGN != 1
    uint32_t decoded_stride = lv_draw_buf_width_to_stride(dsc->header.w, dsc->header.cf);
    if(decoded_stride % source_size) return false;
#endif

    return is_dest_supported(layer);
}

static int32_t delete_cb(lv_draw_unit_t * draw_unit)
{
    return 0;
}

#if LV_DRAW_IPA_ASYNC
static int32_t wait_finish_cb(lv_draw_unit_t * draw_unit)
{
    lv_draw_ipa_unit_t * u = (lv_draw_ipa_unit_t *) draw_unit;

    /* No need to wait if no transfer is in flight */
    if(u->pending_buf == NULL) return 0;

    /* Wait for the transfer complete interrupt */
    lv_thread_sync_wait(&u->interrupt_signal);

    /* Invalidate cache after drawing. This is a no-op when IPA_CACHE is disabled */
    lv_draw_buf_invalidate_cache(u->pending_buf, &u->last_clipped_area);
    u->pending_buf = NULL;
    return 0;
}
#endif /*LV_DRAW_IPA_ASYNC*/

/**
 * Prepare a transfer: wait for the previous one and write the CPU cache of the drawn area back.
 * Return false if there is nothing to draw.
 */
static bool draw_begin(lv_draw_ipa_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_area_t * coords)
{
#if LV_DRAW_IPA_ASYNC
    /* The IPA runs one transfer at a time */
    wait_finish_cb(&u->base_unit);
#endif

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, clip_area)) return false;

    u->last_clipped_area = clipped_coords;
    lv_area_move(&u->last_clipped_area, -layer->buf_area.x1, -layer->buf_area.y1);

    /* Flush cache before drawing. This is a no-op when IPA_CACHE is disabled */
    lv_draw_buf_flush_cache(layer->draw_buf, &u->last_clipped_area);

    u->transfer_started = false;
    return true;
}

/**
 * Finish a transfer: wait for it (sync) or leave it in flight until the next draw or `wait_finish_cb` (async)
 */
static void draw_end(lv_draw_ipa_unit_t * u, lv_layer_t * layer)
{
    /* No transfer to wait for, e.g. an image that failed to decode */
    if(!u->transfer_started) return;

#if LV_DRAW_IPA_ASYNC
    u->pending_buf = layer->draw_buf;
#else
    while(IPA_CTL & IPA_CTL_TEN);

    /* Invalidate cache after drawing. This is a no-op when IPA_CACHE is disabled */
    lv_draw_buf_invalidate_cache(layer->draw_buf, &u->last_clipped_area);
#endif
}

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_ipa_unit_t * u = (lv_draw_ipa_unit_t *) draw_unit;
    if(!draw_begin(u, layer, clip_area, coords)) return;
    lv_draw_ipa_fill(draw_unit, layer, clip_area, dsc, coords);
    draw_end(u, layer);
}

static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_ipa_unit_t * u = (lv_draw_ipa_unit_t *) draw_unit;
    if(!draw_begin(u, layer, clip_area, coords)) return;
    lv_draw_ipa_image(draw_unit, layer, clip_area, dsc, coords);
    draw_end(u, layer);
}

static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_ipa_unit_t * u = (lv_draw_ipa_unit_t *) draw_unit;
    if(!draw_begin(u, layer, clip_area, coords)) return;
    lv_draw_ipa_layer(draw_unit, layer, clip_area, dsc, coords);
    draw_end(u, layer);
}

#endif /*LV_USE_DRAW_IPA*/
