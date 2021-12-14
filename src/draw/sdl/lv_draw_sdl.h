/**
 * @file lv_draw_sdl.h
 *
 */

#ifndef LV_DRAW_SDL_H
#define LV_DRAW_SDL_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include LV_DRAW_SDL_INCLUDE_PATH

#include "../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define LV_DRAW_SDL_TEXTURE_FORMAT SDL_PIXELFORMAT_ARGB8888
#else
#define LV_DRAW_SDL_TEXTURE_FORMAT SDL_PIXELFORMAT_RGBA8888
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct lv_draw_sdl_context_internals_t;

typedef struct {
    lv_draw_sw_ctx_t base_draw;
    SDL_Renderer * renderer;
    struct lv_draw_sdl_context_internals_t * internals;
} lv_draw_sdl_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sdl_init_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx);

/**
 * @brief Free caches
 *
 */
void lv_draw_sdl_deinit_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx);

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

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_H*/
