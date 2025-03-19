/**
 * @file lv_demo_smartwatch_private.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_PRIVATE_H
#define LV_DEMO_SMARTWATCH_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"


#if LV_USE_DEMO_SMARTWATCH


/*********************
 *      DEFINES
 *********************/

#define SCREEN_SIZE 384
#define TRANSITION_GAP 50

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    ARC_SHRINK_DOWN,
    ARC_EXPAND_UP,
    ARC_SHRINK_LEFT,
    ARC_EXPAND_RIGHT,
} lv_smartwatch_arc_animation_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_demo_smartwatch_control_create(void);
lv_obj_t * lv_demo_smartwatch_get_control_screen(void);

void lv_demo_smartwatch_health_create(void);
lv_obj_t * lv_demo_smartwatch_get_health_screen(void);

void lv_demo_smartwatch_music_create(void);
lv_obj_t * lv_demo_smartwatch_get_music_screen(void);

void lv_demo_smartwatch_sports_create(void);
lv_obj_t * lv_demo_smartwatch_get_sports_screen(void);

void lv_demo_smartwatch_weather_create(void);
lv_obj_t * lv_demo_smartwatch_get_weather_screen(void);

void lv_smartwatch_animate_x(lv_obj_t * obj, int32_t x, int32_t duration, int32_t delay);

void lv_smartwatch_animate_x_from(lv_obj_t * obj, int32_t start, int32_t x, int32_t duration, int32_t delay);

void lv_smartwatch_animate_y(lv_obj_t * obj, int32_t y, int32_t duration, int32_t delay);

void lv_smartwatch_anim_opa(lv_obj_t * obj, lv_opa_t opa, int32_t duration, int32_t delay);

void lv_smartwatch_animate_arc(lv_obj_t * obj, lv_smartwatch_arc_animation_t animation, int32_t duration,
                               int32_t delay);


/**********************
 * GLOBAL VARIABLES
 **********************/

extern lv_obj_t * arc_cont;
extern lv_obj_t * main_arc;
extern lv_obj_t * overlay;

/**********************
 *      MACROS
 **********************/

#define ARC_POS(i) ((i) * 30 - 15)

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_PRIVATE_H*/
