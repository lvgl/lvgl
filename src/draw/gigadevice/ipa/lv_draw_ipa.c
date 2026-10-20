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

#define DRAW_UNIT_ID_IPA 16

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t evaluate_cb(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t dispatch_cb(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t delete_cb(lv_draw_unit_t * draw_unit);

#if LV_DRAW_IPA_ASYNC
    static int32_t wait_finish_cb(lv_draw_unit_t * draw_unit);
#endif

static void post_transfer_tasks(lv_draw_ipa_unit_t * u);


#if LV_DRAW_IPA_CACHE
    static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
    static void flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

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
    draw_ipa_unit->base_unit.evaluate_cb = evaluate_cb;
    draw_ipa_unit->base_unit.dispatch_cb = dispatch_cb;
    draw_ipa_unit->base_unit.delete_cb = delete_cb;
#if LV_DRAW_IPA_ASYNC
    draw_ipa_unit->base_unit.wait_for_finish_cb = wait_finish_cb;
#endif
    draw_ipa_unit->base_unit.name = "IPA";

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
            return IPA_DPF_ARGB8888;
        case LV_COLOR_FORMAT_RGB888:
            return IPA_DPF_RGB888;
        case LV_COLOR_FORMAT_RGB565:
            return IPA_DPF_RGB565;
        case LV_COLOR_FORMAT_ARGB1555:
            return IPA_DPF_ARGB1555;
        case LV_COLOR_FORMAT_ARGB4444:
            return IPA_DPF_ARGB4444;
        default:
            LV_ASSERT_MSG(false, "unsupported output color format");
    }
    return IPA_DPF_RGB565;
}

uint32_t lv_draw_ipa_color_to_ipa_color(lv_draw_ipa_output_cf_t cf, lv_color_t color)
{
    switch(cf) {
        case IPA_DPF_ARGB8888:
        case IPA_DPF_RGB888:
            return lv_color_to_u32(color);
        case IPA_DPF_RGB565:
            return lv_color_to_u16(color);
        default:
            LV_ASSERT_MSG(false, "unsupported output color format");
    }
    return 0;
}

void lv_draw_ipa_configure_and_start_transfer(const lv_draw_ipa_configuration_t * conf)
{
    /* Check that addresses are valid regarding to alignment constraints */
    if(((conf->output_cf == IPA_DPF_ARGB8888) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x03)) ||
       ((conf->output_cf == IPA_DPF_RGB888) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x03)) ||
       ((conf->output_cf == IPA_DPF_RGB565) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x01)) ||
       ((conf->output_cf == IPA_DPF_ARGB1555) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x01)) ||
       ((conf->output_cf == IPA_DPF_ARGB4444) &&
        (((uint32_t)(lv_uintptr_t) conf->output_address) & 0x01))) {
        LV_LOG_WARN("Incompatible output address %p and format 0x%x",
                    conf->output_address, conf->output_cf);
    }
    if(((conf->fg_cf == IPA_DPF_ARGB8888) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x03)) ||
       ((conf->fg_cf == IPA_DPF_RGB888) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x03)) ||
       ((conf->fg_cf == IPA_DPF_RGB565) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x01)) ||
       ((conf->fg_cf == IPA_DPF_ARGB1555) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x01)) ||
       ((conf->fg_cf == IPA_DPF_ARGB4444) &&
        (((uint32_t)(lv_uintptr_t) conf->fg_address) & 0x01))) {
        LV_LOG_WARN("Incompatible foreground address %p and format 0x%x",
                    conf->fg_address, conf->fg_cf);
    }
    if(((conf->bg_cf == IPA_DPF_ARGB8888) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x03)) ||
       ((conf->bg_cf == IPA_DPF_RGB888) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x03)) ||
       ((conf->bg_cf == IPA_DPF_RGB565) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x01)) ||
       ((conf->bg_cf == IPA_DPF_ARGB1555) &&
        (((uint32_t)(lv_uintptr_t) conf->bg_address) & 0x01)) ||
       ((conf->bg_cf == IPA_DPF_ARGB4444) &&
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
    IPA_DPCTL = (temp & ~0x07U) | ((uint32_t)conf->output_cf & 0x07U);;

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
static int32_t evaluate_cb(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    switch(task->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                lv_draw_fill_dsc_t * dsc = task->draw_dsc;
                if(!(dsc->radius == 0
                     && dsc->grad.dir == LV_GRAD_DIR_NONE
                     && (dsc->base.layer->color_format == LV_COLOR_FORMAT_ARGB8888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_XRGB8888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB565))) {
                    return 0;
                }
            }
            break;
        case LV_DRAW_TASK_TYPE_LAYER: {
                const lv_draw_image_dsc_t * dsc = task->draw_dsc;
                const lv_layer_t * source_layer = dsc->src;
                if(source_layer == NULL || dsc->clip_radius || dsc->bitmap_mask_src || dsc->tile ||
                   dsc->blend_mode != LV_BLEND_MODE_NORMAL || dsc->recolor_opa > LV_OPA_MIN ||
                   dsc->skew_x || dsc->skew_y || dsc->rotation ||
                   dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
                    return 0;
                }
                if(!(source_layer->color_format == LV_COLOR_FORMAT_ARGB8888 ||
                     source_layer->color_format == LV_COLOR_FORMAT_XRGB8888 ||
                     source_layer->color_format == LV_COLOR_FORMAT_RGB888 ||
                     source_layer->color_format == LV_COLOR_FORMAT_RGB565) ||
                   !(dsc->base.layer->color_format == LV_COLOR_FORMAT_ARGB8888 ||
                     dsc->base.layer->color_format == LV_COLOR_FORMAT_XRGB8888 ||
                     dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB888 ||
                     dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB565)) {
                    return 0;
                }
                const lv_layer_t * target_layer = dsc->base.layer;
                uint32_t source_size = lv_color_format_get_size(source_layer->color_format);
                uint32_t target_size = lv_color_format_get_size(target_layer->color_format);
                uint32_t source_stride = source_layer->draw_buf ? source_layer->draw_buf->header.stride :
                                         lv_draw_buf_width_to_stride(lv_area_get_width(&source_layer->buf_area),
                                                                     source_layer->color_format);
                uint32_t target_stride = target_layer->draw_buf ? target_layer->draw_buf->header.stride :
                                         lv_draw_buf_width_to_stride(lv_area_get_width(&target_layer->buf_area),
                                                                     target_layer->color_format);
                if(source_stride % source_size || target_stride % target_size) return 0;
            }
            break;
        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * dsc = task->draw_dsc;
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
                     && (dsc->header.cf == LV_COLOR_FORMAT_ARGB8888
                         || dsc->header.cf == LV_COLOR_FORMAT_XRGB8888
                         || dsc->header.cf == LV_COLOR_FORMAT_RGB888
                         || dsc->header.cf == LV_COLOR_FORMAT_RGB565
                         || dsc->header.cf == LV_COLOR_FORMAT_ARGB1555)
                     && (dsc->base.layer->color_format == LV_COLOR_FORMAT_ARGB8888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_XRGB8888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB565))) {
                    return 0;
                }
                uint32_t source_size = lv_color_format_get_size(dsc->header.cf);
                if(dsc->header.stride % source_size) return 0;
#if LV_DRAW_BUF_STRIDE_ALIGN != 1
                uint32_t decoded_stride = lv_draw_buf_width_to_stride(dsc->header.w, dsc->header.cf);
                if(decoded_stride % source_size) return 0;
#endif
            }
            break;
        default:
            return 0;
    }

    const lv_layer_t * target_layer = task->target_layer;
    uint32_t target_size = lv_color_format_get_size(target_layer->color_format);
    uint32_t target_stride = target_layer->draw_buf ? target_layer->draw_buf->header.stride :
                             lv_draw_buf_width_to_stride(lv_area_get_width(&target_layer->buf_area),
                                                         target_layer->color_format);
    if(target_stride % target_size) return 0;

    task->preferred_draw_unit_id = DRAW_UNIT_ID_IPA;
    task->preference_score = 0;

    return 0;
}

static int32_t dispatch_cb(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_ipa_unit_t * draw_ipa_unit = (lv_draw_ipa_unit_t *) draw_unit;

    if(draw_ipa_unit->task_act) {
        /*Return immediately if it's busy with draw task*/
        return LV_DRAW_UNIT_IDLE;
    }

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_IPA);
    if(t == NULL) {
        return LV_DRAW_UNIT_IDLE;
    }

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) {
        t->state = LV_DRAW_TASK_STATE_FAILED;
        return LV_DRAW_UNIT_IDLE;
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    t->draw_unit = draw_unit;
    draw_ipa_unit->task_act = t;

    /* Abort rapidly if nothing to do */
    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, &t->area, &t->clip_area)) {
        draw_ipa_unit->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
        draw_ipa_unit->task_act = NULL;

        lv_draw_dispatch_request();
        return 1;
    }

    int32_t x = 0 - t->target_layer->buf_area.x1;
    int32_t y = 0 - t->target_layer->buf_area.y1;

    draw_ipa_unit->last_clipped_area = clipped_coords;
    lv_area_move(&draw_ipa_unit->last_clipped_area, x, y);

    /* Flush cache before drawing. This is a no-op when IPA_CACHE is disabled */
    lv_draw_buf_flush_cache(layer->draw_buf, &draw_ipa_unit->last_clipped_area);

    if(t->type == LV_DRAW_TASK_TYPE_FILL) {
        void * dest = lv_draw_layer_go_to_xy(layer, draw_ipa_unit->last_clipped_area.x1,
                                             draw_ipa_unit->last_clipped_area.y1);

        lv_draw_ipa_fill(t, dest,
                         lv_area_get_width(&clipped_coords),
                         lv_area_get_height(&clipped_coords),
                         layer->draw_buf->header.stride);
    }
    else if(t->type == LV_DRAW_TASK_TYPE_IMAGE) {
        lv_draw_ipa_image(t, t->draw_dsc, &t->area);
    }
    else if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        if(!lv_draw_ipa_layer(t, t->draw_dsc, &t->area)) {
            if(t->state != LV_DRAW_TASK_STATE_FAILED) t->state = LV_DRAW_TASK_STATE_FINISHED;
            draw_ipa_unit->task_act = NULL;
            lv_draw_dispatch_request();
            return 1;
        }
    }

#if LV_DRAW_IPA_ASYNC
    return LV_DRAW_UNIT_IDLE;
#else
    while(IPA_CTL & IPA_CTL_TEN);

    post_transfer_tasks(draw_ipa_unit);

    lv_draw_dispatch_request();

    return 1;
#endif
}

static int32_t delete_cb(lv_draw_unit_t * draw_unit)
{
    return 0;
}

#if LV_DRAW_IPA_ASYNC
static int32_t wait_finish_cb(lv_draw_unit_t * draw_unit)
{
    lv_draw_ipa_unit_t * u = (lv_draw_ipa_unit_t *) draw_unit;

    /* No need to wait if the IPA doesn't have task to complete */
    if(u->task_act == NULL) return 0;

    /* If a IPA task has been dispatched, wait its interrupt */
    lv_thread_sync_wait(&u->interrupt_signal);

    /* Then cleanup the IPA draw unit to accept a new task */
    post_transfer_tasks(u);
    return 0;
}
#endif /*LV_DRAW_IPA_ASYNC*/

static void post_transfer_tasks(lv_draw_ipa_unit_t * u)
{
    /* Invalidate cache after drawing. This is a no-op when IPA_CACHE is disabled */
    lv_draw_buf_invalidate_cache(u->task_act->target_layer->draw_buf, &u->last_clipped_area);

    u->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
    u->task_act = NULL;
}

#endif /*LV_USE_DRAW_IPA*/
