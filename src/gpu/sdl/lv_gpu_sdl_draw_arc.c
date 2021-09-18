/**
 * @file lv_gpu_sdl_draw_arc.c
 *
 * This implementation does not functioning properly so it's not enabled
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../hal/lv_hal_disp.h"
#include "../../core/lv_refr.h"
#include "../../draw/lv_draw_arc.h"
#include "lv_gpu_sdl_utils.h"
#include "lv_gpu_sdl_lru.h"
#include "lv_gpu_sdl_texture_cache.h"
#include "lv_gpu_sdl_mask.h"

#ifndef M_PI
    #include <math.h>
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    uint16_t radius;
    uint16_t angle;
    lv_coord_t width;
    uint8_t rounded;
} lv_draw_arc_key_t;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_arc2(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius, uint16_t start_angle, uint16_t end_angle,
                  const lv_area_t * clip_area, const lv_draw_arc_dsc_t * dsc)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_disp_drv_t * driver = disp->driver;
    SDL_Renderer * renderer = (SDL_Renderer *) driver->user_data;

    lv_area_t area_out;
    area_out.x1 = center_x - radius;
    area_out.y1 = center_y - radius;
    area_out.x2 = center_x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
    area_out.y2 = center_y + radius - 1;

    lv_area_t area_in;
    lv_area_copy(&area_in, &area_out);
    area_in.x1 += dsc->width;
    area_in.y1 += dsc->width;
    area_in.x2 -= dsc->width;
    area_in.y2 -= dsc->width;

    /*Increase 1 px each side to texture, to have better rotation result*/
    lv_area_t texture_area_out;
    lv_area_copy(&texture_area_out, &area_out);
    lv_area_increase(&texture_area_out, 1, 1);

    SDL_Rect area_out_rect, clip_rect;
    lv_area_to_sdl_rect(&texture_area_out, &area_out_rect);
    lv_area_to_sdl_rect(clip_area, &clip_rect);

    lv_draw_arc_key_t key = {
        .magic = LV_GPU_CACHE_KEY_MAGIC_ARC,
        .radius = radius,
        .angle = ((end_angle - start_angle) % 360 + 360) % 360,
        .width = dsc->width,
        .rounded = dsc->rounded,
    };
    //    SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key));
    SDL_Texture * texture = NULL;
    if(texture == NULL) {
        /*Create inner the mask*/
        lv_draw_mask_radius_param_t mask_in_param;
        lv_draw_mask_radius_init(&mask_in_param, &area_in, LV_RADIUS_CIRCLE, true);
        int16_t mask_in_id = lv_draw_mask_add(&mask_in_param, NULL);

        lv_draw_mask_radius_param_t mask_out_param;
        lv_draw_mask_radius_init(&mask_out_param, &area_out, LV_RADIUS_CIRCLE, false);
        int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, NULL);

        SDL_Surface * ark_mask;
        if(key.angle < 360) {
            while(start_angle >= 360) start_angle -= 360;
            while(end_angle >= 360) end_angle -= 360;
            lv_draw_mask_angle_param_t mask_angle_param;
            lv_draw_mask_angle_init(&mask_angle_param, center_x, center_y, 0, key.angle);
            int16_t mask_angle_id = lv_draw_mask_add(&mask_angle_param, NULL);
            ark_mask = lv_sdl_apply_mask_surface(&texture_area_out, NULL, 0);
            lv_draw_mask_remove_id(mask_angle_id);
        }
        else {
            ark_mask = lv_sdl_apply_mask_surface(&texture_area_out, NULL, 0);
        }
        lv_draw_mask_remove_id(mask_out_id);
        lv_draw_mask_remove_id(mask_in_id);

        if(dsc->rounded) {
            SDL_Renderer * mask_renderer = SDL_CreateSoftwareRenderer(ark_mask);
            lv_area_t cap_area = {.x1 = 0, .y1 = 0};
            lv_area_set_width(&cap_area, dsc->width);
            lv_area_set_height(&cap_area, dsc->width);

            lv_draw_mask_radius_param_t mask_rout_param;
            lv_draw_mask_radius_init(&mask_rout_param, &cap_area, LV_RADIUS_CIRCLE, false);
            int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
            SDL_Texture * round_texture = lv_sdl_gen_mask_texture(mask_renderer, &cap_area, &mask_rout_id, 1);
            lv_draw_mask_remove_id(mask_rout_id);

            SDL_SetTextureBlendMode(round_texture, SDL_BLENDMODE_BLEND);
            float mid_point = radius - key.width / 2.0f;
            SDL_Rect cap_dst;
            cap_dst.w = lv_area_get_width(&cap_area);
            cap_dst.h = lv_area_get_height(&cap_area);
            cap_dst.x = mid_point + lv_sdl_round(SDL_cos(0) * mid_point);
            cap_dst.y = mid_point + lv_sdl_round(SDL_sin(0) * mid_point);
            SDL_RenderCopy(mask_renderer, round_texture, NULL, &cap_dst);
            cap_dst.x = mid_point + lv_sdl_round(SDL_cos(key.angle * M_PI / 180.0f) * mid_point);
            cap_dst.y = mid_point + lv_sdl_round(SDL_sin(key.angle * M_PI / 180.0f) * mid_point);
            SDL_RenderCopy(mask_renderer, round_texture, NULL, &cap_dst);
            SDL_DestroyTexture(round_texture);
            SDL_DestroyRenderer(mask_renderer);
        }
        texture = SDL_CreateTextureFromSurface(renderer, ark_mask);
        SDL_FreeSurface(ark_mask);

        SDL_assert(texture);
        //        lv_gpu_draw_cache_put(&key, sizeof(key), texture);
    }

    SDL_Color arc_color;
    lv_color_to_sdl_color(&dsc->color, &arc_color);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, dsc->opa);
    SDL_SetTextureColorMod(texture, arc_color.r, arc_color.g, arc_color.b);
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderCopyEx(renderer, texture, NULL, &area_out_rect, start_angle, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(texture);
}

#endif /*LV_USE_GPU_SDL*/
