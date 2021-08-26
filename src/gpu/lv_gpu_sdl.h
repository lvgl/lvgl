//
// Created by Mariotaku on 2021/08/20.
//

#ifndef LV_GPU_H
#define LV_GPU_H

#include "hal/lv_hal_disp.h"
#include "misc/lv_color.h"
#include "misc/lv_area.h"
#include "misc/lv_style.h"
#include "font/lv_font.h"
#include "draw/lv_draw_mask.h"

#include <SDL.h>

lv_disp_t *lv_sdl_display_init(SDL_Window *window);

void lv_sdl_display_deinit(lv_disp_t *disp);

#endif //LV_GPU_H
