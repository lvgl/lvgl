/**
 * @file lv_draw_sdl_img.h
 *
 */

#ifndef LV_DRAW_SDL_IMG_H
#define LV_DRAW_SDL_IMG_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include LV_GPU_SDL_INCLUDE_PATH

#include "../lv_draw.h"

#include "lv_draw_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_draw_sdl_img_header_t {
    lv_img_header_t base;
    SDL_Rect rect;
    bool managed;
} lv_draw_sdl_img_header_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/
/**
 *
 * @param ctx Drawing context
 * @param key Texture cache key
 * @param key_size Size of texture cache key
 * @param src Image source pointer
 * @param frame_id Frame ID for animated image
 * @param texture Texture for render
 * @param header Header also holds sdl image info
 * @param texture_in_cache Whether the texture has been put in the cache. Please note for managed texture,
 * this will be false too. So you'll need to check header.managed too.
 * @return Whether the image has been loaded successfully
 */
bool lv_draw_sdl_img_load_texture(lv_draw_sdl_ctx_t * ctx, lv_draw_sdl_cache_key_head_img_t * key, size_t key_size,
                                  const void * src, int32_t frame_id, SDL_Texture ** texture,
                                  lv_draw_sdl_img_header_t ** header, bool * texture_in_cache);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_IMG_H*/
