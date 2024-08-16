/**
 * @file lv_draw_dma2d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_dma2d.h"
#if LV_USE_DRAW_DMA2D

#include "../lv_draw_private.h"
#include "../sw/lv_draw_sw.h"
#include "../../misc/lv_area_private.h"

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_dma2d.h"
#include "stm32h7b3i_discovery_lcd.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_DMA2D 5

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAW_DMA2D_OUTPUT_CF_ARGB8888 = 0,
    LV_DRAW_DMA2D_OUTPUT_CF_RGB888,
    LV_DRAW_DMA2D_OUTPUT_CF_RGB565,
    LV_DRAW_DMA2D_OUTPUT_CF_ARGB1555,
    LV_DRAW_DMA2D_OUTPUT_CF_ARGB4444
} lv_draw_dma2d_output_cf_t;

typedef enum {
    LV_DRAW_DMA2D_FGBG_CF_ARGB8888 = 0,
    LV_DRAW_DMA2D_FGBG_CF_RGB888,
    LV_DRAW_DMA2D_FGBG_CF_RGB565,
    LV_DRAW_DMA2D_FGBG_CF_ARGB1555,
    LV_DRAW_DMA2D_FGBG_CF_ARGB4444,
    LV_DRAW_DMA2D_FGBG_CF_L8,
    LV_DRAW_DMA2D_FGBG_CF_AL44,
    LV_DRAW_DMA2D_FGBG_CF_AL88,
    LV_DRAW_DMA2D_FGBG_CF_L4,
    LV_DRAW_DMA2D_FGBG_CF_A8,
    LV_DRAW_DMA2D_FGBG_CF_A4,
    LV_DRAW_DMA2D_FGBG_CF_YCBCR
} lv_draw_dma2d_fgbg_cf_t;

typedef enum {
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY = 0,
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_PFC,
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING,
    LV_DRAW_DMA2D_MODE_REGISTER_TO_MEMORY,
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIXED_COLOR_FG,
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIXED_COLOR_BG
} lv_draw_dma2d_mode_t;

typedef struct {
    lv_draw_dma2d_mode_t mode;
    uint32_t w;
    uint32_t h;

    void * output_address;
    uint32_t output_offset;
    lv_draw_dma2d_output_cf_t output_cf;

    uint32_t reg_to_mem_mode_color;

    uint32_t fg_color;
    uint32_t fg_alpha_mode; /* 3 possible modes - 0x0 to 0x2 */
    uint32_t fg_alpha;

    void * bg_address;
    uint32_t bg_offset;
    lv_draw_dma2d_fgbg_cf_t bg_cf;

} lv_draw_dma2d_configuration_t;

typedef struct {
    void * first_byte;
    uint32_t width_bytes;
    uint32_t height;
    uint32_t stride;
} lv_draw_dma2d_cache_area_t;

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
    lv_draw_dma2d_cache_area_t writing_area;
#if LV_USE_OS
    lv_thread_t thread;
#if LV_USE_DRAW_DMA2D_INTERRUPT
    lv_thread_sync_t interrupt_signal;
#else
    lv_thread_sync_t thread_begin_polling_signal;
#endif
#endif
} lv_draw_dma2d_unit_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t evaluate_cb(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t dispatch_cb(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t delete_cb(lv_draw_unit_t * draw_unit);
static void opaque_fill(void * first_pixel, int32_t w, int32_t h, int32_t stride, lv_color_t color,
                        lv_color_format_t cf, lv_draw_dma2d_unit_t * u);
static void fill(void * first_pixel, int32_t w, int32_t h, int32_t stride, lv_color_t color, lv_color_format_t cf,
                 lv_opa_t opa, lv_draw_dma2d_unit_t * u);
static void configure_and_start_transfer(const lv_draw_dma2d_configuration_t * conf, lv_draw_dma2d_unit_t * u);
#if LV_USE_OS
    static void thread_cb(void * arg);
#endif
#if !LV_USE_DRAW_DMA2D_INTERRUPT
    static void await_transfer_completion(void);
#endif
static void post_transfer_tasks(lv_draw_dma2d_unit_t * u);
static void invalidate_cache(const lv_draw_dma2d_cache_area_t * mem_area);
static void clean_cache(const lv_draw_dma2d_cache_area_t * mem_area);

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OS || LV_USE_DRAW_DMA2D_INTERRUPT
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

#if LV_USE_OS || LV_USE_DRAW_DMA2D_INTERRUPT
    g_unit = draw_dma2d_unit;
#endif

#if LV_USE_OS
    g_unit = draw_dma2d_unit;

#if LV_USE_DRAW_DMA2D_INTERRUPT
    lv_result_t res = lv_thread_sync_init(&draw_dma2d_unit->interrupt_signal);
    LV_ASSERT(res == LV_RESULT_OK);

    res = lv_thread_init(&draw_dma2d_unit->thread, LV_THREAD_PRIO_HIGH, thread_cb, 2 * 1024, draw_dma2d_unit);
    LV_ASSERT(res == LV_RESULT_OK);
#else
    lv_result_t res = lv_thread_sync_init(&draw_dma2d_unit->thread_begin_polling_signal);
    LV_ASSERT(res == LV_RESULT_OK);

    res = lv_thread_init(&draw_dma2d_unit->thread, LV_THREAD_PRIO_LOW, thread_cb, 2 * 1024, draw_dma2d_unit);
    LV_ASSERT(res == LV_RESULT_OK);
#endif
#endif

    /* enable the DMA2D clock */
    RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;

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
    RCC->AHB3ENR &= ~RCC_AHB3ENR_DMA2DEN;

#if LV_USE_OS
    lv_result_t res = lv_thread_delete(&g_unit->thread);
    LV_ASSERT(res == LV_RESULT_OK);

#if LV_USE_DRAW_DMA2D_INTERRUPT
    res = lv_thread_sync_delete(&g_unit->interrupt_signal);
    LV_ASSERT(res == LV_RESULT_OK);
#else
    res = lv_thread_sync_delete(&g_unit->thread_begin_polling_signal);
    LV_ASSERT(res == LV_RESULT_OK);
#endif
#endif

#if LV_USE_OS || LV_USE_DRAW_DMA2D_INTERRUPT
    g_unit = NULL;
#endif
}

#if LV_USE_DRAW_DMA2D_INTERRUPT
void lv_draw_dma2d_transfer_complete_interrupt_handler(void)
{
#if LV_USE_OS
    lv_result_t res = lv_thread_sync_signal_isr(&u->interrupt_signal);
    LV_ASSERT(res == LV_RESULT_OK);
#else
    post_transfer_tasks(g_unit);
#endif
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
    lv_draw_task_t * t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_DMA2D);
    if(t == NULL) {
        return LV_DRAW_UNIT_IDLE;
    }

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) {
        return LV_DRAW_UNIT_IDLE;
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_dma2d_unit->base_unit.target_layer = layer;
    draw_dma2d_unit->base_unit.clip_area = &t->clip_area;
    draw_dma2d_unit->task_act = t;

    if(t->type == LV_DRAW_TASK_TYPE_FILL) {
        lv_draw_fill_dsc_t * dsc = t->draw_dsc;
        const lv_area_t * coords = &t->area;
        lv_area_t clipped_coords;
        lv_area_intersect(&clipped_coords, coords, draw_dma2d_unit->base_unit.clip_area);

        void * dest = lv_draw_layer_go_to_xy(layer,
                                             clipped_coords.x1 - layer->buf_area.x1,
                                             clipped_coords.y1 - layer->buf_area.y1);

        if(dsc->opa >= LV_OPA_MAX) {
            opaque_fill(dest,
                        lv_area_get_width(&clipped_coords),
                        lv_area_get_height(&clipped_coords),
                        lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format),
                        dsc->color,
                        dsc->base.layer->color_format,
                        draw_dma2d_unit);
        }
        else {
            fill(dest,
                 lv_area_get_width(&clipped_coords),
                 lv_area_get_height(&clipped_coords),
                 lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format),
                 dsc->color,
                 dsc->base.layer->color_format,
                 dsc->opa,
                 draw_dma2d_unit);
        }
    }

    return 1;
}

static int32_t delete_cb(lv_draw_unit_t * draw_unit)
{
    return 0;
}

static void opaque_fill(void * first_pixel, int32_t w, int32_t h, int32_t stride, lv_color_t color,
                        lv_color_format_t cf, lv_draw_dma2d_unit_t * u)
{
    lv_draw_dma2d_output_cf_t output_cf;
    uint32_t cf_size;
    uint32_t color_u32;
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            output_cf = LV_DRAW_DMA2D_OUTPUT_CF_ARGB8888;
            cf_size = 4;
            color_u32 = lv_color_to_u32(color);
            break;
        case LV_COLOR_FORMAT_RGB888:
            output_cf = LV_DRAW_DMA2D_OUTPUT_CF_RGB888;
            cf_size = 3;
            color_u32 = lv_color_to_u32(color);
            break;
        case LV_COLOR_FORMAT_RGB565:
            output_cf = LV_DRAW_DMA2D_OUTPUT_CF_RGB565;
            cf_size = 2;
            color_u32 = lv_color_to_u16(color);
            break;
        default:
            LV_LOG_ERROR("unsupported output color format");
            return;
    }

    lv_draw_dma2d_cache_area_t cache_area = {
        .first_byte = first_pixel,
        .width_bytes = w * cf_size,
        .height = h,
        .stride = stride
    };
    lv_memcpy(&u->writing_area, &cache_area, sizeof(lv_draw_dma2d_cache_area_t));

    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_REGISTER_TO_MEMORY,
        .w = w,
        .h = h,

        .output_address = first_pixel,
        .output_offset = (stride / cf_size) - w,
        .output_cf = output_cf,

        .reg_to_mem_mode_color = color_u32
    };
    configure_and_start_transfer(&conf, u);
}

static void fill(void * first_pixel, int32_t w, int32_t h, int32_t stride, lv_color_t color, lv_color_format_t cf,
                 lv_opa_t opa, lv_draw_dma2d_unit_t * u)
{
    lv_draw_dma2d_output_cf_t output_cf;
    uint32_t cf_size;
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            output_cf = LV_DRAW_DMA2D_OUTPUT_CF_ARGB8888;
            cf_size = 4;
            break;
        case LV_COLOR_FORMAT_RGB888:
            output_cf = LV_DRAW_DMA2D_OUTPUT_CF_RGB888;
            cf_size = 3;
            break;
        case LV_COLOR_FORMAT_RGB565:
            output_cf = LV_DRAW_DMA2D_OUTPUT_CF_RGB565;
            cf_size = 2;
            break;
        default:
            LV_LOG_ERROR("unsupported output color format");
            return;
    }

    lv_draw_dma2d_cache_area_t cache_area = {
        .first_byte = first_pixel,
        .width_bytes = w * cf_size,
        .height = h,
        .stride = stride
    };
    lv_memcpy(&u->writing_area, &cache_area, sizeof(lv_draw_dma2d_cache_area_t));

    /* make sure the background area DMA2D is blending is up-to-date in main memory */
    clean_cache(&cache_area);

    uint32_t output_offset = (stride / cf_size) - w;
    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIXED_COLOR_FG,
        .w = w,
        .h = h,

        .output_address = first_pixel,
        .output_offset = output_offset,
        .output_cf = output_cf,

        .fg_color = lv_color_to_u32(color),
        .fg_alpha_mode = 0x1, /* replace original fg alpha channel with fg_alpha */
        .fg_alpha = opa,

        .bg_address = first_pixel,
        .bg_offset = output_offset,
        .bg_cf = (lv_draw_dma2d_fgbg_cf_t) output_cf
    };
    configure_and_start_transfer(&conf, u);
}

static void configure_and_start_transfer(const lv_draw_dma2d_configuration_t * conf, lv_draw_dma2d_unit_t * u)
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

    /* foreground color. only for mem-to-mem with blending and fixed-color foreground */
    DMA2D->FGCOLR = conf->fg_color;
    /* foreground pixel format converter control register */
    DMA2D->FGPFCCR = (conf->fg_alpha << DMA2D_FGPFCCR_ALPHA_Pos) | (conf->fg_alpha_mode << DMA2D_FGPFCCR_AM_Pos);

    /* background */

    DMA2D->BGMAR = (uint32_t)(uintptr_t) conf->bg_address;
    DMA2D->BGOR = conf->bg_offset;
    DMA2D->BGPFCCR = ((uint32_t) conf->bg_cf) << DMA2D_BGPFCCR_CM_Pos;


    /* ensure the DMA2D register values are observed before the start transfer bit is set */
    __DSB();

    /* start the transfer (also set mode and enable transfer complete interrupt) */
    DMA2D->CR = DMA2D_CR_START | (((uint32_t) conf->mode) << DMA2D_CR_MODE_Pos)
#if LV_USE_DRAW_DMA2D_INTERRUPT
                | DMA2D_CR_TCIE
#endif
                ;

#if !LV_USE_DRAW_DMA2D_INTERRUPT
#if LV_USE_OS
    lv_result_t res = lv_thread_sync_signal(&u->thread_begin_polling_signal);
    LV_ASSERT(res == LV_RESULT_OK);
#else
    await_transfer_completion();
    post_transfer_tasks(u);
#endif
#endif
}

#if LV_USE_OS
static void thread_cb(void * arg)
{
    lv_draw_dma2d_unit_t * u = arg;

    while(1) {

#if LV_USE_DRAW_DMA2D_INTERRUPT
        lv_thread_sync_wait(&u->interrupt_signal);
#else
        lv_thread_sync_wait(&u->thread_begin_polling_signal);
        await_transfer_completion();
#endif

        post_transfer_tasks(u);
    }
}
#endif

#if !LV_USE_DRAW_DMA2D_INTERRUPT
static void await_transfer_completion(void)
{
    while(DMA2D->CR & DMA2D_CR_START);
}
#endif

static void post_transfer_tasks(lv_draw_dma2d_unit_t * u)
{
    invalidate_cache(&u->writing_area);
    u->task_act->state = LV_DRAW_TASK_STATE_READY;
    u->task_act = NULL;

    lv_draw_dispatch_request();
}

static void invalidate_cache(const lv_draw_dma2d_cache_area_t * mem_area)
{
    if((SCB->CCR & SCB_CCR_DC_Msk) == 0) return; /* data cache is disabled */

    uint32_t rows_remaining = mem_area->height;
    uint32_t row_addr = (uint32_t)(uintptr_t) mem_area->first_byte;
    uint32_t row_end_addr = 0;

    __DSB();

    while(rows_remaining) {
        uint32_t addr = row_addr & ~(__SCB_DCACHE_LINE_SIZE - 1U);
        uint32_t cache_lines = ((((row_addr + mem_area->width_bytes - 1) & ~(__SCB_DCACHE_LINE_SIZE - 1U)) - addr) /
                                __SCB_DCACHE_LINE_SIZE) + 1;

        if(addr == row_end_addr) {
            addr += __SCB_DCACHE_LINE_SIZE;
            cache_lines--;
        }

        while(cache_lines) {
            SCB->DCIMVAC = addr;
            addr += __SCB_DCACHE_LINE_SIZE;
            cache_lines--;
        }

        row_end_addr = addr - __SCB_DCACHE_LINE_SIZE;
        row_addr += mem_area->stride;
        rows_remaining--;
    };

    __DSB();
    __ISB();
}

static void clean_cache(const lv_draw_dma2d_cache_area_t * mem_area)
{
    if((SCB->CCR & SCB_CCR_DC_Msk) == 0) return;  /* data cache is disabled */

    uint32_t rows_remaining = mem_area->height;
    uint32_t row_addr = (uint32_t)(uintptr_t) mem_area->first_byte;
    uint32_t row_end_addr = 0;

    __DSB();

    while(rows_remaining) {
        uint32_t addr = row_addr & ~(__SCB_DCACHE_LINE_SIZE - 1U);
        uint32_t cache_lines = ((((row_addr + mem_area->width_bytes - 1) & ~(__SCB_DCACHE_LINE_SIZE - 1U)) - addr) /
                                __SCB_DCACHE_LINE_SIZE) + 1;

        if(addr == row_end_addr) {
            addr += __SCB_DCACHE_LINE_SIZE;
            cache_lines--;
        }

        while(cache_lines) {
            SCB->DCCMVAC = addr;
            addr += __SCB_DCACHE_LINE_SIZE;
            cache_lines--;
        }

        row_end_addr = addr - __SCB_DCACHE_LINE_SIZE;
        row_addr += mem_area->stride;
        rows_remaining--;
    };

    __DSB();
    __ISB();
}

#endif /*LV_USE_DRAW_DMA2D*/
