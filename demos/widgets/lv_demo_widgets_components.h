/**
 * @file lv_demo_widgets_components.h
 *
 */

#ifndef LV_DEMO_WIDGETS_COMPONENTS_H
#define LV_DEMO_WIDGETS_COMPONENTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"

#if LV_USE_DEMO_WIDGETS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_demo_widgets_components_init(void);

lv_obj_t * lv_demo_widgets_title_create(lv_obj_t * parent, const char * text);

/**********************
 * GLOBAL VARIABLES
 **********************/

extern disp_size_t disp_size;

extern lv_style_t style_title;
extern lv_style_t style_text_muted;
extern lv_style_t style_icon;
extern lv_style_t style_bullet;

extern const lv_font_t * font_large;
extern const lv_font_t * font_normal;

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DEMO_BENCHMARK */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_WIDGETS_COMPONENTS_H*/
