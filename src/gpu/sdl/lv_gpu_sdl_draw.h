//
// Created by Mariotaku on 2021/08/23.
//

#ifndef LVGL_SDL_EXAMPLE_LV_DRAW_GPU_SDL2_H
#define LVGL_SDL_EXAMPLE_LV_DRAW_GPU_SDL2_H
#include "../../draw/lv_draw_line.h"

void lv_draw_line_gpu_sdl(const lv_point_t *point1, const lv_point_t *point2, const lv_area_t *clip,
                          const lv_draw_line_dsc_t *dsc);

#endif //LVGL_SDL_EXAMPLE_LV_DRAW_GPU_SDL2_H
