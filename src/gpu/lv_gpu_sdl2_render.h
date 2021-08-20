//
// Created by Mariotaku on 2021/08/20.
//

#ifndef LV_GPU_SDL2_RENDER_H
#define LV_GPU_SDL2_RENDER_H

#include "hal/lv_hal_disp.h"
#include "misc/lv_color.h"
#include "misc/lv_area.h"
#include "misc/lv_style.h"
#include "font/lv_font.h"
#include "draw/lv_draw_mask.h"

#include <SDL.h>

lv_res_t lv_gpu_sdl2_renderer_init();

void lv_gpu_sdl2_renderer_deinit();

void lv_gpu_sdl_render_fill_color(lv_disp_drv_t *drv, const lv_area_t *disp_area, const lv_area_t *draw_area,
                                  lv_color_t color, lv_opa_t opa, const lv_opa_t *mask, lv_draw_mask_res_t mask_res,
                                  lv_blend_mode_t mode);

void lv_gpu_sdl_render_draw_letter(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t *g,
                                   const lv_area_t *clip_area,
                                   const lv_font_t *font_p, uint32_t letter, lv_color_t color, lv_opa_t opa,
                                   lv_blend_mode_t blend_mode);

#endif //LV_GPU_SDL2_RENDER_H
