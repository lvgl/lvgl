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

#include "../lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_draw_sdl_context_internals_t;

typedef struct lv_draw_sdl_context_t {
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    struct lv_draw_sdl_context_internals_t * internals;
} lv_draw_sdl_context_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sdl_init();

/**
 * @brief Free caches
 *
 */
void lv_draw_sdl_deinit();

void lv_draw_sdl_backend_init(lv_draw_backend_t *backend);

void lv_draw_sdl_context_init(lv_draw_sdl_context_t *context);

void lv_draw_sdl_context_deinit(lv_draw_sdl_context_t *context);

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
