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

#include LV_DRAW_DMA2D_HAL_INCLUDE

#if !LV_DRAW_DMA2D_ASYNC && LV_USE_DRAW_DMA2D_INTERRUPT
    #warning LV_USE_DRAW_DMA2D_INTERRUPT is 1 but has no effect because LV_USE_OS is LV_OS_NONE
#endif

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_DMA2D 5

#if LV_USE_DRAW_DMA2D_INTERRUPT && LV_USE_OS
    #define LV_DRAW_DMA2D_ASYNC 1
#else
    #define LV_DRAW_DMA2D_ASYNC 0
#endif

#if defined(__CORTEX_M) && (__CORTEX_M == 7)
    #define LV_DRAW_DMA2D_CACHE 1
#else
    #define LV_DRAW_DMA2D_CACHE 0
#endif

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

typedef enum {
    LV_DRAW_DMA2D_ALPHA_MODE_NO_MODIFY_IMAGE_ALPHA_CHANNEL = 0,
    LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL,
    LV_DRAW_DMA2D_ALPHA_MODE_MULTIPLY_IMAGE_ALPHA_CHANNEL
} lv_draw_dma2d_alpha_mode_t;

typedef struct {
    lv_draw_dma2d_mode_t mode;
    uint32_t w;
    uint32_t h;

    void * output_address;
    uint32_t output_offset;
    lv_draw_dma2d_output_cf_t output_cf;

    uint32_t reg_to_mem_mode_color;

    const void * fg_address;
    uint32_t fg_offset;
    lv_draw_dma2d_fgbg_cf_t fg_cf;
    uint32_t fg_color;
    uint32_t fg_alpha_mode;
    uint32_t fg_alpha;

    const void * bg_address;
    uint32_t bg_offset;
    lv_draw_dma2d_fgbg_cf_t bg_cf;
    uint32_t bg_color;
    uint32_t bg_alpha_mode;
    uint32_t bg_alpha;

} lv_draw_dma2d_configuration_t;

typedef struct {
    const void * first_byte;
    uint32_t width_bytes;
    uint32_t height;
    uint32_t stride;
} lv_draw_dma2d_cache_area_t;

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * volatile task_act;
#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t writing_area;
#endif
#if LV_DRAW_DMA2D_ASYNC
    lv_thread_t thread;
    lv_thread_sync_t interrupt_signal;
#endif
} lv_draw_dma2d_unit_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t evaluate_cb(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t dispatch_cb(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t delete_cb(lv_draw_unit_t * draw_unit);
static lv_draw_dma2d_output_cf_t cf_to_dma2d_output_cf(lv_color_format_t cf);
static uint32_t color_to_dma2d_ocolr(lv_draw_dma2d_output_cf_t cf, lv_color_t color);
static void draw_opaque_fill(lv_draw_dma2d_unit_t * u, void * first_pixel, int32_t w, int32_t h, int32_t stride);
static void draw_fill(lv_draw_dma2d_unit_t * u, void * first_pixel, int32_t w, int32_t h, int32_t stride);
static void draw_opaque_image(lv_draw_dma2d_unit_t * u, void * dest_first_pixel, lv_area_t * clipped_coords,
                              int32_t dest_stride);
static void draw_image(lv_draw_dma2d_unit_t * u, void * dest_first_pixel, lv_area_t * clipped_coords,
                       int32_t dest_stride);
static void configure_and_start_transfer(const lv_draw_dma2d_configuration_t * conf);
#if LV_DRAW_DMA2D_ASYNC
    static void thread_cb(void * arg);
#endif
#if !LV_DRAW_DMA2D_ASYNC
    static bool check_transfer_completion(void);
#endif
static void post_transfer_tasks(lv_draw_dma2d_unit_t * u);
#if LV_DRAW_DMA2D_CACHE
    static void invalidate_cache(const lv_draw_dma2d_cache_area_t * mem_area);
    static void clean_cache(const lv_draw_dma2d_cache_area_t * mem_area);
#endif

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

#if LV_DRAW_DMA2D_ASYNC
    g_unit = draw_dma2d_unit;

    res = lv_thread_init(&draw_dma2d_unit->thread, LV_THREAD_PRIO_HIGH, thread_cb, 2 * 1024, draw_dma2d_unit);
    LV_ASSERT(res == LV_RESULT_OK);
#endif

    /* enable the DMA2D clock */
#if defined(STM32F4) || defined(STM32F7) || defined(STM32U5)
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;
#elif defined(STM32H7)
    RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;
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
                if(!(dsc->clip_radius == 0
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
                         || dsc->header.cf == LV_COLOR_FORMAT_RGB565)
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
            return -1;
        }
        post_transfer_tasks(draw_dma2d_unit);
#endif
    }

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
        if(!lv_area_intersect(&clipped_coords, coords, draw_dma2d_unit->base_unit.clip_area)) {
            return LV_DRAW_UNIT_IDLE;
        }

        void * dest = lv_draw_layer_go_to_xy(layer,
                                             clipped_coords.x1 - layer->buf_area.x1,
                                             clipped_coords.y1 - layer->buf_area.y1);

        if(dsc->opa >= LV_OPA_MAX) {
            draw_opaque_fill(draw_dma2d_unit,
                             dest,
                             lv_area_get_width(&clipped_coords),
                             lv_area_get_height(&clipped_coords),
                             lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format));
        }
        else {
            draw_fill(draw_dma2d_unit,
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
        if(!lv_area_intersect(&clipped_coords, coords, draw_dma2d_unit->base_unit.clip_area)) {
            return LV_DRAW_UNIT_IDLE;
        }

        void * dest = lv_draw_layer_go_to_xy(layer,
                                             clipped_coords.x1 - layer->buf_area.x1,
                                             clipped_coords.y1 - layer->buf_area.y1);

        if(dsc->opa >= LV_OPA_MAX) {
            draw_opaque_image(
                draw_dma2d_unit,
                dest,
                &clipped_coords,
                lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format));
        }
        else {
            draw_image(
                draw_dma2d_unit,
                dest,
                &clipped_coords,
                lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format));
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

static lv_draw_dma2d_output_cf_t cf_to_dma2d_output_cf(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            return LV_DRAW_DMA2D_OUTPUT_CF_ARGB8888;
        case LV_COLOR_FORMAT_RGB888:
            return LV_DRAW_DMA2D_OUTPUT_CF_RGB888;
        case LV_COLOR_FORMAT_RGB565:
            return LV_DRAW_DMA2D_OUTPUT_CF_RGB565;
        default:
            LV_ASSERT_MSG(false, "unsupported output color format");
    }
}

static uint32_t color_to_dma2d_ocolr(lv_draw_dma2d_output_cf_t cf, lv_color_t color)
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
}

static void draw_opaque_fill(lv_draw_dma2d_unit_t * u, void * first_pixel, int32_t w, int32_t h, int32_t stride)
{
    lv_draw_fill_dsc_t * dsc = u->task_act->draw_dsc;
    lv_color_format_t cf = dsc->base.layer->color_format;

    lv_draw_dma2d_output_cf_t output_cf = cf_to_dma2d_output_cf(cf);
    uint32_t cf_size = LV_COLOR_FORMAT_GET_SIZE(cf);
    uint32_t reg_to_mem_color = color_to_dma2d_ocolr(output_cf, dsc->color);

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t cache_area = {
        .first_byte = first_pixel,
        .width_bytes = w * cf_size,
        .height = h,
        .stride = stride
    };
    lv_memcpy(&u->writing_area, &cache_area, sizeof(lv_draw_dma2d_cache_area_t));
#endif

    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_REGISTER_TO_MEMORY,
        .w = w,
        .h = h,

        .output_address = first_pixel,
        .output_offset = (stride / cf_size) - w,
        .output_cf = output_cf,

        .reg_to_mem_mode_color = reg_to_mem_color
    };
    configure_and_start_transfer(&conf);
}

static void draw_fill(lv_draw_dma2d_unit_t * u, void * first_pixel, int32_t w, int32_t h, int32_t stride)
{
    lv_draw_fill_dsc_t * dsc = u->task_act->draw_dsc;
    lv_color_t color = dsc->color;
    lv_color_format_t cf = dsc->base.layer->color_format;
    lv_opa_t opa = dsc->opa;

    lv_draw_dma2d_output_cf_t output_cf = cf_to_dma2d_output_cf(cf);
    uint32_t cf_size = LV_COLOR_FORMAT_GET_SIZE(cf);

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t cache_area = {
        .first_byte = first_pixel,
        .width_bytes = w * cf_size,
        .height = h,
        .stride = stride
    };
    lv_memcpy(&u->writing_area, &cache_area, sizeof(lv_draw_dma2d_cache_area_t));

    /* make sure the background area DMA2D is blending is up-to-date in main memory */
    clean_cache(&cache_area);
#endif

    uint32_t output_offset = (stride / cf_size) - w;
    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIXED_COLOR_FG,
        .w = w,
        .h = h,

        .output_address = first_pixel,
        .output_offset = output_offset,
        .output_cf = output_cf,

        .fg_color = lv_color_to_u32(color),
        .fg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL,
        .fg_alpha = opa,

        .bg_address = first_pixel,
        .bg_offset = output_offset,
        .bg_cf = (lv_draw_dma2d_fgbg_cf_t) output_cf
    };
    configure_and_start_transfer(&conf);
}

static void draw_opaque_image(lv_draw_dma2d_unit_t * u, void * dest_first_pixel, lv_area_t * clipped_coords,
                              int32_t dest_stride)
{
    int32_t w = lv_area_get_width(clipped_coords);
    int32_t h = lv_area_get_height(clipped_coords);

    lv_draw_image_dsc_t * dsc = u->task_act->draw_dsc;
    lv_color_format_t output_cf = dsc->base.layer->color_format;
    lv_color_format_t image_cf = dsc->header.cf;

    lv_draw_dma2d_output_cf_t output_cf_dma2d = cf_to_dma2d_output_cf(output_cf);
    uint32_t output_cf_size = LV_COLOR_FORMAT_GET_SIZE(output_cf);

    lv_draw_dma2d_fgbg_cf_t image_cf_dma2d = (lv_draw_dma2d_fgbg_cf_t) cf_to_dma2d_output_cf(image_cf);
    uint32_t image_cf_size = LV_COLOR_FORMAT_GET_SIZE(image_cf);

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t dest_area = {
        .first_byte = dest_first_pixel,
        .width_bytes = w * output_cf_size,
        .height = h,
        .stride = dest_stride
    };
    lv_memcpy(&u->writing_area, &dest_area, sizeof(lv_draw_dma2d_cache_area_t));
    if(lv_color_format_has_alpha(image_cf)) {
        /* make sure the background area DMA2D is blending is up-to-date in main memory */
        clean_cache(&dest_area);
    }
#endif

    const lv_image_dsc_t * img_dsc = dsc->src;

    const void * image_first_byte = img_dsc->data
                                    + (img_dsc->header.stride * (clipped_coords->y1 - dsc->image_area.y1))
                                    + (image_cf_size * (clipped_coords->x1 - dsc->image_area.x1));

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t src_area = {
        .first_byte = image_first_byte,
        .width_bytes = w * image_cf_size,
        .height = h,
        .stride = dsc->header.stride
    };
    /* make sure the image area is up-to-date in main memory for DMA2D */
    clean_cache(&src_area);
#endif

    uint32_t output_offset = (dest_stride / output_cf_size) - w;
    lv_draw_dma2d_configuration_t conf = {
        .w = w,
        .h = h,

        .output_address = dest_first_pixel,
        .output_offset = output_offset,
        .output_cf = output_cf_dma2d,

        .fg_address = image_first_byte,
        .fg_offset = (dsc->header.stride / image_cf_size) - w,
        .fg_cf = image_cf_dma2d
    };
    if(lv_color_format_has_alpha(image_cf)) {
        /* only process the background if the image might be transparent */
        conf.mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING;

        conf.bg_address = dest_first_pixel;
        conf.bg_offset = output_offset;
        conf.bg_cf = output_cf_dma2d;
    }
    else {
        conf.mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_PFC;
    }
    configure_and_start_transfer(&conf);
}

static void draw_image(lv_draw_dma2d_unit_t * u, void * dest_first_pixel, lv_area_t * clipped_coords,
                       int32_t dest_stride)
{
    int32_t w = lv_area_get_width(clipped_coords);
    int32_t h = lv_area_get_height(clipped_coords);

    lv_draw_image_dsc_t * dsc = u->task_act->draw_dsc;
    lv_color_format_t output_cf = dsc->base.layer->color_format;
    lv_color_format_t image_cf = dsc->header.cf;
    lv_opa_t opa = dsc->opa;

    lv_draw_dma2d_output_cf_t output_cf_dma2d = cf_to_dma2d_output_cf(output_cf);
    uint32_t output_cf_size = LV_COLOR_FORMAT_GET_SIZE(output_cf);

    lv_draw_dma2d_fgbg_cf_t image_cf_dma2d = (lv_draw_dma2d_fgbg_cf_t) cf_to_dma2d_output_cf(image_cf);
    uint32_t image_cf_size = LV_COLOR_FORMAT_GET_SIZE(image_cf);

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t dest_area = {
        .first_byte = dest_first_pixel,
        .width_bytes = w * output_cf_size,
        .height = h,
        .stride = dest_stride
    };
    lv_memcpy(&u->writing_area, &dest_area, sizeof(lv_draw_dma2d_cache_area_t));
    /* make sure the background area DMA2D is blending is up-to-date in main memory */
    clean_cache(&dest_area);
#endif

    const lv_image_dsc_t * img_dsc = dsc->src;

    const void * image_first_byte = img_dsc->data
                                    + (img_dsc->header.stride * (clipped_coords->y1 - dsc->image_area.y1))
                                    + (image_cf_size * (clipped_coords->x1 - dsc->image_area.x1));

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t src_area = {
        .first_byte = image_first_byte,
        .width_bytes = w * image_cf_size,
        .height = h,
        .stride = dsc->header.stride
    };
    /* make sure the image area is up-to-date in main memory for DMA2D */
    clean_cache(&src_area);
#endif

    uint32_t output_offset = (dest_stride / output_cf_size) - w;
    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING,
        .w = w,
        .h = h,

        .output_address = dest_first_pixel,
        .output_offset = output_offset,
        .output_cf = output_cf_dma2d,

        .fg_address = image_first_byte,
        .fg_offset = (dsc->header.stride / image_cf_size) - w,
        .fg_cf = (lv_draw_dma2d_fgbg_cf_t) image_cf_dma2d,
        .fg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_MULTIPLY_IMAGE_ALPHA_CHANNEL,
        .fg_alpha = opa,

        .bg_address = dest_first_pixel,
        .bg_offset = output_offset,
        .bg_cf = output_cf_dma2d,
    };
    configure_and_start_transfer(&conf);
}

static void configure_and_start_transfer(const lv_draw_dma2d_configuration_t * conf)
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
    invalidate_cache(&u->writing_area);
#endif
    u->task_act->state = LV_DRAW_TASK_STATE_READY;
    u->task_act = NULL;
}

#if LV_DRAW_DMA2D_CACHE
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
#endif

#endif /*LV_USE_DRAW_DMA2D*/
