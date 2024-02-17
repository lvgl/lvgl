/**
 * @file lv_draw_sdl.h
 *
 */

#ifndef LV_DRAW_SDL_H
#define LV_DRAW_SDL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"

#if LV_USE_DRAW_SDL

#include "../../misc/cache/lv_cache.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../display/lv_display.h"
#include "../../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
    uint32_t texture_cache_data_type;
    lv_cache_t * texture_cache;
} lv_draw_sdl_unit_t;

#if LV_DRAW_SW_SHADOW_CACHE_SIZE
typedef struct {
    uint8_t cache[LV_DRAW_SW_SHADOW_CACHE_SIZE * LV_DRAW_SW_SHADOW_CACHE_SIZE];
    int32_t cache_size;
    int32_t cache_r;
} lv_draw_sw_shadow_cache_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_sdl_init(void);

void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sdl_image(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                                   const lv_area_t * coords);

void lv_draw_sdl_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sdl_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sdl_box_shadow(lv_draw_unit_t * draw_unit, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sdl_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_sdl_arc(lv_draw_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sdl_line(lv_draw_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc);

void lv_draw_sdl_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

void lv_draw_sdl_triangle(lv_draw_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc);

void lv_draw_sdl_mask_rect(lv_draw_unit_t * draw_unit, const lv_draw_mask_rect_dsc_t * dsc, const lv_area_t * coords);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_H*/
