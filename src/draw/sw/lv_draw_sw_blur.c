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
#define BLUR_INTENSITY_BITS 12
#define BLUR_INTENSITY_MAX (1 << 12)
#define BLUR_INTENSITY_HALF ((1 << 12) / 2)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void blur_1_bytes_init(uint32_t * sum, uint8_t * buf, uint32_t sample_len, int32_t stride);
static inline uint8_t blur_1_bytes(uint32_t * sum, uint8_t px, uint32_t intensity);

static void blur_2_bytes_init(uint32_t * sum, lv_color16_t * buf, uint32_t sample_len, int32_t stride, bool swapped);
static inline uint16_t blur_2_bytes(uint32_t * sum, uint16_t px, uint32_t intensity, bool swapped);

static void blur_3_bytes_init(uint32_t * sum, volatile uint8_t * buf, uint32_t sample_len, int32_t stride);
static inline void blur_3_bytes(uint32_t * sum, volatile uint8_t * buf, uint32_t intensity);

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

    int32_t layer_x_ofs = t->target_layer->buf_area.x1;
    int32_t layer_y_ofs = t->target_layer->buf_area.y1;
    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) return;
    lv_area_move(&clipped_coords, -layer_x_ofs, -layer_y_ofs);

    uint32_t blur_radius = dsc->blur_radius;

    /*On larger radius skip some pixels as the result is a blob anyways, so not all pixels matter
     *This only every 2nd or 3rd px will be blurred, the result will be stored in the layers buffers,
     *and finally the missing pixels are set to nearest blurred pixel. We loose precision but it looks ok
     *and and it's very fast.
     */
    int32_t skip_cnt = 1;
    if(dsc->quality == LV_BLUR_QUALITY_AUTO) {
        int32_t size = lv_area_get_size(&clipped_coords);
        if(blur_radius >= 32 && dsc->corner_radius == 0 && size > 160 * 160) skip_cnt = 3;
        else if(blur_radius >= 8) skip_cnt = 2;
    }
    else if(dsc->quality == LV_BLUR_QUALITY_SPEED) {
        if(blur_radius >= 24) skip_cnt = 3;
        else skip_cnt = 2;
    }

    /*The blurring are must be multiples of skip_cnt so the blurring is all directions
     * blur the same pixels if some pixels are skipped*/
    clipped_coords.x1 = ((clipped_coords.x1 + (skip_cnt - 1)) / skip_cnt) * skip_cnt;
    clipped_coords.x2 = ((clipped_coords.x2 - (skip_cnt - 1)) / skip_cnt) * skip_cnt;
    clipped_coords.y1 = ((clipped_coords.y1 + (skip_cnt - 1)) / skip_cnt) * skip_cnt;
    clipped_coords.y2 = ((clipped_coords.y2 - (skip_cnt - 1)) / skip_cnt) * skip_cnt;
    if(lv_area_get_width(&clipped_coords) < 0) return;
    if(lv_area_get_height(&clipped_coords) < 0) return;

    blur_radius = blur_radius / skip_cnt;

    /*We will use an IIR low pass filer in all 4 direction:  top to bottom, bottom to top, left to right, right to left.
     *Approximate the the filter coefficient from the radius.
     *The filter is like: this_px = mix(prev_px, this_px, intensity)
     */
    uint32_t intensity = (BLUR_INTENSITY_MAX * blur_radius) / (blur_radius + 4);

    int32_t sample_len = LV_MAX(blur_radius / 2, 1);
    int32_t radius = dsc->corner_radius;
    int32_t w = lv_area_get_width(coords);
    int32_t h = lv_area_get_height(coords);
    int32_t short_side = LV_MIN(w, h);
    if(radius > short_side >> 1) radius = short_side >> 1;

    uint32_t px_size = lv_color_format_get_size(t->target_layer->draw_buf->header.cf);
    int32_t stride_byte = t->target_layer->draw_buf->header.stride;
    int32_t stride_px = stride_byte / px_size;
    int32_t next_px_ofs_byte = px_size * skip_cnt;
    bool swapped = t->target_layer->draw_buf->header.cf == LV_COLOR_FORMAT_RGB565_SWAPPED;

    uint32_t sum[3];
    int32_t y;
    int32_t x;

    /*Blur each column top to bottom and bottom to top.*/
    for(x = clipped_coords.x1; x <= clipped_coords.x2; x += skip_cnt) {
        int32_t cir_y = get_rounded_edge_point(coords->x1, coords->x2, layer_x_ofs + x, radius);
        int32_t y_start = LV_CLAMP(clipped_coords.y1, coords->y1 - layer_y_ofs + cir_y, clipped_coords.y2);
        int32_t y_end = LV_CLAMP(clipped_coords.y1, coords->y2  - layer_y_ofs - cir_y, clipped_coords.y2);

        /*Make sure that the width and height is a multiple of skip_cnt so that back and forth blurring
         *surely affects the same pixels */
        y_start = (y_start / skip_cnt) * skip_cnt;
        y_end = (y_end / skip_cnt) * skip_cnt;
        if(y_start > y_end) continue;

        uint32_t sample_len_limited = LV_MIN((y_end - y_start) / skip_cnt + 1, sample_len);

        if(px_size == 1) {
            /*Compiler optimization might mishandle it, so add volatile*/
            uint8_t * buf_column_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            blur_1_bytes_init(sum, buf_column_start, sample_len_limited, stride_byte * skip_cnt);

            uint8_t buf_prev = buf_column_start[0] + 1; /*Make sure that it's not equal in the first round*/
            for(y = y_start; y <= y_end; y += skip_cnt) {
                if(buf_prev != *buf_column_start) {
                    *buf_column_start = blur_1_bytes(sum, *buf_column_start, intensity);
                    buf_prev = *buf_column_start;
                }
                buf_column_start += stride_byte * skip_cnt;
            }

            uint8_t * buf_column_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);
            blur_1_bytes_init(sum, buf_column_end, sample_len_limited, -stride_byte * skip_cnt);
            buf_prev = buf_column_end[0] + 1; /*Make sure that it's not equal in the first round*/
            for(y = y_start; y <= y_end; y += skip_cnt) {
                if(buf_prev != *buf_column_end) {
                    *buf_column_end = blur_1_bytes(sum, *buf_column_end, intensity);
                    buf_prev = *buf_column_end;
                }
                buf_column_end -= stride_byte * skip_cnt;
            }
        }
        else if(px_size == 2) {
            uint16_t * buf16_column_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            blur_2_bytes_init(sum, (lv_color16_t *)buf16_column_start, sample_len_limited, stride_px * skip_cnt, swapped);
            uint16_t buf16_prev = buf16_column_start[0] + 1; /*Make sure that it's not equal in the first round*/

            for(y = y_start; y <= y_end; y += skip_cnt) {
                if(buf16_prev != *buf16_column_start) {
                    *buf16_column_start = blur_2_bytes(sum, *buf16_column_start, intensity, swapped);
                    buf16_prev = *buf16_column_start;
                }
                buf16_column_start += stride_px * skip_cnt;
            }

            uint16_t * buf16_column_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);
            blur_2_bytes_init(sum, (lv_color16_t *)buf16_column_end, sample_len_limited, -stride_px * skip_cnt, swapped);
            buf16_prev = buf16_column_end[0] + 1; /*Make sure that it's not equal in the first round*/

            for(y = y_start; y <= y_end; y += skip_cnt) {
                if(buf16_prev != *buf16_column_end) {
                    *buf16_column_end = blur_2_bytes(sum, *buf16_column_end, intensity, swapped);
                    buf16_prev = *buf16_column_end;
                }
                buf16_column_end -= stride_px * skip_cnt;
            }
        }
        else if(px_size >= 3) {
            /*Compiler optimization might mishandle it, so add volatile*/
            volatile uint8_t * buf_column_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_start);
            blur_3_bytes_init(sum, buf_column_start, sample_len_limited, stride_byte * skip_cnt);

            for(y = y_start; y <= y_end; y += skip_cnt) {
                blur_3_bytes(sum, buf_column_start, intensity);
                buf_column_start += stride_byte * skip_cnt;
            }

            volatile uint8_t * buf_column_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x, y_end);
            blur_3_bytes_init(sum, buf_column_end, sample_len_limited, -stride_byte * skip_cnt);
            for(y = y_start; y <= y_end; y += skip_cnt) {
                blur_3_bytes(sum, buf_column_end, intensity);
                buf_column_end -= stride_byte * skip_cnt;
            }
        }
    }

    /*Blur each line from left to right and right to left.
     *Also fill the gap in each line because of skipped pixels*/
    for(y = clipped_coords.y1; y <= clipped_coords.y2; y += skip_cnt) {
        int32_t cir_x = get_rounded_edge_point(coords->y1, coords->y2, layer_y_ofs + y, radius);
        int32_t x_start = LV_CLAMP(clipped_coords.x1, coords->x1  - layer_x_ofs + cir_x, clipped_coords.x2);
        int32_t x_end = LV_CLAMP(clipped_coords.x1, coords->x2  - layer_x_ofs - cir_x, clipped_coords.x2);

        /*Make sure that the width and height is a multiple of skip_cnt so that back and forth blurring
         *surely affects the same pixels */
        x_start = (x_start / skip_cnt) * skip_cnt;
        x_end = (x_end / skip_cnt) * skip_cnt;

        if(x_start > x_end) continue;
        uint32_t line_len_byte = (x_end - x_start + skip_cnt) * px_size;
        uint32_t sample_len_limited = LV_MIN((x_end - x_start) / skip_cnt + 1, sample_len);


        if(px_size == 1) {
            /*Compiler optimization might mishandle it, so add volatile*/
            uint8_t * buf_line_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);

            blur_1_bytes_init(sum, buf_line_start, sample_len_limited, px_size * skip_cnt);

            uint8_t buf_prev = buf_line_start[0] + 1; /*Make sure that it's not equal in the first round*/
            for(x = x_start + skip_cnt; x <= x_end; x += skip_cnt) {
                if(buf_prev != *buf_line_start) {
                    *buf_line_start = blur_1_bytes(sum, *buf_line_start, intensity);
                    buf_prev = *buf_line_start;
                }
                buf_line_start += next_px_ofs_byte;
            }

            uint8_t * buf_line_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);
            blur_1_bytes_init(sum, buf_line_end, sample_len_limited, -(int32_t)px_size * skip_cnt);
            buf_prev = buf_line_end[0] + 1; /*Make sure that it's not equal in the first round*/

            for(x = x_start; x <= x_end; x += skip_cnt) {
                if(buf_prev != *buf_line_end) {
                    *buf_line_end = blur_1_bytes(sum, *buf_line_end, intensity);
                    buf_prev = *buf_line_end;
                }

                /*This is the final pixel, fill the gaps in the line by just repeating the pixel (simple upscale)*/
                if(skip_cnt == 2) {
                    buf_line_end[1] = buf_line_end[0];
                }
                else if(skip_cnt == 3) {
                    buf_line_end[1] = buf_line_end[0];
                    buf_line_end[2] = buf_line_end[0];
                }

                /*Fill the empty lines by duplicating a the finished filled lines to the gaps*/
                if(skip_cnt > 1 && x + skip_cnt > x_end) {
                    uint8_t * buf_copy_from = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
                    lv_memcpy(buf_copy_from + stride_byte, buf_copy_from, line_len_byte);
                    if(skip_cnt == 3) {
                        lv_memcpy(buf_copy_from + stride_byte * 2, buf_copy_from, line_len_byte);
                    }
                }

                buf_line_end -= next_px_ofs_byte;
            }

        }
        else if(px_size == 2) {
            uint16_t * buf16_line_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
            blur_2_bytes_init(sum, (lv_color16_t *)buf16_line_start, sample_len_limited,  skip_cnt, swapped);

            uint16_t buf16_prev = buf16_line_start[0] + 1; /*Make sure that it's not equal in the first round*/
            for(x = x_start; x <= x_end; x += skip_cnt) {

                if(buf16_prev != *buf16_line_start) {
                    *buf16_line_start = blur_2_bytes(sum, *buf16_line_start, intensity, swapped);
                    buf16_prev = *buf16_line_start;
                }
                buf16_line_start += skip_cnt;
            }

            uint16_t * buf16_line_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);
            blur_2_bytes_init(sum, (lv_color16_t *)buf16_line_end, sample_len_limited, - skip_cnt, swapped);
            buf16_prev = buf16_line_end[0] + 1; /*Make sure that it's not equal in the first round*/

            for(x = x_start; x <= x_end; x += skip_cnt) {
                if(buf16_prev != *buf16_line_end) {
                    *buf16_line_end = blur_2_bytes(sum, *buf16_line_end, intensity, swapped);
                    buf16_prev = *buf16_line_end;
                }

                /*This is the final pixel, fill the gaps in the line by just repeating the pixel (simple upscale)*/
                if(skip_cnt == 2) {
                    /*Fill the empty lines by duplicating a the finished filled lines to the gaps*/
                    buf16_line_end[1] = buf16_line_end[0];
                }
                else if(skip_cnt == 3) {
                    /*Fill the empty lines by duplicating a the finished filled lines to the gaps*/
                    buf16_line_end[1] = buf16_line_end[0];
                    buf16_line_end[2] = buf16_line_end[0];
                }

                /*Fill the empty lines by duplicating a the finished filled lines to the gaps*/
                if(skip_cnt > 1 && x + skip_cnt > x_end) {
                    uint8_t * buf_copy_from = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
                    lv_memcpy(buf_copy_from + stride_byte, buf_copy_from, line_len_byte);
                    if(skip_cnt == 3) {
                        lv_memcpy(buf_copy_from + stride_byte * 2, buf_copy_from, line_len_byte);
                    }
                }

                buf16_line_end -= skip_cnt;
            }
        }
        else if(px_size >= 3) {
            /*Compiler optimization might mishandle it, so add volatile*/
            volatile uint8_t * buf_line_start = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);

            blur_3_bytes_init(sum, buf_line_start, sample_len_limited, px_size * skip_cnt);
            for(x = x_start + skip_cnt; x <= x_end; x += skip_cnt) {
                blur_3_bytes(sum, buf_line_start, intensity);
                buf_line_start += next_px_ofs_byte;
            }

            volatile uint8_t * buf_line_end = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_end, y);
            blur_3_bytes_init(sum, buf_line_end, sample_len_limited, -(int32_t)px_size * skip_cnt);

            for(x = x_start; x <= x_end; x += skip_cnt) {
                blur_3_bytes(sum, buf_line_end, intensity);

                /*This is the final pixel, fill the gaps in the line by just repeating the pixel (simple upscale)*/
                if(skip_cnt == 2) {
                    buf_line_end[px_size + 0] = buf_line_end[0];
                    buf_line_end[px_size + 1] = buf_line_end[1];
                    buf_line_end[px_size + 2] = buf_line_end[2];
                }
                else if(skip_cnt == 3) {
                    buf_line_end[px_size + 0] = buf_line_end[0];
                    buf_line_end[px_size + 1] = buf_line_end[1];
                    buf_line_end[px_size + 2] = buf_line_end[2];
                    buf_line_end[px_size * 2 + 0] = buf_line_end[0];
                    buf_line_end[px_size * 2 + 1] = buf_line_end[1];
                    buf_line_end[px_size * 2 + 2] = buf_line_end[2];
                }

                /*Fill the empty lines by duplicating a the finished filled lines to the gaps*/
                if(skip_cnt > 1 && x + skip_cnt > x_end) {
                    uint8_t * buf_copy_from = lv_draw_buf_goto_xy(t->target_layer->draw_buf, x_start, y);
                    lv_memcpy(buf_copy_from + stride_byte, buf_copy_from, line_len_byte);
                    if(skip_cnt == 3) {
                        lv_memcpy(buf_copy_from + stride_byte * 2, buf_copy_from, line_len_byte);
                    }
                }

                buf_line_end -= next_px_ofs_byte;
            }

        }
    }

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void blur_1_bytes_init(uint32_t * sum, uint8_t * buf, uint32_t sample_len, int32_t stride)
{
    uint32_t s;

    sum[0] = 0;

    for(s = 0; s < sample_len; s++) {
        sum[0] += buf[0];
        buf += stride;
    }
    sum[0] = (sum[0] << BLUR_INTENSITY_BITS) / sample_len;
}

static void blur_3_bytes_init(uint32_t * sum, volatile uint8_t * buf, uint32_t sample_len, int32_t stride)
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
    sum[0] = (sum[0] << BLUR_INTENSITY_BITS) / sample_len;
    sum[1] = (sum[1] << BLUR_INTENSITY_BITS) / sample_len;
    sum[2] = (sum[2] << BLUR_INTENSITY_BITS) / sample_len;
}

static void blur_2_bytes_init(uint32_t * sum, lv_color16_t * buf, uint32_t sample_len, int32_t stride, bool swapped)
{
    uint32_t s;

    sum[0] = 0;
    sum[1] = 0;
    sum[2] = 0;
    for(s = 0; s < sample_len; s++) {
        uint16_t v = *(uint16_t *)buf;
        if(swapped) v = (v >> 8) | (v << 8);
        lv_color16_t * c = (lv_color16_t *)&v;
        sum[0] += c->red;
        sum[1] += c->green;
        sum[2] += c->blue;
        buf += stride;
    }

    sum[0] = (sum[0] << BLUR_INTENSITY_BITS) / sample_len;
    sum[1] = (sum[1] << BLUR_INTENSITY_BITS) / sample_len;
    sum[2] = (sum[2] << BLUR_INTENSITY_BITS) / sample_len;
}


static inline uint8_t blur_1_bytes(uint32_t * sum, uint8_t px, uint32_t intensity)
{
    uint32_t intensity_inv = BLUR_INTENSITY_MAX - intensity;

    *sum = (((*sum) * intensity) >> BLUR_INTENSITY_BITS) + ((px * intensity_inv));
    return (*sum) >> BLUR_INTENSITY_BITS;
}

static inline uint16_t blur_2_bytes(uint32_t * sum, uint16_t px, uint32_t intensity, bool swapped)
{
    const uint32_t inv = BLUR_INTENSITY_MAX - intensity;
    const uint32_t half = BLUR_INTENSITY_MAX >> 1;
    const uint32_t shift = BLUR_INTENSITY_BITS;

    if(swapped) px = (px >> 8) | (px << 8);

    /* unpack */
    uint32_t r =  px >> 11;
    uint32_t g = (px >> 5) & 0x3F;
    uint32_t b =  px        & 0x1F;

    uint32_t s0 = sum[0];
    uint32_t s1 = sum[1];
    uint32_t s2 = sum[2];

    /* fused multiply-accumulate pattern */
    s0 = (s0 * intensity >> shift) + (r * inv);
    s1 = (s1 * intensity >> shift) + (g * inv);
    s2 = (s2 * intensity >> shift) + (b * inv);

    sum[0] = s0;
    sum[1] = s1;
    sum[2] = s2;

    /* final */
    r = (s0 + half) >> shift;
    g = (s1 + half) >> shift;
    b = (s2 + half) >> shift;

    uint16_t res = (uint16_t)((r << 11) | (g << 5) | b);
    if(swapped) res = (res >> 8) | (res << 8);
    return res;
}



static inline void blur_3_bytes(uint32_t * sum, volatile uint8_t * buf, uint32_t intensity)
{
    uint32_t intensity_inv = BLUR_INTENSITY_MAX - intensity;

    sum[0] = ((sum[0] * intensity) >> BLUR_INTENSITY_BITS) + ((buf[0] * intensity_inv));
    buf[0] = sum[0] >> BLUR_INTENSITY_BITS;

    sum[1] = ((sum[1] * intensity) >> BLUR_INTENSITY_BITS) + ((buf[1] * intensity_inv));
    buf[1] = sum[1] >> BLUR_INTENSITY_BITS;

    sum[2] = ((sum[2] * intensity) >> BLUR_INTENSITY_BITS) + ((buf[2] * intensity_inv));
    buf[2] = sum[2] >> BLUR_INTENSITY_BITS;
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
