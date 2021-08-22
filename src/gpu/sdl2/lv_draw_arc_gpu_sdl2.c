//
// Created by Mariotaku on 2021/08/21.
//
#include <hal/lv_hal_disp.h>
#include <core/lv_refr.h>
#include "draw/lv_draw_arc.h"
#include "lv_gpu_sdl2_utils.h"
#include "lv_gpu_sdl2_lru.h"
#include "lv_gpu_draw_cache.h"

typedef struct {
    uint16_t radius;
    uint16_t angle;
    lv_coord_t width;
    uint8_t rounded;
} lv_draw_arc_key_t;

void lv_draw_arc(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius, uint16_t start_angle, uint16_t end_angle,
                 const lv_area_t *clip_area, const lv_draw_arc_dsc_t *dsc) {
    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    lv_disp_drv_t *driver = disp->driver;
    SDL_Renderer *renderer = (SDL_Renderer *) driver->draw_buf->buf_act;

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

    SDL_Rect area_out_rect, clip_rect;
    lv_area_to_sdl_rect(&area_out, &area_out_rect);
    lv_area_to_sdl_rect(clip_area, &clip_rect);

    lv_draw_arc_key_t key = {.radius = radius, .angle = ((end_angle - start_angle) % 360 + 360) %
                                                        360, .width = dsc->width,
            .rounded = dsc->rounded};
    SDL_Texture *texture = NULL;
    lv_lru_get(lv_sdl2_texture_cache, &key, sizeof(key), (void **) &texture);
    if (texture == NULL) {
        /*Create inner the mask*/
        lv_draw_mask_radius_param_t mask_in_param;
        lv_draw_mask_radius_init(&mask_in_param, &area_in, LV_RADIUS_CIRCLE, true);
        int16_t mask_in_id = lv_draw_mask_add(&mask_in_param, NULL);

        lv_draw_mask_radius_param_t mask_out_param;
        lv_draw_mask_radius_init(&mask_out_param, &area_out, LV_RADIUS_CIRCLE, false);
        int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, NULL);

        SDL_Surface *arg_mask;
        if (key.angle < 360) {
            while (start_angle >= 360) start_angle -= 360;
            while (end_angle >= 360) end_angle -= 360;
            lv_draw_mask_angle_param_t mask_angle_param;
            lv_draw_mask_angle_init(&mask_angle_param, center_x, center_y, 0, key.angle);
            int16_t mask_angle_id = lv_draw_mask_add(&mask_angle_param, NULL);
            arg_mask = lv_sdl2_apply_mask_surface(&area_out);
            lv_draw_mask_remove_id(mask_angle_id);
        } else {
            arg_mask = lv_sdl2_apply_mask_surface(&area_out);
        }
        lv_draw_mask_remove_id(mask_out_id);
        lv_draw_mask_remove_id(mask_in_id);

        if (dsc->rounded) {
            SDL_Renderer *mask_renderer = SDL_CreateSoftwareRenderer(arg_mask);
            lv_area_t cap_area = {.x1 = 0, .y1 = 0};
            lv_area_set_width(&cap_area, dsc->width);
            lv_area_set_height(&cap_area, dsc->width);

            lv_draw_mask_radius_param_t mask_rout_param;
            lv_draw_mask_radius_init(&mask_rout_param, &cap_area, LV_RADIUS_CIRCLE, false);
            int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
            SDL_Texture *round_texture = lv_sdl2_gen_mask_texture(mask_renderer, &cap_area);
            lv_draw_mask_remove_id(mask_rout_id);

            SDL_SetTextureBlendMode(round_texture, SDL_BLENDMODE_BLEND);
            float mid_point = radius - key.width / 2.0f;
            SDL_FRect cap_dst;
            cap_dst.w = lv_area_get_width(&cap_area);
            cap_dst.h = lv_area_get_height(&cap_area);
            cap_dst.x = mid_point + SDL_round(SDL_cos(0) * mid_point);
            cap_dst.y = mid_point + SDL_round(SDL_sin(0) * mid_point);
            SDL_RenderCopyF(mask_renderer, round_texture, NULL, &cap_dst);
            cap_dst.x = mid_point + SDL_round(SDL_cos(key.angle * M_PI / 180.0f) * mid_point);
            cap_dst.y = mid_point + SDL_round(SDL_sin(key.angle * M_PI / 180.0f) * mid_point);
            SDL_RenderCopyF(mask_renderer, round_texture, NULL, &cap_dst);
            SDL_DestroyTexture(round_texture);
            SDL_DestroyRenderer(mask_renderer);
        }
        texture = SDL_CreateTextureFromSurface(renderer, arg_mask);
        SDL_FreeSurface(arg_mask);

        SDL_assert(texture);
        lv_lru_set(lv_sdl2_texture_cache, &key, sizeof(key), texture, radius * radius * 4);
    }

    SDL_Color arc_color;
    lv_color_to_sdl_color(&dsc->color, &arc_color);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, dsc->opa);
    SDL_SetTextureColorMod(texture, arc_color.r, arc_color.g, arc_color.b);
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderCopyEx(renderer, texture, NULL, &area_out_rect, start_angle, NULL, SDL_FLIP_NONE);
}