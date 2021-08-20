//
// Created by Mariotaku on 2021/08/21.
//

#include "misc/lv_style.h"
#include "lv_gpu_sdl2_utils.h"

void lv_area_to_sdl_rect(const lv_area_t *in, SDL_Rect *out) {
    out->x = in->x1;
    out->y = in->y1;
    out->w = in->x2 - in->x1 + 1;
    out->h = in->y2 - in->y1 + 1;
}

void lv_area_to_sdl_frect(const lv_area_t *in, SDL_FRect *out) {
    out->x = in->x1;
    out->y = in->y1;
    out->w = in->x2 - in->x1 + 1;
    out->h = in->y2 - in->y1 + 1;
}

void lv_color_to_sdl_color(const lv_color_t *in, SDL_Color *out) {
    uint32_t color32 = lv_color_to32(*in);
    lv_color32_t *color32_t = (lv_color32_t *) &color32;
    out->a = color32_t->ch.alpha;
    out->r = color32_t->ch.red;
    out->g = color32_t->ch.green;
    out->b = color32_t->ch.blue;
}

void lv_area_zoom_to_sdl_rect(const lv_area_t *in, SDL_Rect *out, uint16_t zoom, const lv_point_t *pivot) {
    if (zoom == LV_IMG_ZOOM_NONE) {
        lv_area_to_sdl_rect(in, out);
        return;
    }
    int w = in->x2 - in->x1 + 1, h = in->y2 - in->y1 + 1;
    int sw = w * zoom >> 8, sh = h * zoom >> 8;
    out->x = in->x1 - (sw / 2 - pivot->x);
    out->y = in->y1 - (sh / 2 - pivot->y);
    out->w = sw;
    out->h = sh;
}