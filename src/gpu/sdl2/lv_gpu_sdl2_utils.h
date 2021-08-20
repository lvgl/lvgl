//
// Created by Mariotaku on 2021/08/21.
//

#ifndef LV_GPU_SDL2_UTILS_H
#define LV_GPU_SDL2_UTILS_H

#include "misc/lv_color.h"
#include "misc/lv_area.h"
#include "SDL_rect.h"

void lv_area_to_sdl_rect(const lv_area_t *in, SDL_Rect *out);

void lv_area_to_sdl_frect(const lv_area_t *in, SDL_FRect *out);

void lv_color_to_sdl_color(const lv_color_t *in, SDL_Color *out);

void lv_area_zoom_to_sdl_rect(const lv_area_t *in, SDL_Rect *out,uint16_t zoom, const lv_point_t *pivot);

#endif //LV_GPU_SDL2_UTILS_H
