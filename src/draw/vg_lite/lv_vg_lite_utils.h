/**
 * @file lv_vg_lite_utils.h
 *
 */

#ifndef LV_VG_LITE_UTILS_H
#define LV_VG_LITE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_DRAW_VG_LITE

#include "../../misc/lv_profiler.h"

#include <stdbool.h>
#if LV_USE_VG_LITE_THORVG
#include "../../others/vg_lite_tvg/vg_lite.h"
#else
#include <vg_lite.h>
#endif

/*********************
 *      DEFINES
 *********************/

#define LV_VG_LITE_IS_ERROR(err) (err > 0)

#if LV_VG_LITE_USE_ASSERT
#define LV_VG_LITE_ASSERT(expr) LV_ASSERT(expr)
#else
#define LV_VG_LITE_ASSERT(expr)
#endif

#define LV_VG_LITE_CHECK_ERROR(expr)                          \
    do {                                                      \
        vg_lite_error_t error = expr;                         \
        if (LV_VG_LITE_IS_ERROR(error)) {                     \
            LV_LOG_ERROR("Execute '" #expr "' error: %d", (int)error);  \
            lv_vg_lite_error_dump_info(error);                \
            LV_VG_LITE_ASSERT(false);                         \
        }                                                     \
    } while (0)

#define LV_VG_LITE_ASSERT_PATH(path) LV_VG_LITE_ASSERT(lv_vg_lite_path_check(path))
#define LV_VG_LITE_ASSERT_SRC_BUFFER(buffer) LV_VG_LITE_ASSERT(lv_vg_lite_buffer_check(buffer, true))
#define LV_VG_LITE_ASSERT_DEST_BUFFER(buffer) LV_VG_LITE_ASSERT(lv_vg_lite_buffer_check(buffer, false))
#define LV_VG_LITE_ASSERT_MATRIX(matrix) LV_VG_LITE_ASSERT(lv_vg_lite_matrix_check(matrix))

#define LV_VG_LITE_ALIGN(number, align_bytes) \
    (((number) + ((align_bytes)-1)) & ~((align_bytes)-1))

#define LV_VG_LITE_IS_ALIGNED(num, align) (((uintptr_t)(num) & ((align)-1)) == 0)

#define LV_VG_LITE_IS_INDEX_FMT(fmt) \
    ((fmt) == VG_LITE_INDEX_1        \
     || (fmt) == VG_LITE_INDEX_2  \
     || (fmt) == VG_LITE_INDEX_4  \
     || (fmt) == VG_LITE_INDEX_8)

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_vg_lite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Print info */

void lv_vg_lite_dump_info(void);

void lv_vg_lite_error_dump_info(vg_lite_error_t error);

const char * lv_vg_lite_error_string(vg_lite_error_t error);

const char * lv_vg_lite_feature_string(vg_lite_feature_t feature);

const char * lv_vg_lite_buffer_format_string(vg_lite_buffer_format_t format);

const char * lv_vg_lite_vlc_op_string(uint8_t vlc_op);

void lv_vg_lite_path_dump_info(const vg_lite_path_t * path);

void lv_vg_lite_stroke_dump_info(const vg_lite_stroke_t * stroke);

void lv_vg_lite_buffer_dump_info(const vg_lite_buffer_t * buffer);

void lv_vg_lite_matrix_dump_info(const vg_lite_matrix_t * matrix);

bool lv_vg_lite_is_dest_cf_supported(lv_color_format_t cf);

bool lv_vg_lite_is_src_cf_supported(lv_color_format_t cf);

/* Converter */

vg_lite_buffer_format_t lv_vg_lite_vg_fmt(lv_color_format_t cf);

void lv_vg_lite_buffer_format_bytes(
    vg_lite_buffer_format_t format,
    uint32_t * mul,
    uint32_t * div,
    uint32_t * bytes_align);

uint32_t lv_vg_lite_width_to_stride(uint32_t w, vg_lite_buffer_format_t color_format);

uint32_t lv_vg_lite_width_align(uint32_t w);

void lv_vg_lite_buffer_init(
    vg_lite_buffer_t * buffer,
    const void * ptr,
    int32_t width,
    int32_t height,
    uint32_t stride,
    vg_lite_buffer_format_t format,
    bool tiled);

void lv_vg_lite_buffer_from_draw_buf(vg_lite_buffer_t * buffer, const lv_draw_buf_t * draw_buf);

void lv_vg_lite_image_matrix(vg_lite_matrix_t * matrix, int32_t x, int32_t y, const lv_draw_image_dsc_t * dsc);

vg_lite_color_t lv_vg_lite_image_recolor(vg_lite_buffer_t * buffer, const lv_draw_image_dsc_t * dsc);

bool lv_vg_lite_buffer_open_image(vg_lite_buffer_t * buffer, lv_image_decoder_dsc_t * decoder_dsc, const void * src,
                                  bool no_cache, bool premultiply);

void lv_vg_lite_image_dsc_init(struct _lv_draw_vg_lite_unit_t * unit);

void lv_vg_lite_image_dsc_deinit(struct _lv_draw_vg_lite_unit_t * unit);

vg_lite_blend_t lv_vg_lite_blend_mode(lv_blend_mode_t blend_mode, bool has_pre_mul);

uint32_t lv_vg_lite_get_palette_size(vg_lite_buffer_format_t format);

vg_lite_color_t lv_vg_lite_color(lv_color_t color, lv_opa_t opa, bool pre_mul);

void lv_vg_lite_rect(vg_lite_rectangle_t * rect, const lv_area_t * area);

static inline void lv_vg_lite_matrix(vg_lite_matrix_t * dest, const lv_matrix_t * src)
{
    *(lv_matrix_t *)dest = *src;
}

/* Param checker */

bool lv_vg_lite_buffer_check(const vg_lite_buffer_t * buffer, bool is_src);

bool lv_vg_lite_path_check(const vg_lite_path_t * path);

bool lv_vg_lite_matrix_check(const vg_lite_matrix_t * matrix);

/* Wrapper */

bool lv_vg_lite_support_blend_normal(void);

bool lv_vg_lite_16px_align(void);

void lv_vg_lite_matrix_multiply(vg_lite_matrix_t * matrix, const vg_lite_matrix_t * mult);

bool lv_vg_lite_matrix_inverse(vg_lite_matrix_t * result, const vg_lite_matrix_t * matrix);

lv_point_precise_t lv_vg_lite_matrix_transform_point(const vg_lite_matrix_t * matrix, const lv_point_precise_t * point);

void lv_vg_lite_set_scissor_area(const lv_area_t * area);

void lv_vg_lite_disable_scissor(void);

void lv_vg_lite_flush(struct _lv_draw_vg_lite_unit_t * u);

void lv_vg_lite_finish(struct _lv_draw_vg_lite_unit_t * u);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*VG_LITE_UTILS_H*/
