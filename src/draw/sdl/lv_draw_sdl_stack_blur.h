/**
 * @file lv_draw_sdl_stack_blur.h
 *
 */
#ifndef LV_DRAW_SDL_STACK_BLUR_H
#define LV_DRAW_SDL_STACK_BLUR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#include "lvgl/src/misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_stack_blur_grayscale(lv_opa_t * buf, uint16_t w, uint16_t h, uint16_t r);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SDL_STACK_BLUR_H*/
