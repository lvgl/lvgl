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
#define BLUR_SAMPLE_SIZE 8
#define BLUR_MULTIPLIER_BITS 12
#define BLUR_MULTIPLIER_MAX (1 << 12)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static inline void blur_3px_init(uint32_t * sum, uint8_t * buf, uint32_t px_cnt, uint32_t len, int32_t stride)
{

    uint32_t sample_cnt = LV_MIN(BLUR_SAMPLE_SIZE, len);
    uint8_t * buf_ori = buf;

    uint32_t i = 0;
    uint32_t s;

    sum[i] = 0;
    buf = buf_ori;
    for(s = 0; s < sample_cnt; s++) {
        sum[i] += buf[i];
        buf += stride;
    }
    sum[i] = (sum[i] << BLUR_MULTIPLIER_BITS) / sample_cnt;

    i++;
    sum[i] = 0;
    buf = buf_ori;
    for(s = 0; s < sample_cnt; s++) {
        sum[i] += buf[i];
        buf += stride;
    }
    sum[i] = (sum[i] << BLUR_MULTIPLIER_BITS) / sample_cnt;

    i++;
    sum[i] = 0;
    buf = buf_ori;
    for(s = 0; s < sample_cnt; s++) {
        sum[i] += buf[i];
        buf += stride;
    }
    sum[i] = (sum[i] << BLUR_MULTIPLIER_BITS) / sample_cnt;
}

static inline void blur_2px_init(uint32_t * sum, lv_color16_t * buf, uint32_t px_cnt, uint32_t len, int32_t stride)
{

    uint32_t sample_cnt = LV_MIN(BLUR_SAMPLE_SIZE, len);
    lv_color16_t * buf_ori = buf;

    uint32_t i = 0;
    uint32_t s;

    sum[i] = 0;
    buf = buf_ori;
    for(s = 0; s < sample_cnt; s++) {
        sum[i] += buf->red;
        buf += stride;
    }
    sum[i] = (sum[i] << BLUR_MULTIPLIER_BITS) / sample_cnt;

    i++;
    sum[i] = 0;
    buf = buf_ori;
    for(s = 0; s < sample_cnt; s++) {
        sum[i] += buf->green;
        buf += stride;
    }
    sum[i] = (sum[i] << BLUR_MULTIPLIER_BITS) / sample_cnt;

    i++;
    sum[i] = 0;
    buf = buf_ori;
    for(s = 0; s < sample_cnt; s++) {
        sum[i] += buf->blue;
        buf += stride;
    }
    sum[i] = (sum[i] << BLUR_MULTIPLIER_BITS) / sample_cnt;
}

static inline void blur_3px(uint32_t * sum, uint8_t * buf, uint32_t intensity)
{
    uint32_t intensity_inv = BLUR_MULTIPLIER_MAX - intensity;

    uint32_t ch = 0;
    if(buf[ch] != sum[ch] >> BLUR_MULTIPLIER_BITS) {
        sum[ch] = ((sum[ch] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[ch] * intensity_inv));
        buf[ch] = sum[ch] >> BLUR_MULTIPLIER_BITS;
    }

    ch++;
    if(buf[ch] != sum[ch] >> BLUR_MULTIPLIER_BITS) {
        sum[ch] = ((sum[ch] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[ch] * intensity_inv));
        buf[ch] = sum[ch] >> BLUR_MULTIPLIER_BITS;
    }

    ch++;
    if(buf[ch] != sum[ch] >> BLUR_MULTIPLIER_BITS) {
        sum[ch] = ((sum[ch] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[ch] * intensity_inv));
        buf[ch] = sum[ch] >> BLUR_MULTIPLIER_BITS;
    }
}

static inline void blur_2px(uint32_t * sum, lv_color16_t * buf, uint32_t intensity)
{
    uint32_t intensity_inv = BLUR_MULTIPLIER_MAX - intensity;
    uint32_t ch = 0;
    sum[ch] = ((sum[ch] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf->red * intensity_inv));
    buf->red = (sum[ch] + BLUR_MULTIPLIER_MAX / 2) >> BLUR_MULTIPLIER_BITS;

    ch++;
    sum[ch] = ((sum[ch] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf->green * intensity_inv));
    buf->green = (sum[ch] + BLUR_MULTIPLIER_MAX / 2) >> BLUR_MULTIPLIER_BITS;

    ch++;
    sum[ch] = ((sum[ch] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf->blue * intensity_inv));
    buf->blue = (sum[ch] + BLUR_MULTIPLIER_MAX / 2) >> BLUR_MULTIPLIER_BITS;
}

static inline int32_t get_cir_p(int32_t p1, int32_t p2, int32_t p, int32_t r)
{
    if(p < p1 + r) p = r - (p - p1);
    else if(p > p2 - r) p = r - (p2 - p);
    else return 0;

    uint32_t res = lv_sqrt32(r * r - p * p);
    return r - res;
}

void lv_draw_sw_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->intensity == 0) return;
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) return;
    lv_area_move(&clipped_coords, -t->target_layer->buf_area.x1, -t->target_layer->buf_area.y1);

    uint32_t intensity = (BLUR_MULTIPLIER_MAX * dsc->intensity) / (dsc->intensity + 4);

    int32_t radius = dsc->radius;
    int32_t w = lv_area_get_width(coords);
    int32_t h = lv_area_get_height(coords);
    int32_t short_side = LV_MIN(w, h);
    if(radius > short_side >> 1) radius = short_side >> 1;

    uint32_t px_size = lv_color_format_get_size(t->target_layer->draw_buf->header.cf);

    int32_t stride = t->target_layer->draw_buf->header.stride;
    if(px_size == 2) {
        /*For RGB565 use px stride not byte stride*/
        stride = stride / 2;
    }

    uint32_t sum_start[3];
    uint32_t sum_end[3];

    int32_t y;
    int32_t x;

    for(y = clipped_coords.y1; y <= clipped_coords.y2; y++) {
        int32_t cir_x = get_cir_p(coords->y1, coords->y2, y, radius);
        int32_t x_start = coords->x1 + cir_x;
        int32_t x_end = coords->x2 - cir_x;

        x_start = LV_CLAMP(clipped_coords.x1, x_start, clipped_coords.x2);
        x_end = LV_CLAMP(clipped_coords.x1, x_end, clipped_coords.x2);
        if(x_start > x_end) continue;

        if(px_size >= 3) {
            uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
            uint8_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);

            blur_3px_init(sum_start, buf_start, px_size, x_end - x_start + 1, px_size);
            blur_3px_init(sum_end, buf_end, px_size, x_end - x_start + 1, -px_size);

            for(x = x_start; x <= x_end; x++) {
                blur_3px(sum_start, buf_start, intensity);
                buf_start += px_size;
            }

            for(x = x_start; x <= x_end; x++) {
                blur_3px(sum_end, buf_end, intensity);
                buf_end -= px_size;
            }
        }
        else if(px_size == 2) {
            lv_color16_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
            lv_color16_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);

            blur_2px_init(sum_start, buf_start, px_size, x_end - x_start + 1, 1);
            blur_2px_init(sum_end, buf_end, px_size, x_end - x_start + 1, -1);

            for(x = x_start; x <= x_end; x++) {
                blur_2px(sum_start, buf_start, intensity);
                buf_start ++;
            }

            for(x = x_start; x <= x_end; x++) {
                blur_2px(sum_end, buf_end, intensity);
                buf_end --;
            }
        }
    }


    for(x = clipped_coords.x1; x <= clipped_coords.x2; x++) {
        int32_t cir_y = get_cir_p(coords->x1, coords->x2, x, radius);
        int32_t y_start = coords->y1 + cir_y;
        int32_t y_end = coords->y2 - cir_y;

        y_start = LV_CLAMP(clipped_coords.y1, y_start, clipped_coords.y2);
        y_end = LV_CLAMP(clipped_coords.y1, y_end, clipped_coords.y2);
        if(y_start > y_end) continue;

        if(px_size >= 3) {
            uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            uint8_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);
            blur_3px_init(sum_start, buf_start, px_size, y_end - y_start + 1, stride);
            blur_3px_init(sum_end, buf_end, px_size, y_end - y_start + 1, -stride);

            for(y = y_start; y <= y_end; y++) {
                blur_3px(sum_start, buf_start, intensity);
                buf_start += stride;
            }

            for(y = y_start; y <= y_end; y++) {
                blur_3px(sum_end, buf_end, intensity);
                buf_end -= stride;
            }
        }
        else if(px_size == 2) {
            lv_color16_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            lv_color16_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);

            blur_2px_init(sum_start, buf_start, px_size, y_end - y_start + 1, stride);
            blur_2px_init(sum_end, buf_end, px_size, y_end - y_start + 1, -stride);

            for(y = y_start; y <= y_end; y++) {
                blur_2px(sum_start, buf_start, intensity);
                buf_start += stride;
            }

            for(y = y_start; y <= y_end; y++) {
                blur_2px(sum_end, buf_end, intensity);
                buf_end -= stride;
            }
        }
    }

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif /*LV_USE_DRAW_SW*/
