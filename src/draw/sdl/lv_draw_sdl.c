/**
 * @file lv_draw_sdl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_draw_sdl.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_layer.h"

/*********************
 *      DEFINES
 *********************/
void lv_draw_sdl_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

lv_res_t lv_draw_sdl_img_core(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                              const lv_area_t * coords, const void * src);

void lv_draw_sdl_draw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                             uint32_t letter);

void lv_draw_sdl_draw_line(lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc, const lv_point_t * point1,
                           const lv_point_t * point2);

void lv_draw_sdl_draw_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                          uint16_t radius, uint16_t start_angle, uint16_t end_angle);

void lv_draw_sdl_polygon(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t * points,
                         uint16_t point_cnt);

void lv_draw_sdl_draw_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

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

void lv_draw_sdl_init_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)
{
    _lv_draw_sdl_utils_init();
    lv_memset_00(draw_ctx, sizeof(lv_draw_sdl_ctx_t));
    draw_ctx->draw_rect = lv_draw_sdl_draw_rect;
    draw_ctx->draw_img = lv_draw_sdl_img_core;
    draw_ctx->draw_letter = lv_draw_sdl_draw_letter;
    draw_ctx->draw_line = lv_draw_sdl_draw_line;
    draw_ctx->draw_arc = lv_draw_sdl_draw_arc;
    draw_ctx->draw_polygon = lv_draw_sdl_polygon;
    draw_ctx->draw_bg = lv_draw_sdl_draw_bg;
    draw_ctx->layer_init = lv_draw_sdl_layer_init;
    draw_ctx->layer_blend = lv_draw_sdl_layer_blend;
    draw_ctx->layer_destroy = lv_draw_sdl_layer_destroy;
    draw_ctx->layer_instance_size = sizeof(lv_draw_sdl_layer_ctx_t);
    lv_draw_sdl_ctx_t * draw_ctx_sdl = (lv_draw_sdl_ctx_t *) draw_ctx;
    draw_ctx_sdl->renderer = ((lv_draw_sdl_drv_param_t *) disp_drv->user_data)->renderer;
    draw_ctx_sdl->internals = lv_mem_alloc(sizeof(lv_draw_sdl_context_internals_t));
    lv_memset_00(draw_ctx_sdl->internals, sizeof(lv_draw_sdl_context_internals_t));
    lv_draw_sdl_texture_cache_init(draw_ctx_sdl);
}

void lv_draw_sdl_deinit_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sdl_ctx_t * draw_ctx_sdl = (lv_draw_sdl_ctx_t *) draw_ctx;
    lv_draw_sdl_texture_cache_deinit(draw_ctx_sdl);
    lv_mem_free(draw_ctx_sdl->internals);
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

#endif /*LV_USE_GPU_SDL*/
