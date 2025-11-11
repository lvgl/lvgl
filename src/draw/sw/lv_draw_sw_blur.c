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

static inline void repeate_2x2(uint8_t * buf, uint32_t px_size, uint32_t stride)
{
    buf[px_size + 0] = buf[0];
    buf[px_size + 1] = buf[1];
    buf[px_size + 2] = buf[2];
    buf[stride + 0] = buf[0];
    buf[stride + 1] = buf[1];
    buf[stride + 2] = buf[2];
    buf[stride + px_size + 0] = buf[0];
    buf[stride + px_size + 1] = buf[1];
    buf[stride + px_size + 2] = buf[2];
}

static inline void repeate_2x2_2byte(lv_color16_t * buf, uint32_t stride)
{
    buf[1] = buf[0];
    buf[stride] = buf[0];
    buf[stride + 1] = buf[0];
}

void lv_draw_sw_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->blur_radius == 0) return;
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) return;
    lv_area_move(&clipped_coords, -t->target_layer->buf_area.x1, -t->target_layer->buf_area.y1);

    uint32_t blur_radius = dsc->blur_radius;
    uint32_t skip_cnt = 1;
    //    if(dsc->corner_radius == 0)
    {
        if(blur_radius >= 32) {
            skip_cnt = 4;
            blur_radius = blur_radius / 4;
        }
        else if(blur_radius >= 8) {
            skip_cnt = 2;
            blur_radius = blur_radius / 2;
        }
    }

    uint32_t intensity = (BLUR_MULTIPLIER_MAX * blur_radius) / (blur_radius + 4);

    int32_t sample_len = LV_MAX(blur_radius / 2, 1);
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
    uint32_t not_equal_buf;
    uint8_t * buf_prev;

    uint32_t sum_start[3];
    uint32_t sum_end[3];

    int32_t y;
    int32_t x;

    uint32_t cnt = 0;
    for(y = (clipped_coords.y1 / skip_cnt) * skip_cnt; y <= (clipped_coords.y2 / skip_cnt) * skip_cnt; y += skip_cnt) {
        int32_t cir_x = get_rounded_edge_point(coords->y1, coords->y2, y, radius);
        int32_t x_start = ((coords->x1 + cir_x) / skip_cnt) * skip_cnt;
        int32_t x_end = ((coords->x2 - cir_x) / skip_cnt) * skip_cnt;

        x_start = LV_CLAMP((clipped_coords.x1 / skip_cnt) * skip_cnt, x_start, clipped_coords.x2);
        x_end = LV_CLAMP(clipped_coords.x1, x_end, (clipped_coords.x2 / skip_cnt) * skip_cnt);
        if(x_start > x_end) continue;

        uint32_t sample_len_limited = LV_MIN(x_end - x_start + 1, sample_len);

        if(px_size >= 3) {
            uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
            uint8_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);

            blur_3_bytes_init(sum_start, buf_start, sample_len_limited, px_size);
            blur_3_bytes_init(sum_end, buf_end, sample_len_limited, -px_size);
            buf_start += px_size * skip_cnt;
            for(x = x_start + skip_cnt; x <= x_end; x += skip_cnt) {
                blur_3_bytes(sum_start, buf_start, intensity);
                buf_start += px_size * skip_cnt;
            }
            for(x = x_start; x <= x_end; x += skip_cnt) {
                blur_3_bytes(sum_end, buf_end, intensity);
                buf_prev = buf_start;
                buf_end -= px_size * skip_cnt;
            }

        }
        else if(px_size == 2) {
            lv_color16_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
            lv_color16_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);

            blur_2_bytes_init(sum_start, buf_start, sample_len, 1);
            blur_2_bytes_init(sum_end, buf_end, sample_len, -1);

            not_equal_buf = 0x0000;
            buf_prev = &not_equal_buf;
            for(x = x_start; x <= x_end; x += skip_cnt) {
                uint16_t act = *((uint16_t *) buf_start);
                uint16_t prev = *((uint16_t *) buf_prev);
                if(act != prev) {
                    blur_2_bytes(sum_start, buf_start, intensity);
                    buf_prev = buf_start;
                }
                else {
                    cnt++;
                }
                buf_start +=  skip_cnt;
            }

            for(x = x_start; x <= x_end; x += skip_cnt) {
                uint16_t act = *((uint16_t *) buf_end);
                uint16_t prev = *((uint16_t *) buf_prev);
                if(act != prev) {
                    blur_2_bytes(sum_end, buf_end, intensity);
                    buf_prev = buf_end;
                }
                else {
                    cnt++;
                }
                buf_end -= skip_cnt;
            }
        }
    }


    for(x = (clipped_coords.x1 / skip_cnt) * skip_cnt; x <= (clipped_coords.x2 / skip_cnt) * skip_cnt; x += skip_cnt) {
        int32_t cir_y = get_rounded_edge_point(coords->x1, coords->x2, x, radius);
        int32_t y_start = ((coords->y1 + cir_y) / skip_cnt) * skip_cnt;
        int32_t y_end = ((coords->y2 - cir_y) / skip_cnt) * skip_cnt;

        y_start = LV_CLAMP((clipped_coords.y1 / skip_cnt) * skip_cnt, y_start, clipped_coords.y2);
        y_end = LV_CLAMP(clipped_coords.y1, y_end, (clipped_coords.y2 / skip_cnt) * skip_cnt);
        if(y_start > y_end) continue;

        uint32_t sample_len_limited = LV_MIN(y_end - y_start + 1, sample_len);

        if(px_size >= 3) {
            uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            uint8_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);
            blur_3_bytes_init(sum_start, buf_start, sample_len_limited, stride);
            blur_3_bytes_init(sum_end, buf_end, sample_len_limited, -stride);

            for(y = y_start; y <= y_end; y += skip_cnt) {
                blur_3_bytes(sum_start, buf_start, intensity);
                buf_prev = buf_start;
                buf_start += stride * skip_cnt;
            }

            for(y = y_start; y <= y_end; y += skip_cnt) {
                blur_3_bytes(sum_end, buf_end, intensity);
                buf_end -= stride * skip_cnt;
            }

            if(skip_cnt == 2) {
                buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
                for(y = y_start; y <= y_end; y += skip_cnt) {
                    repeate_2x2(buf_start, px_size, stride);
                    buf_start += stride * skip_cnt;
                }
            }
            else if(skip_cnt == 4 && x > 4) {
                buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start + 4);
                for(y = y_start + 4 ; y < y_end; y += skip_cnt) {

                    repeate_2x2(buf_start, px_size, stride);

                    uint8_t * buf_prev;

                    buf_prev = buf_start - 4 * px_size;
                    uint8_t * buf_interp1 = buf_start - 2 * px_size;
                    buf_interp1[0] = (buf_start[0] + buf_prev[0]) / 2;
                    buf_interp1[1] = (buf_start[1] + buf_prev[1]) / 2;
                    buf_interp1[2] = (buf_start[2] + buf_prev[2]) / 2;
                    repeate_2x2(buf_interp1, px_size, stride);

                    buf_prev = buf_start - 4 * stride;
                    uint8_t * buf_interp2 = buf_start - 2 * stride;
                    buf_interp2[0] = (buf_start[0] + buf_prev[0]) / 2;
                    buf_interp2[1] = (buf_start[1] + buf_prev[1]) / 2;
                    buf_interp2[2] = (buf_start[2] + buf_prev[2]) / 2;
                    repeate_2x2(buf_interp2, px_size, stride);

                    uint8_t * buf_interp3 = buf_start - 4 * stride - 2 * px_size;
                    uint8_t * buf_interp4 = buf_start - 2 * stride - 4 * px_size;
                    uint8_t * buf_interp_mid = buf_start - 2 * stride - 2 * px_size;
                    buf_interp_mid[0] = (buf_interp1[0] + buf_interp2[0] + buf_interp3[0] + buf_interp4[0]) / 4;
                    buf_interp_mid[1] = (buf_interp1[1] + buf_interp2[1] + buf_interp3[1] + buf_interp4[1]) / 4;
                    buf_interp_mid[2] = (buf_interp1[2] + buf_interp2[2] + buf_interp3[2] + buf_interp4[2]) / 4;
                    repeate_2x2(buf_interp_mid, px_size, stride);
                    buf_start += stride * skip_cnt;
                }

            }
        }
        else if(px_size == 2) {
            lv_color16_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            lv_color16_t * buf_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);

            blur_2_bytes_init(sum_start, buf_start, sample_len_limited, stride);
            blur_2_bytes_init(sum_end, buf_end, sample_len_limited, -stride);

            for(y = y_start; y <= y_end; y += skip_cnt) {
                uint16_t act = *((uint16_t *) buf_start);
                uint16_t prev = *((uint16_t *) buf_prev);
                if(act != prev) {
                    blur_2_bytes(sum_start, buf_start, intensity);
                    buf_prev = buf_start;
                }
                else {
                    cnt++;
                }
                buf_start += stride * skip_cnt;
            }

            for(y = y_start; y <= y_end; y += skip_cnt) {
                uint16_t act = *((uint16_t *) buf_end);
                uint16_t prev = *((uint16_t *) buf_prev);
                if(act != prev) {
                    blur_2_bytes(sum_end, buf_end, intensity);
                    buf_prev = buf_end;
                }
                else {
                    cnt++;
                }
                buf_end -= stride * skip_cnt;
            }

            if(skip_cnt == 2) {
                buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
                for(y = y_start; y <= y_end; y += skip_cnt) {
                    repeate_2x2_2byte(buf_start, stride);
                    buf_start += stride * skip_cnt;
                }
            }
            else if(skip_cnt == 4 && x > 4) {
                buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start + 4);
                for(y = y_start + 4; y < y_end; y += skip_cnt) {
                    //                  printf("%d %d\n", x, y);
                    uint16_t * buf_start_u16 = buf_start;
                    repeate_2x2_2byte(buf_start_u16, stride);

                    uint16_t * buf_prev;
                    //
                    buf_prev = buf_start_u16 - 4;
                    uint16_t * buf_interp1 = buf_start_u16 - 2;
                    buf_interp1[0] = lv_color_16_16_mix(buf_start_u16[0], buf_prev[0], LV_OPA_50);
                    repeate_2x2_2byte(buf_interp1, stride);
                    //
                    buf_prev = buf_start_u16 - 4 * stride;
                    uint16_t * buf_interp2 = buf_start - 2 * stride;
                    buf_interp2[0] = lv_color_16_16_mix(buf_start_u16[0], buf_prev[0], LV_OPA_50);
                    repeate_2x2_2byte(buf_interp2, stride);

                    uint16_t * buf_interp3 = buf_start_u16 - 4 * stride - 2;
                    uint16_t * buf_interp4 = buf_start_u16 - 2 * stride - 4;
                    uint16_t * buf_interp_mid = buf_start_u16 - 2 * stride - 2;
                    //              buf_interp_mid[0] = lv_color_16_16_mix(buf_interp1[0] + buf_interp2[0] + buf_interp3[0] + buf_interp4[0]) / 4;
                    buf_interp_mid[0] = lv_color_16_16_mix(buf_interp1[0], buf_interp2[0], LV_OPA_50) ;
                    repeate_2x2_2byte(buf_interp_mid, stride);
                    buf_start += stride * skip_cnt;
                }
            }
        }
    }
    //    printf("skip_cnt: %d\n", cnt);

    //    for(y = clipped_coords.y1; y < clipped_coords.y2; y+=2) {
    //
    //      int32_t cir_x = get_rounded_edge_point(coords->y1, coords->y2, y, radius);
    //      int32_t x_start = coords->x1 + cir_x;
    //      int32_t x_end = coords->x2 - cir_x;
    //      x_start = LV_CLAMP(clipped_coords.x1, x_start, clipped_coords.x2);
    //      x_end = LV_CLAMP(clipped_coords.x1, x_end, clipped_coords.x2 & ~0x1);
    //      if(x_start > x_end) continue;
    //
    //      uint8_t * buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y);
    //      for(x = clipped_coords.x1; x < clipped_coords.x2; x+=2) {
    //            buf_start[px_size + 0] = buf_start[0];
    //            buf_start[stride + 0] = buf_start[2 * stride * 2];
    //            buf_start[stride + 2 * px_size + 0] = buf_start[0];
    //


    //              for(y = y_start + 1; y < y_end; y+=2) {
    //                  uint8_t * prev_px = buf_start - stride;
    //                  buf_start[0] = (prev_px[0] + buf_start[0 + stride]) / 2;
    //                  buf_start[1] = (prev_px[1] + buf_start[1 + stride]) / 2;
    //                  buf_start[2] = (prev_px[2] + buf_start[2 + stride]) / 2;
    //                  buf_start[px_size + 0] = buf_start[0];
    //                  buf_start[px_size + 1] = buf_start[1];
    //                  buf_start[px_size + 2] = buf_start[2];
    //                  buf_start += stride*2;
    //              }
    //
    //              buf_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start + 1, y);
    //              for(x = x_start + 1; x < x_end; x+=2) {
    //                  uint8_t * prev_px = buf_start - px_size;
    //                  buf_start[0] = (prev_px[0] + buf_start[0 + px_size]) / 2;
    //                  buf_start[1] = (prev_px[1] + buf_start[1 + px_size]) / 2;
    //                  buf_start[2] = (prev_px[2] + buf_start[2 + px_size]) / 2;
    //                  buf_start += px_size*2;
    //              }

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

    sum[0] = ((sum[0] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[0] * intensity_inv));
    buf[0] = sum[0] >> BLUR_MULTIPLIER_BITS;

    sum[1] = ((sum[1] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[1] * intensity_inv));
    buf[1] = sum[1] >> BLUR_MULTIPLIER_BITS;

    sum[2] = ((sum[2] * intensity) >> BLUR_MULTIPLIER_BITS) + ((buf[2] * intensity_inv));
    buf[2] = sum[2] >> BLUR_MULTIPLIER_BITS;
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
