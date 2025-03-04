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

/**********************
 *      TYPEDEFS
 **********************/

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

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_PRIVATE_H*/
