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
#define BLUR_MULTIPLIER_BITS 12
#define BLUR_MULTIPLIER_MAX (1 << 12)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void blur_3_bytes_init(uint32_t * sum, uint8_t * buf, uint32_t sample_len, int32_t stride);
static void blur_2_bytes_init(uint32_t * sum, lv_color16_t * buf, uint32_t sample_len, int32_t stride);

static inline void blur_2_bytes(uint32_t * sum, lv_color16_t * buf, uint32_t intensity);
static inline void blur_3_bytes(uint32_t * sum, uint8_t * buf, uint32_t intensity);
static int32_t get_rounded_edge_point(int32_t p_start, int32_t p_end, int32_t p, int32_t r);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->blur_radius == 0) return;
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) return;
    lv_area_move(&clipped_coords, -t->target_layer->buf_area.x1, -t->target_layer->buf_area.y1);

    uint32_t intensity = (BLUR_MULTIPLIER_MAX * dsc->blur_radius) / (dsc->blur_radius + 4);
    int32_t sample_len = LV_MAX(dsc->blur_radius / 2, 1);
    int32_t radius = dsc->corner_radius;
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
        int32_t cir_x = get_rounded_edge_point(coords->y1, coords->y2, y, radius);
        int32_t x_start = coords->x1 + cir_x;
        int32_t x_end = coords->x2 - cir_x;

        x_start = LV_CLAMP(clipped_coords.x1, x_start, clipped_coords.x2);
        x_end = LV_CLAMP(clipped_coords.x1, x_end, clipped_coords.x2);
        if(x_start > x_end) continue;

        uint32_t sample_len_limited = LV_MIN(x_end - x_start + 1, sample_len);

        if(px_size >= 3) {
            uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
            uint8_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);

            blur_3_bytes_init(sum_start, buf_start, sample_len_limited, px_size);
            blur_3_bytes_init(sum_end, buf_end, sample_len_limited, -px_size);

            for(x = x_start; x <= x_end; x++) {
                blur_3_bytes(sum_start, buf_start, intensity);
                buf_start += px_size;
            }

            for(x = x_start; x <= x_end; x++) {
                blur_3_bytes(sum_end, buf_end, intensity);
                buf_end -= px_size;
            }
        }
        else if(px_size == 2) {
            lv_color16_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
            lv_color16_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);

            blur_2_bytes_init(sum_start, buf_start, sample_len, 1);
            blur_2_bytes_init(sum_end, buf_end, sample_len, -1);

            for(x = x_start; x <= x_end; x++) {
                blur_2_bytes(sum_start, buf_start, intensity);
                buf_start ++;
            }

            for(x = x_start; x <= x_end; x++) {
                blur_2_bytes(sum_end, buf_end, intensity);
                buf_end --;
            }
        }
    }


    for(x = clipped_coords.x1; x <= clipped_coords.x2; x++) {
        int32_t cir_y = get_rounded_edge_point(coords->x1, coords->x2, x, radius);
        int32_t y_start = coords->y1 + cir_y;
        int32_t y_end = coords->y2 - cir_y;

        y_start = LV_CLAMP(clipped_coords.y1, y_start, clipped_coords.y2);
        y_end = LV_CLAMP(clipped_coords.y1, y_end, clipped_coords.y2);
        if(y_start > y_end) continue;

        uint32_t sample_len_limited = LV_MIN(y_end - y_start + 1, sample_len);

        if(px_size >= 3) {
            uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            uint8_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);
            blur_3_bytes_init(sum_start, buf_start, sample_len_limited, stride);
            blur_3_bytes_init(sum_end, buf_end, sample_len_limited, -stride);

            for(y = y_start; y <= y_end; y++) {
                blur_3_bytes(sum_start, buf_start, intensity);
                buf_start += stride;
            }

            for(y = y_start; y <= y_end; y++) {
                blur_3_bytes(sum_end, buf_end, intensity);
                buf_end -= stride;
            }
        }
        else if(px_size == 2) {
            lv_color16_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            lv_color16_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);

            blur_2_bytes_init(sum_start, buf_start, sample_len_limited, stride);
            blur_2_bytes_init(sum_end, buf_end, sample_len_limited, -stride);

            for(y = y_start; y <= y_end; y++) {
                blur_2_bytes(sum_start, buf_start, intensity);
                buf_start += stride;
            }

            for(y = y_start; y <= y_end; y++) {
                blur_2_bytes(sum_end, buf_end, intensity);
                buf_end -= stride;
            }
        }
    }

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void blur_3_bytes_init(uint32_t * sum, uint8_t * buf, uint32_t sample_len, int32_t stride)
{
    uint32_t s;

    sum[0] = 0;
    sum[1] = 0;
    sum[2] = 0;

    for(s = 0; s < sample_len; s++) {
        sum[0] += buf[0];
        sum[1] += buf[1];
        sum[2] += buf[2];
        buf += stride;
    }
    sum[0] = (sum[0] << BLUR_MULTIPLIER_BITS) / sample_len;
    sum[1] = (sum[1] << BLUR_MULTIPLIER_BITS) / sample_len;
    sum[2] = (sum[2] << BLUR_MULTIPLIER_BITS) / sample_len;
}

static void blur_2_bytes_init(uint32_t * sum, lv_color16_t * buf, uint32_t sample_len, int32_t stride)
{
    uint32_t s;

    sum[0] = 0;
    sum[1] = 0;
    sum[2] = 0;
    for(s = 0; s < sample_len; s++) {
        sum[0] += buf->red;
        sum[1] += buf->green;
        sum[2] += buf->blue;
        buf += stride;
    }

    sum[0] = (sum[0] << BLUR_MULTIPLIER_BITS) / sample_len;
    sum[1] = (sum[1] << BLUR_MULTIPLIER_BITS) / sample_len;
    sum[2] = (sum[2] << BLUR_MULTIPLIER_BITS) / sample_len;
}

static inline void blur_2_bytes(uint32_t * sum, lv_color16_t * buf, uint32_t intensity)
{

    uint32_t intensity_inv = BLUR_MULTIPLIER_MAX - intensity;
    sum[0] = ((sum[0] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf->red * intensity_inv));
    buf->red = (sum[0] + BLUR_MULTIPLIER_MAX / 2) >> BLUR_MULTIPLIER_BITS;

    sum[1] = ((sum[1] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf->green * intensity_inv));
    buf->green = (sum[1] + BLUR_MULTIPLIER_MAX / 2) >> BLUR_MULTIPLIER_BITS;

    sum[2] = ((sum[2] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf->blue * intensity_inv));
    buf->blue = (sum[2] + BLUR_MULTIPLIER_MAX / 2) >> BLUR_MULTIPLIER_BITS;
}

static inline void blur_3_bytes(uint32_t * sum, uint8_t * buf, uint32_t intensity)
{
    uint32_t intensity_inv = BLUR_MULTIPLIER_MAX - intensity;

    if(buf[0] != sum[0] >> BLUR_MULTIPLIER_BITS) {
        sum[0] = ((sum[0] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[0] * intensity_inv));
        buf[0] = sum[0] >> BLUR_MULTIPLIER_BITS;
    }

    if(buf[1] != sum[1] >> BLUR_MULTIPLIER_BITS) {
        sum[1] = ((sum[1] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[1] * intensity_inv));
        buf[1] = sum[1] >> BLUR_MULTIPLIER_BITS;
    }

    if(buf[2] != sum[2] >> BLUR_MULTIPLIER_BITS) {
        sum[2] = ((sum[2] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[2] * intensity_inv));
        buf[2] = sum[2] >> BLUR_MULTIPLIER_BITS;
    }
}

/**
 * Get the X or Y point for a rounded edge.
 * If the X coordinates are used Y will be returned and vice versa
 * Calculates the left or top edge
 * @param p_start   the edge's X1 or Y1 coordinate
 * @param p_end     the edge's X2 or Y2 coordinate
 * @param p         the X or Y coordinate on the edge for which the related X or X should be returned
 * @param r         the radius of the corner
 * @return          the X or Y coordinate corresponding to the provided coordinates
 */
static int32_t get_rounded_edge_point(int32_t p_start, int32_t p_end, int32_t p, int32_t r)
{
    if(p < p_start + r) p = r - (p - p_start);
    else if(p > p_end - r) p = r - (p_end - p);
    else return 0;

    uint32_t res = lv_sqrt32(r * r - p * p);
    return r - res;
}


#endif /*LV_USE_DRAW_SW*/
