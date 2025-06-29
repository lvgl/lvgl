/**
 * @file lv_draw_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_dma2d_private.h"
#if LV_USE_DRAW_DMA2D

#include "../sw/lv_draw_sw.h"
#include "../../misc/lv_area_private.h"

#if !LV_DRAW_DMA2D_ASYNC && LV_USE_DRAW_DMA2D_INTERRUPT
    #warning LV_USE_DRAW_DMA2D_INTERRUPT is 1 but has no effect because LV_USE_OS is LV_OS_NONE
#endif

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_DMA2D 5

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t evaluate_cb(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t dispatch_cb(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t delete_cb(lv_draw_unit_t * draw_unit);
#if LV_DRAW_DMA2D_ASYNC
    static void thread_cb(void * arg);
#endif
#if !LV_DRAW_DMA2D_ASYNC
    static bool check_transfer_completion(void);
#endif
static void post_transfer_tasks(lv_draw_dma2d_unit_t * u);

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_DRAW_DMA2D_ASYNC
    static lv_draw_dma2d_unit_t * g_unit;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_dma2d_init(void)
{
    lv_draw_dma2d_unit_t * draw_dma2d_unit = lv_draw_create_unit(sizeof(lv_draw_dma2d_unit_t));
    draw_dma2d_unit->base_unit.evaluate_cb = evaluate_cb;
    draw_dma2d_unit->base_unit.dispatch_cb = dispatch_cb;
    draw_dma2d_unit->base_unit.delete_cb = delete_cb;
    draw_dma2d_unit->base_unit.name = "DMA2D";

#if LV_DRAW_DMA2D_ASYNC
    g_unit = draw_dma2d_unit;

    lv_result_t res = lv_thread_init(&draw_dma2d_unit->thread, "dma2d", LV_DRAW_THREAD_PRIO, thread_cb, 2 * 1024,
                                     draw_dma2d_unit);
    LV_ASSERT(res == LV_RESULT_OK);
#endif

    /* enable the DMA2D clock */
#if defined(STM32F4) || defined(STM32F7) || defined(STM32U5) || defined(STM32L4)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;
#elif defined(STM32H7)
    RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;
#elif defined(STM32H7RS)
    RCC->AHB5ENR |= RCC_AHB5ENR_DMA2DEN;
#else
#warning "LVGL can't enable the clock for DMA2D"
#endif

    /* disable dead time */
    DMA2D->AMTCR = 0;

    /* enable the interrupt */
    NVIC_EnableIRQ(DMA2D_IRQn);
}

void lv_draw_dma2d_deinit(void)
{
    /* disable the interrupt */
    NVIC_DisableIRQ(DMA2D_IRQn);

    /* disable the DMA2D clock */
#if defined(STM32F4) || defined(STM32F7) || defined(STM32U5) || defined(STM32L4)
    RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA2DEN;
#elif defined(STM32H7)
    RCC->AHB3ENR &= ~RCC_AHB3ENR_DMA2DEN;
#elif defined(STM32H7RS)
    RCC->AHB5ENR &= ~RCC_AHB5ENR_DMA2DEN;
#endif

#if LV_DRAW_DMA2D_ASYNC
    lv_result_t res = lv_thread_delete(&g_unit->thread);
    LV_ASSERT(res == LV_RESULT_OK);

    res = lv_thread_sync_delete(&g_unit->interrupt_signal);
    LV_ASSERT(res == LV_RESULT_OK);

    g_unit = NULL;
#endif
}

#if LV_USE_DRAW_DMA2D_INTERRUPT
void lv_draw_dma2d_transfer_complete_interrupt_handler(void)
{
#if LV_DRAW_DMA2D_ASYNC
    lv_thread_sync_signal_isr(&g_unit->interrupt_signal);
#endif
}
#endif

lv_draw_dma2d_output_cf_t lv_draw_dma2d_cf_to_dma2d_output_cf(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            return LV_DRAW_DMA2D_OUTPUT_CF_ARGB8888;
        case LV_COLOR_FORMAT_RGB888:
            return LV_DRAW_DMA2D_OUTPUT_CF_RGB888;
        case LV_COLOR_FORMAT_RGB565:
            return LV_DRAW_DMA2D_OUTPUT_CF_RGB565;
        case LV_COLOR_FORMAT_ARGB1555:
            return LV_DRAW_DMA2D_OUTPUT_CF_ARGB1555;
        default:
            LV_ASSERT_MSG(false, "unsupported output color format");
    }
    return LV_DRAW_DMA2D_OUTPUT_CF_RGB565;
}

uint32_t lv_draw_dma2d_color_to_dma2d_color(lv_draw_dma2d_output_cf_t cf, lv_color_t color)
{
    switch(cf) {
        case LV_DRAW_DMA2D_OUTPUT_CF_ARGB8888:
        case LV_DRAW_DMA2D_OUTPUT_CF_RGB888:
            return lv_color_to_u32(color);
        case LV_DRAW_DMA2D_OUTPUT_CF_RGB565:
            return lv_color_to_u16(color);
        default:
            LV_ASSERT_MSG(false, "unsupported output color format");
    }
    return 0;
}

void lv_draw_dma2d_configure_and_start_transfer(const lv_draw_dma2d_configuration_t * conf)
{
    /* number of lines register */
    DMA2D->NLR = (conf->w << DMA2D_NLR_PL_Pos) | (conf->h << DMA2D_NLR_NL_Pos);

    /* output */

    /* output memory address register */
    DMA2D->OMAR = (uint32_t)(uintptr_t) conf->output_address;
    /* output offset register */
    DMA2D->OOR = conf->output_offset;
    /* output pixel format converter control register */
    DMA2D->OPFCCR = ((uint32_t) conf->output_cf) << DMA2D_OPFCCR_CM_Pos;

    /* Fill color. Only for mode LV_DRAW_DMA2D_MODE_REGISTER_TO_MEMORY */
    DMA2D->OCOLR = conf->reg_to_mem_mode_color;

    /* foreground */

    /* foreground memory address register */
    DMA2D->FGMAR = (uint32_t)(uintptr_t) conf->fg_address;
    /* foreground offset register */
    DMA2D->FGOR = conf->fg_offset;
    /* foreground color. only for mem-to-mem with blending and fixed-color foreground */
    DMA2D->FGCOLR = conf->fg_color;
    /* foreground pixel format converter control register */
    DMA2D->FGPFCCR = (((uint32_t) conf->fg_cf) << DMA2D_FGPFCCR_CM_Pos)
                     | (conf->fg_alpha << DMA2D_FGPFCCR_ALPHA_Pos)
                     | (conf->fg_alpha_mode << DMA2D_FGPFCCR_AM_Pos);

    /* background */

    DMA2D->BGMAR = (uint32_t)(uintptr_t) conf->bg_address;
    DMA2D->BGOR = conf->bg_offset;
    DMA2D->BGCOLR = conf->bg_color;
    DMA2D->BGPFCCR = (((uint32_t) conf->bg_cf) << DMA2D_BGPFCCR_CM_Pos)
                     | (conf->bg_alpha << DMA2D_BGPFCCR_ALPHA_Pos)
                     | (conf->bg_alpha_mode << DMA2D_BGPFCCR_AM_Pos);

    /* ensure the DMA2D register values are observed before the start transfer bit is set */
    __DSB();

    /* start the transfer (also set mode and enable transfer complete interrupt) */
    DMA2D->CR = DMA2D_CR_START | (((uint32_t) conf->mode) << DMA2D_CR_MODE_Pos)
#if LV_USE_DRAW_DMA2D_INTERRUPT
                | DMA2D_CR_TCIE
#endif
                ;
}

#if LV_DRAW_DMA2D_CACHE
void lv_draw_dma2d_invalidate_cache(const lv_draw_dma2d_cache_area_t * mem_area)
{
    if(SCB->CCR & SCB_CCR_DC_Msk) {
        SCB_InvalidateDCache();
    }
}

void lv_draw_dma2d_clean_cache(const lv_draw_dma2d_cache_area_t * mem_area)
{
    if(SCB->CCR & SCB_CCR_DC_Msk) {
        SCB_CleanDCache();
    }
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
                     && dsc->scale_x == 256
                     && dsc->scale_y == 256
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
            }
            break;
        default:
            return 0;
    }

    task->preferred_draw_unit_id = DRAW_UNIT_ID_DMA2D;
    task->preference_score = 0;

    return 0;
}

static int32_t dispatch_cb(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_dma2d_unit_t * draw_dma2d_unit = (lv_draw_dma2d_unit_t *) draw_unit;

    if(draw_dma2d_unit->task_act) {
#if LV_DRAW_DMA2D_ASYNC
        /*Return immediately if it's busy with draw task*/
        return 0;
#else
        if(!check_transfer_completion()) {
            return LV_DRAW_UNIT_IDLE;
        }
        post_transfer_tasks(draw_dma2d_unit);
#endif
    }

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_DMA2D);
    if(t == NULL) {
        return LV_DRAW_UNIT_IDLE;
    }

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) {
        return LV_DRAW_UNIT_IDLE;
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    t->draw_unit = draw_unit;
    draw_dma2d_unit->task_act = t;

    if(t->type == LV_DRAW_TASK_TYPE_FILL) {
        lv_draw_fill_dsc_t * dsc = t->draw_dsc;
        const lv_area_t * coords = &t->area;
        lv_area_t clipped_coords;
        if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) {
            return LV_DRAW_UNIT_IDLE;
        }

        void * dest = lv_draw_layer_go_to_xy(layer,
                                             clipped_coords.x1 - layer->buf_area.x1,
                                             clipped_coords.y1 - layer->buf_area.y1);

        if(dsc->opa >= LV_OPA_MAX) {
            lv_draw_dma2d_opaque_fill(t,
                                      dest,
                                      lv_area_get_width(&clipped_coords),
                                      lv_area_get_height(&clipped_coords),
                                      lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format));
        }
        else {
            lv_draw_dma2d_fill(t,
                               dest,
                               lv_area_get_width(&clipped_coords),
                               lv_area_get_height(&clipped_coords),
                               lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format));
        }
    }
    else if(t->type == LV_DRAW_TASK_TYPE_IMAGE) {
        lv_draw_image_dsc_t * dsc = t->draw_dsc;
        const lv_area_t * coords = &t->area;
        lv_area_t clipped_coords;
        if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) {
            return LV_DRAW_UNIT_IDLE;
        }

        if(dsc->opa >= LV_OPA_MAX) {
            lv_draw_dma2d_opaque_image(t, dsc, &t->area);
        }
        else {
            lv_draw_dma2d_image(t, dsc, &t->area);
        }
    }

#if !LV_DRAW_DMA2D_ASYNC
    lv_draw_dispatch_request();
#endif

    return 1;
}

static int32_t delete_cb(lv_draw_unit_t * draw_unit)
{
    return 0;
}

#if LV_DRAW_DMA2D_ASYNC
static void thread_cb(void * arg)
{
    lv_draw_dma2d_unit_t * u = arg;

    lv_thread_sync_init(&u->interrupt_signal);

    while(1) {

        do {
            lv_thread_sync_wait(&u->interrupt_signal);
        } while(u->task_act != NULL);

        post_transfer_tasks(u);
        lv_draw_dispatch_request();
    }
}
#endif

#if !LV_DRAW_DMA2D_ASYNC
static bool check_transfer_completion(void)
{
    return !(DMA2D->CR & DMA2D_CR_START);
}
#endif

static void post_transfer_tasks(lv_draw_dma2d_unit_t * u)
{
#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_invalidate_cache(&u->writing_area);
#endif
    u->task_act->state = LV_DRAW_TASK_STATE_READY;
    u->task_act = NULL;
}

#endif /*LV_USE_DRAW_DMA2D*/
