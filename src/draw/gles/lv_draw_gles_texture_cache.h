/**
 * @file lv_draw_gles_texture_cache.h
 *
 */

#ifndef LV_DRAW_GLES_TEXTURE_CACHE_H
#define LV_DRAW_GLES_TEXTURE_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES

#include "lv_draw_gles.h"
#include "lv_draw_gles_priv.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_GPU_CACHE_KEY_MAGIC_ARC = 0x01,
    LV_GPU_CACHE_KEY_MAGIC_IMG = 0x11,
    LV_GPU_CACHE_KEY_MAGIC_IMG_ROUNDED_CORNERS = 0x12,
    LV_GPU_CACHE_KEY_MAGIC_LINE = 0x21,
    LV_GPU_CACHE_KEY_MAGIC_RECT_BG = 0x31,
    LV_GPU_CACHE_KEY_MAGIC_RECT_SHADOW = 0x32,
    LV_GPU_CACHE_KEY_MAGIC_RECT_BORDER = 0x33,
    LV_GPU_CACHE_KEY_MAGIC_FONT_GLYPH = 0x41,
    LV_GPU_CACHE_KEY_MAGIC_MASK = 0x51,
} lv_gles_cache_key_magic_t;

typedef struct {
    lv_gles_cache_key_magic_t magic;
    lv_img_src_t type;
    int32_t frame_id;
} lv_draw_gles_cache_key_head_img_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_gles_texture_cache_init(lv_draw_gles_ctx_t * ctx);

void lv_draw_gles_texture_cache_deinit(lv_draw_gles_ctx_t * ctx);

GLuint lv_draw_gles_texture_cache_get(lv_draw_gles_ctx_t * ctx,
                                      const void * key,
                                      size_t key_length,
                                      bool * found);

void lv_draw_gles_texture_cache_put(lv_draw_gles_ctx_t * ctx, const void * key, size_t key_length, GLuint texture);

lv_draw_gles_cache_key_head_img_t * lv_draw_gles_texture_img_key_create(const void * src, int32_t frame_id,
                                                                      size_t * size);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_GLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GLES_TEXTURE_CACHE_H*/