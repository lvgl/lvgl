//
// Created by Mariotaku on 2021/08/23.
//

#ifndef LVGL_SDL_EXAMPLE_LV_GPU_SDL2_MASK_H
#define LVGL_SDL_EXAMPLE_LV_GPU_SDL2_MASK_H

#include <SDL.h>

#include "misc/lv_area.h"
#include "misc/lv_color.h"

extern SDL_Palette *lv_sdl2_palette_grayscale8;

lv_opa_t *lv_draw_mask_dump(const lv_area_t *coords);

SDL_Surface *lv_sdl2_create_mask_surface(lv_opa_t *pixels, lv_coord_t width, lv_coord_t height);

SDL_Texture *lv_sdl2_create_mask_texture(SDL_Renderer *renderer, lv_opa_t *pixels,
                                         lv_coord_t width, lv_coord_t height);

SDL_Surface *lv_sdl2_apply_mask_surface(const lv_area_t *coords);

SDL_Texture *lv_sdl2_gen_mask_texture(SDL_Renderer *renderer, const lv_area_t *coords);

void lv_draw_mask_blur(lv_opa_t *buf, uint16_t w, uint16_t h, uint16_t r);

#endif //LVGL_SDL_EXAMPLE_LV_GPU_SDL2_MASK_H
