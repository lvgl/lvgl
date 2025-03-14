/**
 * @file lv_demo_smartwatch.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_H
#define LV_DEMO_SMARTWATCH_H

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
} arc_animation_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a smartwatch demo.
 */
void lv_demo_smartwatch(void);

void ui_animate_x(lv_obj_t * obj, int32_t x, int32_t duration, int32_t delay);
void ui_animate_x_from(lv_obj_t * obj, int32_t start, int32_t x, int32_t duration, int32_t delay);
void ui_animate_y(lv_obj_t * obj, int32_t y, int32_t duration, int32_t delay);
void ui_anim_opa(lv_obj_t * obj, lv_opa_t opa, int32_t duration, int32_t delay);

void animate_arc(lv_obj_t * obj, arc_animation_t animation, int32_t duration, int32_t delay);

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

#endif /*LV_DEMO_SMARTWATCH_H*/
