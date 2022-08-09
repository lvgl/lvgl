/**
 * @file lv_draw_sdl_mask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "../../misc/lv_gc.h"
#include "lv_draw_sdl_mask.h"
#include "lv_draw_sdl_utils.h"

/*********************
 *      DEFINES
 *********************/
#ifndef HAVE_SDL_CUSTOM_BLEND_MODE
    #define HAVE_SDL_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_opa_t * lv_draw_sdl_mask_dump_opa(const lv_area_t * coords, const int16_t * ids, int16_t ids_count)
{
    SDL_assert(coords->x2 >= coords->x1);
    SDL_assert(coords->y2 >= coords->y1);
    lv_coord_t w = lv_area_get_width(coords), h = lv_area_get_height(coords);
    lv_opa_t * mask_buf = lv_malloc(w * h);
    for(lv_coord_t y = 0; y < h; y++) {
        lv_opa_t * line_buf = &mask_buf[y * w];
        lv_memset(line_buf, 0xff, w);
        lv_coord_t abs_x = (lv_coord_t) coords->x1, abs_y = (lv_coord_t)(y + coords->y1), len = (lv_coord_t) w;
        lv_draw_mask_res_t res;
        if(ids) {
            res = lv_draw_mask_apply_ids(line_buf, abs_x, abs_y, len, ids, ids_count);
        }
        else {
            res = lv_draw_mask_apply(line_buf, abs_x, abs_y, len);
        }
        if(res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memzero(line_buf, w);
        }
    }
    return mask_buf;
}

SDL_Texture * lv_draw_sdl_mask_dump_texture(SDL_Renderer * renderer, const lv_area_t * coords, const int16_t * ids,
                                            int16_t ids_count)
{
    lv_coord_t w = lv_area_get_width(coords), h = lv_area_get_height(coords);
    lv_opa_t * mask_buf = lv_draw_sdl_mask_dump_opa(coords, ids, ids_count);
    SDL_Surface * surface = lv_sdl_create_opa_surface(mask_buf, w, h, w);
    lv_free(mask_buf);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_SDL*/
