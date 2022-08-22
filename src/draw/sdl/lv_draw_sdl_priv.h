/**
 * @file lv_draw_sdl_priv.h
 *
 */

#ifndef LV_DRAW_SDL_PRIV_H
#define LV_DRAW_SDL_PRIV_H


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
#include "../../misc/lv_lru.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_draw_sdl_context_internals_t {
    lv_lru_t * texture_cache;
    SDL_Texture * mask;
    SDL_Texture * composition;
    SDL_Texture * target_backup;
    uint8_t transform_count;
} lv_draw_sdl_context_internals_t;

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

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_PRIV_H*/
