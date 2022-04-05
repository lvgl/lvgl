/**
 * @file lv_gif.h
 *
 */

#ifndef LV_GIF_H
#define LV_GIF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"
#if LV_USE_GIF

#include "gifdec.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_GIF_LOOP_DEFAULT, // play gif as many times as is defined in the actual file
    LV_GIF_LOOP_SINGLE, // play only one loop
    LV_GIF_LOOP_ON // loop indefinitely
} lv_gif_loop_t;

typedef struct {
    lv_img_t img;
    gd_GIF * gif;
    lv_timer_t * timer;
    lv_img_dsc_t imgdsc;
    uint32_t last_call;
    lv_gif_loop_t loop;
} lv_gif_t;

extern const lv_obj_class_t lv_gif_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_gif_create(lv_obj_t * parent);
void lv_gif_set_src(lv_obj_t * obj, const void * src);
void lv_gif_restart(lv_obj_t * gif);
void lv_gif_start(lv_obj_t * gif);
void lv_gif_stop(lv_obj_t * gif);
void lv_gif_set_loop(lv_obj_t * gif, lv_gif_loop_t loop);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GIF*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GIF_H*/
