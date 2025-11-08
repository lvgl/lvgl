/**
 * @file lv_draw_sw_blur.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area_private.h"
#include "lv_draw_sw_mask_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_sw.h"

#if LV_USE_DRAW_SW

#include "../../misc/lv_math.h"
#include "../../misc/lv_types.h"
#include "../../core/lv_refr_private.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void /* LV_ATTRIBUTE_FAST_MEM */ draw_blur_hor(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc);
static void /* LV_ATTRIBUTE_FAST_MEM */ draw_blur_ver(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static inline void blur_init(uint32_t * sum, uint8_t * buf, uint32_t px_cnt)
{
    uint32_t i = 0;
    for(i = 0; i < px_cnt; i++) {
        sum[i] = buf[i] * 256;
    }
}

static inline void blur_px(uint32_t * sum, uint8_t * buf, uint32_t intensity, uint32_t px_cnt)
{
    uint32_t i = 0;
    for(i = 0; i < px_cnt; i++) {
        sum[i] = ((sum[i] * intensity) >> 8) + ((buf[i] * (256 - intensity)));
        buf[i] = sum[i] >> 8;
    }
}

#include <math.h>

static inline int32_t get_cir_p(int32_t p1, int32_t p2, int32_t p, int32_t r)
{
    if(p < p1 + r) p = r - (p - p1);
    else if(p > p2 - r) p = r - (p2 - p);
    else return 0;

    double dx = sqrt((double)r * r - (double)p * p);
    return r - (int32_t)(dx);
}

void lv_draw_sw_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->intensity == 0) return;
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) return;

    uint32_t intensity = dsc->intensity;

    int32_t radius = dsc->radius;
    uint32_t px_size = lv_color_format_get_size(t->target_layer->draw_buf->header.cf);
    int32_t y;
    int32_t x;

    for(y = clipped_coords.y1; y <= clipped_coords.y2; y++) {
        int32_t cir_x = get_cir_p(coords->y1, coords->y2, y, radius);
        int32_t x_start = coords->x1 + cir_x;
        int32_t x_end = coords->x2 - cir_x;
        if(x_start < clipped_coords.x1) x_start = clipped_coords.x1;
        if(x_end > clipped_coords.x2) x_end = clipped_coords.x2;

        uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
        uint8_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);

        uint32_t sum_start[4];
        uint32_t sum_end[4];
        blur_init(sum_start, buf_start, px_size);
        blur_init(sum_end, buf_end, px_size);

        for(x = x_start; x <= x_end; x++) {
            blur_px(sum_start, buf_start, intensity, px_size);
            blur_px(sum_end, buf_end, intensity, px_size);
            buf_start += 4;
            buf_end -= 4;
        }
    }

    int32_t stride = t->target_layer->draw_buf->header.stride;
    for(x = clipped_coords.x1; x <= clipped_coords.x2; x++) {
        int32_t cir_y = get_cir_p(coords->x1, coords->x2, x, radius);
        int32_t y_start = coords->y1 + cir_y;
        int32_t y_end = coords->y2 - cir_y;
        if(y_start < clipped_coords.y1) y_start = clipped_coords.y1;
        if(y_end > clipped_coords.y2) y_end = clipped_coords.y2;

        uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
        uint8_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);
        uint32_t sum_start[4];
        uint32_t sum_end[4];
        blur_init(sum_start, buf_start, px_size);
        blur_init(sum_end, buf_end, px_size);

        for(y = y_start; y <= y_end; y++) {
            blur_px(sum_start, buf_start, intensity, px_size);
            blur_px(sum_end, buf_end, intensity, px_size);
            buf_start += stride;
            buf_end -= stride;
        }
    }

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif /*LV_USE_DRAW_SW*/
