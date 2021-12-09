/**
 * @file lv_draw_sdl_texture_cache.h
 *
 */

#ifndef LV_DRAW_SDL_TEXTURE_CACHE_H
#define LV_DRAW_SDL_TEXTURE_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include LV_DRAW_SDL_INCLUDE_PATH
#include "lv_draw_sdl.h"
#include "lv_draw_sdl_priv.h"
#include "../../draw/lv_img_decoder.h"
#include "../../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

#define LV_DRAW_SDL_DEC_DSC_TEXTURE_HEAD "@LVSDLTex"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    char head[8];
    SDL_Texture * texture;
    bool texture_managed;
    bool texture_referenced;
} lv_draw_sdl_dec_dsc_userdata_t;

typedef enum {
    LV_GPU_CACHE_KEY_MAGIC_ARC = 0x01,
    LV_GPU_CACHE_KEY_MAGIC_IMG = 0x11,
    LV_GPU_CACHE_KEY_MAGIC_LINE = 0x21,
    LV_GPU_CACHE_KEY_MAGIC_RECT_BG = 0x31,
    LV_GPU_CACHE_KEY_MAGIC_RECT_SHADOW = 0x32,
    LV_GPU_CACHE_KEY_MAGIC_RECT_BORDER = 0x33,
    LV_GPU_CACHE_KEY_MAGIC_FONT_GLYPH = 0x41,
    LV_GPU_CACHE_KEY_MAGIC_MASK = 0x51,
} lv_sdl_cache_key_magic_t;

typedef enum {
    LV_DRAW_SDL_CACHE_FLAG_NONE = 0,
    LV_DRAW_SDL_CACHE_FLAG_MANAGED = 1,
} lv_draw_sdl_cache_flag_t;

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_img_src_t type;
    int32_t frame_id;
} lv_draw_sdl_cache_key_head_img_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sdl_texture_cache_init(struct lv_draw_sdl_context_internals_t *internals);

void lv_draw_sdl_texture_cache_deinit(struct lv_draw_sdl_context_internals_t *internals);

/**
 * Find cached texture by key. The texture can be destroyed during usage.
 */
SDL_Texture * lv_draw_sdl_texture_cache_get(const void * key, size_t key_length, bool * found);

SDL_Texture * lv_draw_sdl_texture_cache_get_with_userdata(const void * key, size_t key_length, bool * found,
                                                          void ** userdata);

/**
 * Find cached texture by key. The texture will be marked so it won't be destroyed.
 */
SDL_Texture * lv_draw_sdl_texture_cache_take(const void * key, size_t key_length, bool * found);

SDL_Texture * lv_draw_sdl_texture_cache_take_with_userdata(const void * key, size_t key_length, bool * found,
                                                          void ** userdata);

void lv_draw_sdl_texture_cache_put(const void * key, size_t key_length, SDL_Texture * texture);

void lv_draw_sdl_texture_cache_put_advanced(const void * key, size_t key_length, SDL_Texture * texture, void * userdata,
                                            void userdata_free(void*), lv_draw_sdl_cache_flag_t flags);

lv_draw_sdl_cache_key_head_img_t * lv_draw_sdl_texture_img_key_create(const void * src, int32_t frame_id, size_t * size);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_DRAW_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_TEXTURE_CACHE_H*/
