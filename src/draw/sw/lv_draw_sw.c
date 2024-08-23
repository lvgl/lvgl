/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_private.h"
#include "../lv_draw_private.h"
#if LV_USE_DRAW_SW

#include "../../core/lv_refr.h"
#include "../../display/lv_display_private.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    #if LV_USE_THORVG_EXTERNAL
        #include <thorvg_capi.h>
    #else
        #include "../../libs/thorvg/thorvg_capi.h"
    #endif
#endif

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "arm2d/lv_draw_sw_helium.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
    #include LV_DRAW_SW_ASM_CUSTOM_INCLUDE
#endif

#if LV_DRAW_SW_DRAW_UNIT_CNT > 1 && LV_USE_OS == LV_OS_NONE
    #error "OS support is required when more than one SW rendering units are enabled"
#endif

/*********************
 *      DEFINES
 *********************/
#define DRAW_UNIT_ID_SW     1

#ifndef LV_DRAW_SW_RGB565_SWAP
    #define LV_DRAW_SW_RGB565_SWAP(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE90_ARGB8888
    #define LV_DRAW_SW_ROTATE90_ARGB8888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE180_ARGB8888
    #define LV_DRAW_SW_ROTATE180_ARGB8888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE270_ARGB8888
    #define LV_DRAW_SW_ROTATE270_ARGB8888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE90_RGB888
    #define LV_DRAW_SW_ROTATE90_RGB888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE180_RGB888
    #define LV_DRAW_SW_ROTATE180_RGB888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE270_RGB888
    #define LV_DRAW_SW_ROTATE270_RGB888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE90_RGB565
    #define LV_DRAW_SW_ROTATE90_RGB565(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE180_RGB565
    #define LV_DRAW_SW_ROTATE180_RGB565(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE270_RGB565
    #define LV_DRAW_SW_ROTATE270_RGB565(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE90_L8
    #define LV_DRAW_SW_ROTATE90_L8(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE180_L8
    #define LV_DRAW_SW_ROTATE180_L8(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE270_L8
    #define LV_DRAW_SW_ROTATE270_L8(...) LV_RESULT_INVALID
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_USE_OS
    static void render_thread_cb(void * ptr);
#endif

static void execute_drawing(lv_draw_sw_unit_t * u);

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t lv_draw_sw_delete(lv_draw_unit_t * draw_unit);

#if LV_DRAW_SW_SUPPORT_ARGB8888
static void rotate90_argb8888(const uint32_t * src, uint32_t * dst, int32_t src_width, int32_t src_height,
                              int32_t src_stride,
                              int32_t dst_stride);
static void rotate180_argb8888(const uint32_t * src, uint32_t * dst, int32_t width, int32_t height, int32_t src_stride,
                               int32_t dest_stride);
static void rotate270_argb8888(const uint32_t * src, uint32_t * dst, int32_t src_width, int32_t src_height,
                               int32_t src_stride,
                               int32_t dst_stride);
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
static void rotate90_rgb888(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                            int32_t src_stride,
                            int32_t dst_stride);
static void rotate180_rgb888(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dest_stride);
static void rotate270_rgb888(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dst_stride);
#endif
#if LV_DRAW_SW_SUPPORT_RGB565
static void rotate90_rgb565(const uint16_t * src, uint16_t * dst, int32_t src_width, int32_t src_height,
                            int32_t src_stride,
                            int32_t dst_stride);
static void rotate180_rgb565(const uint16_t * src, uint16_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dest_stride);
static void rotate270_rgb565(const uint16_t * src, uint16_t * dst, int32_t src_width, int32_t src_height,
                             int32_t src_stride,
                             int32_t dst_stride);
#endif

#if LV_DRAW_SW_SUPPORT_L8

static void rotate90_l8(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                        int32_t src_stride,
                        int32_t dst_stride);
static void rotate180_l8(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                         int32_t dest_stride);
static void rotate270_l8(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                         int32_t src_stride,
                         int32_t dst_stride);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_init(void)
{

#if LV_DRAW_SW_COMPLEX == 1
    lv_draw_sw_mask_init();
#endif

    uint32_t i;
    for(i = 0; i < LV_DRAW_SW_DRAW_UNIT_CNT; i++) {
        lv_draw_sw_unit_t * draw_sw_unit = lv_draw_create_unit(sizeof(lv_draw_sw_unit_t));
        draw_sw_unit->base_unit.dispatch_cb = dispatch;
        draw_sw_unit->base_unit.evaluate_cb = evaluate;
        draw_sw_unit->idx = i;
        draw_sw_unit->base_unit.delete_cb = LV_USE_OS ? lv_draw_sw_delete : NULL;

#if LV_USE_OS
        lv_thread_init(&draw_sw_unit->thread, LV_THREAD_PRIO_HIGH, render_thread_cb, LV_DRAW_THREAD_STACK_SIZE, draw_sw_unit);
#endif
    }

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    tvg_engine_init(TVG_ENGINE_SW, 0);
#endif
}

void lv_draw_sw_deinit(void)
{
#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    tvg_engine_term(TVG_ENGINE_SW);
#endif

#if LV_DRAW_SW_COMPLEX == 1
    lv_draw_sw_mask_deinit();
#endif
}

static int32_t lv_draw_sw_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_OS
    lv_draw_sw_unit_t * draw_sw_unit = (lv_draw_sw_unit_t *) draw_unit;

    LV_LOG_INFO("cancel software rendering thread");
    draw_sw_unit->exit_status = true;

    if(draw_sw_unit->inited) {
        lv_thread_sync_signal(&draw_sw_unit->sync);
    }

    return lv_thread_delete(&draw_sw_unit->thread);
#else
    LV_UNUSED(draw_unit);
    return 0;
#endif
}

void lv_draw_sw_rgb565_swap(void * buf, uint32_t buf_size_px)
{
    if(LV_DRAW_SW_RGB565_SWAP(buf, buf_size_px) == LV_RESULT_OK) return;

    uint32_t u32_cnt = buf_size_px / 2;
    uint16_t * buf16 = buf;
    uint32_t * buf32 = buf;

    while(u32_cnt >= 8) {
        buf32[0] = ((buf32[0] & 0xff00ff00) >> 8) | ((buf32[0] & 0x00ff00ff) << 8);
        buf32[1] = ((buf32[1] & 0xff00ff00) >> 8) | ((buf32[1] & 0x00ff00ff) << 8);
        buf32[2] = ((buf32[2] & 0xff00ff00) >> 8) | ((buf32[2] & 0x00ff00ff) << 8);
        buf32[3] = ((buf32[3] & 0xff00ff00) >> 8) | ((buf32[3] & 0x00ff00ff) << 8);
        buf32[4] = ((buf32[4] & 0xff00ff00) >> 8) | ((buf32[4] & 0x00ff00ff) << 8);
        buf32[5] = ((buf32[5] & 0xff00ff00) >> 8) | ((buf32[5] & 0x00ff00ff) << 8);
        buf32[6] = ((buf32[6] & 0xff00ff00) >> 8) | ((buf32[6] & 0x00ff00ff) << 8);
        buf32[7] = ((buf32[7] & 0xff00ff00) >> 8) | ((buf32[7] & 0x00ff00ff) << 8);
        buf32 += 8;
        u32_cnt -= 8;
    }

    while(u32_cnt) {
        *buf32 = ((*buf32 & 0xff00ff00) >> 8) | ((*buf32 & 0x00ff00ff) << 8);
        buf32++;
        u32_cnt--;
    }

    if(buf_size_px & 0x1) {
        uint32_t e = buf_size_px - 1;
        buf16[e] = ((buf16[e] & 0xff00) >> 8) | ((buf16[e] & 0x00ff) << 8);
    }

}

void lv_draw_sw_i1_invert(void * buf, uint32_t buf_size)
{
    if(buf == NULL) return;

    uint8_t * byte_buf = (uint8_t *)buf;
    uint32_t i;

    /*Make the buffer aligned*/
    while(((uintptr_t)(byte_buf) & (sizeof(int) - 1)) && buf_size > 0) {
        *byte_buf = ~(*byte_buf);
        byte_buf++;
        buf_size--;
    }

    if(buf_size >= sizeof(uint32_t)) {
        uint32_t * aligned_addr = (uint32_t *)byte_buf;
        uint32_t word_count = buf_size / 4;

        for(i = 0; i < word_count; ++i) {
            aligned_addr[i] = ~aligned_addr[i];
        }

        byte_buf = (uint8_t *)(aligned_addr + word_count);
        buf_size = buf_size % sizeof(uint32_t);
    }

    for(i = 0; i < buf_size; ++i) {
        byte_buf[i] = ~byte_buf[i];
    }
}

void lv_draw_sw_rotate(const void * src, void * dest, int32_t src_width, int32_t src_height, int32_t src_stride,
                       int32_t dest_stride, lv_display_rotation_t rotation, lv_color_format_t color_format)
{
    if(rotation == LV_DISPLAY_ROTATION_90) {
        switch(color_format) {
#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                rotate90_l8(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565
            case LV_COLOR_FORMAT_RGB565:
                rotate90_rgb565(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                rotate90_rgb888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
            case LV_COLOR_FORMAT_ARGB8888:
                rotate90_argb8888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
            default:
                break;
        }

        return;
    }

    if(rotation == LV_DISPLAY_ROTATION_180) {
        switch(color_format) {
#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                rotate180_l8(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565
            case LV_COLOR_FORMAT_RGB565:
                rotate180_rgb565(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                rotate180_rgb888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
            case LV_COLOR_FORMAT_ARGB8888:
                rotate180_argb8888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
            default:
                break;
        }

        return;
    }

    if(rotation == LV_DISPLAY_ROTATION_270) {
        switch(color_format) {
#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                rotate270_l8(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565
            case LV_COLOR_FORMAT_RGB565:
                rotate270_rgb565(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                rotate270_rgb888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
            case LV_COLOR_FORMAT_ARGB8888:
                rotate270_argb8888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
            default:
                break;
        }

        return;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static inline void execute_drawing_unit(lv_draw_sw_unit_t * u)
{
    execute_drawing(u);

    u->task_act->state = LV_DRAW_TASK_STATE_READY;
    u->task_act = NULL;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();
}

static int32_t evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);

    switch(task->type) {
        case LV_DRAW_TASK_TYPE_IMAGE:
        case LV_DRAW_TASK_TYPE_LAYER: {
                lv_draw_image_dsc_t * draw_dsc = task->draw_dsc;

                /* not support skew */
                if(draw_dsc->skew_x != 0 || draw_dsc->skew_y != 0) {
                    return 0;
                }

                bool transformed = draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                                   draw_dsc->scale_y != LV_SCALE_NONE ? true : false;

                bool masked = draw_dsc->bitmap_mask_src != NULL;
                if(masked && transformed)  return 0;

                lv_color_format_t cf = draw_dsc->header.cf;
                if(masked && (cf == LV_COLOR_FORMAT_A8 || cf == LV_COLOR_FORMAT_RGB565A8)) {
                    return 0;
                }
            }
            break;
        default:
            break;
    }

    if(task->preference_score >= 100) {
        task->preference_score = 100;
        task->preferred_draw_unit_id = DRAW_UNIT_ID_SW;
    }

    return 0;
}

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    LV_PROFILER_BEGIN;
    lv_draw_sw_unit_t * draw_sw_unit = (lv_draw_sw_unit_t *) draw_unit;

    /*Return immediately if it's busy with draw task*/
    if(draw_sw_unit->task_act) {
        LV_PROFILER_END;
        return 0;
    }

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_SW);
    if(t == NULL) {
        LV_PROFILER_END;
        return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) {
        LV_PROFILER_END;
        return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_sw_unit->base_unit.target_layer = layer;
    draw_sw_unit->base_unit.clip_area = &t->clip_area;
    draw_sw_unit->task_act = t;

#if LV_USE_OS
    /*Let the render thread work*/
    if(draw_sw_unit->inited) lv_thread_sync_signal(&draw_sw_unit->sync);
#else
    execute_drawing_unit(draw_sw_unit);
#endif
    LV_PROFILER_END;
    return 1;
}

#if LV_USE_OS
static void render_thread_cb(void * ptr)
{
    lv_draw_sw_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);
    u->inited = true;

    while(1) {
        while(u->task_act == NULL) {
            if(u->exit_status) {
                break;
            }
            lv_thread_sync_wait(&u->sync);
        }

        if(u->exit_status) {
            LV_LOG_INFO("ready to exit software rendering thread");
            break;
        }

        execute_drawing_unit(u);
    }

    u->inited = false;
    lv_thread_sync_delete(&u->sync);
    LV_LOG_INFO("exit software rendering thread");
}
#endif

static void execute_drawing(lv_draw_sw_unit_t * u)
{
    LV_PROFILER_BEGIN;
    /*Render the draw task*/
    lv_draw_task_t * t = u->task_act;
    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_sw_fill((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_sw_border((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
            lv_draw_sw_box_shadow((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_sw_label((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_sw_image((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_sw_arc((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_sw_line((lv_draw_unit_t *)u, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_sw_triangle((lv_draw_unit_t *)u, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_sw_layer((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
            lv_draw_sw_mask_rect((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
        case LV_DRAW_TASK_TYPE_VECTOR:
            lv_draw_sw_vector((lv_draw_unit_t *)u, t->draw_dsc);
            break;
#endif
        default:
            break;
    }

#if LV_USE_PARALLEL_DRAW_DEBUG
    /*Layers manage it for themselves*/
    if(t->type != LV_DRAW_TASK_TYPE_LAYER) {
        lv_area_t draw_area;
        if(!lv_area_intersect(&draw_area, &t->area, u->base_unit.clip_area)) return;

        int32_t idx = 0;
        lv_draw_unit_t * draw_unit_tmp = _draw_info.unit_head;
        while(draw_unit_tmp != (lv_draw_unit_t *)u) {
            draw_unit_tmp = draw_unit_tmp->next;
            idx++;
        }
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_palette_main(idx % LV_PALETTE_LAST);
        rect_dsc.border_color = rect_dsc.bg_color;
        rect_dsc.bg_opa = LV_OPA_10;
        rect_dsc.border_opa = LV_OPA_80;
        rect_dsc.border_width = 1;
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &draw_area);

        lv_point_t txt_size;
        lv_text_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

        lv_area_t txt_area;
        txt_area.x1 = draw_area.x1;
        txt_area.y1 = draw_area.y1;
        txt_area.x2 = draw_area.x1 + txt_size.x - 1;
        txt_area.y2 = draw_area.y1 + txt_size.y - 1;

        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_color_white();
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &txt_area);

        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d", idx);
        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_black();
        label_dsc.text = buf;
        lv_draw_sw_label((lv_draw_unit_t *)u, &label_dsc, &txt_area);
    }
#endif
    LV_PROFILER_END;
}

#if LV_DRAW_SW_SUPPORT_ARGB8888

static void rotate270_argb8888(const uint32_t * src, uint32_t * dst, int32_t src_width, int32_t src_height,
                               int32_t src_stride,
                               int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE90_ARGB8888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint32_t);
    dst_stride /= sizeof(uint32_t);

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = x * dst_stride;
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex + (src_height - y - 1)] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

static void rotate180_argb8888(const uint32_t * src, uint32_t * dst, int32_t width, int32_t height, int32_t src_stride,
                               int32_t dest_stride)
{
    LV_UNUSED(dest_stride);
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE180_ARGB8888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint32_t);

    for(int32_t y = 0; y < height; ++y) {
        int32_t dstIndex = (height - y - 1) * src_stride;
        int32_t srcIndex = y * src_stride;
        for(int32_t x = 0; x < width; ++x) {
            dst[dstIndex + width - x - 1] = src[srcIndex + x];
        }
    }
}

static void rotate90_argb8888(const uint32_t * src, uint32_t * dst, int32_t src_width, int32_t src_height,
                              int32_t src_stride, int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE270_ARGB8888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint32_t);
    dst_stride /= sizeof(uint32_t);

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = (src_width - x - 1);
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex * dst_stride + y] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_RGB888

static void rotate90_rgb888(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                            int32_t src_stride,
                            int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE90_RGB888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t x = 0; x < src_width; ++x) {
        for(int32_t y = 0; y < src_height; ++y) {
            int32_t srcIndex = y * src_stride + x * 3;
            int32_t dstIndex = (src_width - x - 1) * dst_stride + y * 3;
            dst[dstIndex] = src[srcIndex];       /*Red*/
            dst[dstIndex + 1] = src[srcIndex + 1]; /*Green*/
            dst[dstIndex + 2] = src[srcIndex + 2]; /*Blue*/
        }
    }
}

static void rotate180_rgb888(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dest_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE180_RGB888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t y = 0; y < height; ++y) {
        for(int32_t x = 0; x < width; ++x) {
            int32_t srcIndex = y * src_stride + x * 3;
            int32_t dstIndex = (height - y - 1) * dest_stride + (width - x - 1) * 3;
            dst[dstIndex] = src[srcIndex];
            dst[dstIndex + 1] = src[srcIndex + 1];
            dst[dstIndex + 2] = src[srcIndex + 2];
        }
    }
}

static void rotate270_rgb888(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE270_RGB888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t x = 0; x < width; ++x) {
        for(int32_t y = 0; y < height; ++y) {
            int32_t srcIndex = y * src_stride + x * 3;
            int32_t dstIndex = x * dst_stride + (height - y - 1) * 3;
            dst[dstIndex] = src[srcIndex];       /*Red*/
            dst[dstIndex + 1] = src[srcIndex + 1]; /*Green*/
            dst[dstIndex + 2] = src[srcIndex + 2]; /*Blue*/
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_RGB565

static void rotate270_rgb565(const uint16_t * src, uint16_t * dst, int32_t src_width, int32_t src_height,
                             int32_t src_stride,
                             int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE90_RGB565(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint16_t);
    dst_stride /= sizeof(uint16_t);

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = x * dst_stride;
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex + (src_height - y - 1)] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

static void rotate180_rgb565(const uint16_t * src, uint16_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dest_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE180_RGB565(src, dst, width, height, src_stride)) {
        return ;
    }

    src_stride /= sizeof(uint16_t);
    dest_stride /= sizeof(uint16_t);

    for(int32_t y = 0; y < height; ++y) {
        int32_t dstIndex = (height - y - 1) * dest_stride;
        int32_t srcIndex = y * src_stride;
        for(int32_t x = 0; x < width; ++x) {
            dst[dstIndex + width - x - 1] = src[srcIndex + x];
        }
    }
}

static void rotate90_rgb565(const uint16_t * src, uint16_t * dst, int32_t src_width, int32_t src_height,
                            int32_t src_stride,
                            int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE270_RGB565(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint16_t);
    dst_stride /= sizeof(uint16_t);

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = (src_width - x - 1);
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex * dst_stride + y] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

#endif


#if LV_DRAW_SW_SUPPORT_L8

static void rotate90_l8(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                        int32_t src_stride,
                        int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE270_L8(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = (src_width - x - 1);
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex * dst_stride + y] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

static void rotate180_l8(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                         int32_t dest_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE180_L8(src, dst, width, height, src_stride)) {
        return ;
    }

    for(int32_t y = 0; y < height; ++y) {
        int32_t dstIndex = (height - y - 1) * dest_stride;
        int32_t srcIndex = y * src_stride;
        for(int32_t x = 0; x < width; ++x) {
            dst[dstIndex + width - x - 1] = src[srcIndex + x];
        }
    }
}

static void rotate270_l8(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                         int32_t src_stride,
                         int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE90_L8(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = x * dst_stride;
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex + (src_height - y - 1)] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

#endif

#endif /*LV_USE_DRAW_SW*/
