/**
 * @file lv_gpu_sdl_texture_cache.h
 *
 */

#ifndef LV_GPU_SDL_TEXTURE_CACHE_H
#define LV_GPU_SDL_TEXTURE_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#include LV_GPU_SDL_INCLUDE_PATH
#include "../../draw/lv_img_decoder.h"
#include "../../misc/lv_area.h"
#include "lv_gpu_sdl_lru.h"

/*********************
 *      DEFINES
 *********************/

#define LV_GPU_SDL_DEC_DSC_TEXTURE_HEAD "@LVSDLTex"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    char head[8];
    SDL_Texture * texture;
} lv_gpu_sdl_dec_dsc_userdata_t;

typedef enum {
    LV_GPU_CACHE_KEY_MAGIC_ARC = 0x01,
    LV_GPU_CACHE_KEY_MAGIC_IMG = 0x11,
    LV_GPU_CACHE_KEY_MAGIC_LINE = 0x21,
    LV_GPU_CACHE_KEY_MAGIC_RECT_BG = 0x31,
    LV_GPU_CACHE_KEY_MAGIC_RECT_SHADOW = 0x32,
    LV_GPU_CACHE_KEY_MAGIC_RECT_BORDER = 0x33,
    LV_GPU_CACHE_KEY_MAGIC_FONT = 0x41,
    LV_GPU_CACHE_KEY_TEMP = 0xFF,
} lv_gpu_cache_key_magic_t;

typedef enum {
    LV_GPU_SDL_CACHE_FLAG_NONE = 0,
    LV_GPU_SDL_CACHE_FLAG_MANAGED = 1,
} lv_gpu_sdl_cache_flag_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_img_src_t type;
    int32_t frame_id;
} lv_gpu_sdl_cache_key_head_img_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _lv_gpu_sdl_texture_cache_init();

void _lv_gpu_sdl_texture_cache_deinit();

SDL_Texture * lv_gpu_draw_cache_get(const void * key, size_t key_length, bool * found);

SDL_Texture * lv_gpu_draw_cache_get_with_userdata(const void * key, size_t key_length, bool * found, void ** userdata);

void lv_gpu_draw_cache_put(const void * key, size_t key_length, SDL_Texture * texture);

void lv_gpu_draw_cache_put_advanced(const void * key, size_t key_length, SDL_Texture * texture, void * userdata,
                                    lv_lru_free_t userdata_free, lv_gpu_sdl_cache_flag_t flags);

SDL_Texture * lv_gpu_temp_texture_obtain(SDL_Renderer * renderer, lv_coord_t width, lv_coord_t height);

lv_gpu_sdl_cache_key_head_img_t * lv_gpu_sdl_img_cache_key_create(const void * src, int32_t frame_id, size_t * size);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_SDL_TEXTURE_CACHE_H*/
