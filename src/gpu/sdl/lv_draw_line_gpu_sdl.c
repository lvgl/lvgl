/**
 * @file lv_gpu_sdl_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "SDL.h"
#include "../../core/lv_refr.h"
#include "lv_draw_gpu_sdl.h"
#include "lv_gpu_sdl_utils.h"
#include "lv_gpu_sdl_lru.h"
#include "lv_gpu_draw_cache.h"
#include "lv_gpu_sdl_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_coord_t length;
    lv_coord_t thickness;
} lv_draw_line_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_draw_line_key_t line_key_create(lv_coord_t length, lv_coord_t thickness);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_line_gpu_sdl(const lv_point_t *point1, const lv_point_t *point2, const lv_area_t *clip,
                          const lv_draw_line_dsc_t *dsc) {
    if (dsc->width == 0) return;
    if (dsc->opa <= LV_OPA_MIN) return;
    if (lv_draw_mask_get_cnt() > 0) return;

    if (point1->x == point2->x && point1->y == point2->y) return;

    lv_area_t clip_line;
    clip_line.x1 = LV_MIN(point1->x, point2->x) - dsc->width / 2;
    clip_line.x2 = LV_MAX(point1->x, point2->x) + dsc->width / 2;
    clip_line.y1 = LV_MIN(point1->y, point2->y) - dsc->width / 2;
    clip_line.y2 = LV_MAX(point1->y, point2->y) + dsc->width / 2;

    bool is_common;
    is_common = _lv_area_intersect(&clip_line, &clip_line, clip);
    if (!is_common) return;

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;

    SDL_Color line_color;
    lv_color_to_sdl_color(&dsc->color, &line_color);

    int length = lv_sdl_round(SDL_sqrt(SDL_pow(point2->y - point1->y + 1, 2) + SDL_pow(point2->x - point1->x + 1, 2)));
    lv_coord_t thickness = dsc->width;
    lv_draw_line_key_t key = line_key_create(length, thickness);
    lv_area_t coords = {1, 1, length, dsc->width};
    lv_area_t tex_coords;
    lv_area_copy(&tex_coords, &coords);
    lv_area_increase(&tex_coords, 1, 1);

    SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key), NULL);

    if (texture == NULL) {
        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, &coords, 0, false);
        int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
        texture = lv_sdl_gen_mask_texture(renderer, &tex_coords);
        lv_draw_mask_remove_id(mask_rout_id);
        SDL_assert(texture);
        lv_gpu_draw_cache_put(&key, sizeof(key), texture);
    }
    double angle = SDL_atan2(point2->y - point1->y, point2->x - point1->x) * 180 / M_PI;

    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(clip, &clip_rect);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, dsc->opa);
    SDL_SetTextureColorMod(texture, line_color.r, line_color.g, line_color.b);
    SDL_RenderSetClipRect(renderer, &clip_rect);

    SDL_Rect coords_rect;
    lv_area_to_sdl_rect(&tex_coords, &coords_rect);
    coords_rect.x = point1->x - coords_rect.h / 2;
    coords_rect.y = point1->y - coords_rect.h / 2;
    SDL_Point center = {coords_rect.h / 2, coords_rect.h / 2};
    SDL_RenderCopyEx(renderer, texture, NULL, &coords_rect, angle, &center, SDL_FLIP_NONE);

    if (dsc->round_end || dsc->round_start) {
        lv_draw_rect_dsc_t cir_dsc;
        lv_draw_rect_dsc_init(&cir_dsc);
        cir_dsc.bg_color = dsc->color;
        cir_dsc.radius = LV_RADIUS_CIRCLE;
        cir_dsc.bg_opa = dsc->opa;

        int32_t r = (dsc->width >> 1);
        int32_t r_corr = (dsc->width & 1) ? 0 : 1;
        lv_area_t cir_area;

        if (dsc->round_start) {
            cir_area.x1 = point1->x - r;
            cir_area.y1 = point1->y - r;
            cir_area.x2 = point1->x + r - r_corr;
            cir_area.y2 = point1->y + r - r_corr;
            lv_draw_rect(&cir_area, clip, &cir_dsc);
        }

        if (dsc->round_end) {
            cir_area.x1 = point2->x - r;
            cir_area.y1 = point2->y - r;
            cir_area.x2 = point2->x + r - r_corr;
            cir_area.y2 = point2->y + r - r_corr;
            lv_draw_rect(&cir_area, clip, &cir_dsc);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_draw_line_key_t line_key_create(lv_coord_t length, lv_coord_t thickness) {
    lv_draw_line_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_LINE;
    key.length = length;
    key.thickness = thickness;
    return key;
}


#endif /*LV_USE_GPU_SDL*/
