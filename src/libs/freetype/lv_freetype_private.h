/**
 * @file lv_freetype_private.h
 *
 */

#ifndef LV_FREETYPE_PRIVATE_H
#define LV_FREETYPE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_freetype.h"

#if LV_USE_FREETYPE

#include "../../misc/cache/lv_cache.h"
#include "../../misc/lv_ll.h"
#include "../../font/lv_font.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H
#include FT_SIZES_H
#include FT_IMAGE_H
#include FT_OUTLINE_H
#include FT_STROKER_H

/*********************
 *      DEFINES
 *********************/

#ifdef FT_CONFIG_OPTION_ERROR_STRINGS
#define FT_ERROR_MSG(msg, error_code) \
    LV_LOG_ERROR(msg " error(0x%x): %s", (int)error_code, FT_Error_String(error_code))
#else
#define FT_ERROR_MSG(msg, error_code) \
    LV_LOG_ERROR(msg " error(0x%x)", (int)error_code)
#endif

#define LV_FREETYPE_FONT_DSC_MAGIC_NUM 0x5F5F4654 /* '__FT' */
#define LV_FREETYPE_FONT_DSC_HAS_MAGIC_NUM(dsc) ((dsc)->magic_num == LV_FREETYPE_FONT_DSC_MAGIC_NUM)
#define LV_ASSERT_FREETYPE_FONT_DSC(dsc)                                                   \
    do {                                                                                   \
        LV_ASSERT_NULL(dsc);                                                               \
        LV_ASSERT_FORMAT_MSG(LV_FREETYPE_FONT_DSC_HAS_MAGIC_NUM(dsc),                      \
                             "Invalid font descriptor: 0x%" LV_PRIx32, (dsc)->magic_num);  \
    } while (0)

#define FT_INT_TO_F26DOT6(x) ((x) << 6)
#define FT_F26DOT6_TO_INT(x) ((x) >> 6)

#define FT_INT_TO_F16DOT16(x) ((x) << 16)
#define FT_F16DOT16_TO_INT(x) ((x) >> 16)

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_freetype_outline_vector_t {
    int32_t x;
    int32_t y;
};

typedef struct {
    int32_t segments_size;
    int32_t data_size;
} lv_freetype_outline_sizes_t;

struct _lv_freetype_outline_event_param_t {
    lv_freetype_outline_t outline;
    lv_freetype_outline_type_t type;
    lv_freetype_outline_vector_t to;
    lv_freetype_outline_vector_t control1;
    lv_freetype_outline_vector_t control2;
    lv_freetype_outline_sizes_t sizes;
};


typedef struct _lv_freetype_cache_node_t lv_freetype_cache_node_t;

struct _lv_freetype_cache_node_t {
    const char * pathname;
    lv_freetype_font_style_t style;
    lv_freetype_font_render_mode_t render_mode;

    uint32_t ref_size;                  /**< Reference size for calculating outline glyph's real size.*/

    FT_Face face;
    lv_mutex_t face_lock;
    bool face_has_kerning;

    /*glyph cache*/
    lv_cache_t * glyph_cache;

    /*draw data cache*/
    lv_cache_t * draw_data_cache;
};

typedef struct _lv_freetype_context_t {
    FT_Library library;
    lv_ll_t face_id_ll;
    lv_event_cb_t event_cb;

    uint32_t max_glyph_cnt;

    lv_cache_t * cache_node_cache;
} lv_freetype_context_t;

typedef struct _lv_freetype_font_dsc_t {
    uint32_t magic_num;
    lv_font_t font;
    uint32_t size;
    lv_freetype_font_style_t style;
    lv_freetype_font_render_mode_t render_mode;
    lv_freetype_context_t * context;
    lv_freetype_cache_node_t * cache_node;
    lv_cache_entry_t * cache_node_entry;
    FTC_FaceID face_id;
    uint32_t outline_stroke_width;
    lv_font_kerning_t kerning;
} lv_freetype_font_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the FreeType context.
 *
 * @return A pointer to the FreeType context used by LittlevGL.
 */
lv_freetype_context_t * lv_freetype_get_context(void);

void lv_freetype_italic_transform(FT_Face face);
int32_t lv_freetype_italic_transform_on_pos(lv_point_t point);

lv_cache_t * lv_freetype_create_glyph_cache(uint32_t cache_size);
void lv_freetype_set_cbs_glyph(lv_freetype_font_dsc_t * dsc);

lv_cache_t * lv_freetype_create_draw_data_image(uint32_t cache_size);
void lv_freetype_set_cbs_image_font(lv_freetype_font_dsc_t * dsc);

lv_cache_t * lv_freetype_create_draw_data_outline(uint32_t cache_size);
void lv_freetype_set_cbs_outline_font(lv_freetype_font_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FREETYPE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FREETYPE_PRIVATE_H*/
