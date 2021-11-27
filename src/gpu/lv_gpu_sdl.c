/**
 * @file lv_gpu_sdl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#include "../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_gpu_sdl.h"
#include "../draw/sdl/lv_draw_sdl_utils.h"
#include "../draw/sdl/lv_draw_sdl_texture_cache.h"

#include "../draw/sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/
void lv_draw_sdl_draw_rect(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc);

lv_res_t lv_draw_sdl_img_core(const lv_area_t * coords, const lv_area_t * mask, const void * src,
                              const lv_draw_img_dsc_t * draw_dsc);

void lv_draw_sdl_draw_letter(const lv_point_t * pos_p, const lv_area_t * clip_area,
                            const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa,
                            lv_blend_mode_t blend_mode);

void lv_draw_sdl_draw_blend_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                                lv_color_t color, lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode);

void lv_draw_sdl_draw_blend_map(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * clip_area,
                               const lv_color_t * src_buf, const lv_area_t * src_area,
                               lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode);
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

void lv_gpu_sdl_init()
{
    _lv_draw_sdl_utils_init();
    _lv_draw_sdl_texture_cache_init();
}

void lv_gpu_sdl_deinit()
{
    _lv_draw_sdl_texture_cache_deinit();
    _lv_draw_sdl_utils_deinit();
}

void lv_gpu_sdl_backend_init(lv_draw_backend_t * backend, SDL_Renderer * renderer, SDL_Texture * texture)
{
    lv_draw_backend_init(backend);
    lv_draw_sdl_backend_context_t * ctx = lv_mem_alloc(sizeof(lv_draw_sdl_backend_context_t));
    lv_memset_00(ctx, sizeof(lv_draw_sdl_backend_context_t));
    ctx->renderer = renderer;
    ctx->texture = texture;
    backend->ctx = ctx;
    backend->draw_rect = lv_draw_sdl_draw_rect;
    backend->draw_arc = lv_draw_sw_arc;
    backend->draw_img_core = lv_draw_sdl_img_core;
    backend->draw_letter = lv_draw_sdl_draw_letter;
    backend->draw_line = lv_draw_sw_line;
    backend->draw_polygon = lv_draw_sw_polygon;
    backend->blend_fill = lv_draw_sdl_draw_blend_fill;
    backend->blend_map = lv_draw_sdl_draw_blend_map;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif /*LV_USE_GPU_SDL*/
