/**
 * @file lv_draw_sdl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "lv_draw_sdl.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_layer.h"

/*********************
 *      DEFINES
 *********************/
void lv_draw_sdl_draw_rect(lv_layer_t * layer, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

lv_res_t lv_draw_sdl_img_core(lv_layer_t * layer, const lv_draw_img_dsc_t * draw_dsc,
                              const lv_area_t * coords, const void * src);

void lv_draw_sdl_draw_letter(lv_layer_t * layer, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                             uint32_t letter);

void lv_draw_sdl_draw_line(lv_layer_t * layer, const lv_draw_line_dsc_t * dsc, const lv_point_t * point1,
                           const lv_point_t * point2);

void lv_draw_sdl_draw_arc(lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                          uint16_t radius, uint16_t start_angle, uint16_t end_angle);

void lv_draw_sdl_polygon(lv_layer_t * layer, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t points[],
                         uint16_t point_cnt);

void lv_draw_sdl_draw_bg(lv_layer_t * layer, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

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

void lv_draw_sdl_init_ctx(lv_disp_t * disp_drv, lv_layer_t * layer)
{
    _lv_draw_sdl_utils_init();
    lv_memzero(layer, sizeof(lv_draw_sdl_ctx_t));
    layer->draw_rect = lv_draw_sdl_draw_rect;
    layer->draw_img = lv_draw_sdl_img_core;
    layer->draw_letter = lv_draw_sdl_draw_letter;
    layer->draw_line = lv_draw_sdl_draw_line;
    layer->draw_arc = lv_draw_sdl_draw_arc;
    layer->draw_polygon = lv_draw_sdl_polygon;
    layer->draw_bg = lv_draw_sdl_draw_bg;
    layer->layer_init = lv_draw_sdl_layer_init;
    layer->layer_blend = lv_draw_sdl_layer_blend;
    layer->layer_destroy = lv_draw_sdl_layer_destroy;
    layer->layer_instance_size = sizeof(lv_draw_sdl_layer_ctx_t);
    lv_draw_sdl_ctx_t * layer_sdl = (lv_draw_sdl_ctx_t *) layer;
    layer_sdl->renderer = ((lv_draw_sdl_drv_param_t *) disp_drv->user_data)->renderer;
    layer_sdl->internals = lv_malloc(sizeof(lv_draw_sdl_context_internals_t));
    lv_memzero(layer_sdl->internals, sizeof(lv_draw_sdl_context_internals_t));
    lv_draw_sdl_texture_cache_init(layer_sdl);
}

void lv_draw_sdl_deinit_ctx(lv_disp_t * disp_drv, lv_layer_t * layer)
{
    lv_draw_sdl_ctx_t * layer_sdl = (lv_draw_sdl_ctx_t *) layer;
    lv_draw_sdl_texture_cache_deinit(layer_sdl);
    lv_free(layer_sdl->internals);
    _lv_draw_sdl_utils_deinit();
}

SDL_Texture * lv_draw_sdl_create_screen_texture(SDL_Renderer * renderer, lv_coord_t hor, lv_coord_t ver)
{
    SDL_Texture * texture = SDL_CreateTexture(renderer, LV_DRAW_SDL_TEXTURE_FORMAT, SDL_TEXTUREACCESS_TARGET, hor, ver);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_SDL*/
