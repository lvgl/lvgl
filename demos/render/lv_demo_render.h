/**
 * @file lv_demo_render.h
 *
 */

#ifndef LV_DEMO_RENDER_H
#define LV_DEMO_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

#if LV_USE_DEMO_RENDER

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_DEMO_RENDER_SCENE_FILL,
    LV_DEMO_RENDER_SCENE_BORDER,
    LV_DEMO_RENDER_SCENE_BOX_SHADOW,
    LV_DEMO_RENDER_SCENE_TEXT,
    LV_DEMO_RENDER_SCENE_IMAGE_NORMAL,
    LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR,
    LV_DEMO_RENDER_SCENE_LINE,
    LV_DEMO_RENDER_SCENE_ARC_NORMAL,
    LV_DEMO_RENDER_SCENE_ARC_IMAGE,
    LV_DEMO_RENDER_SCENE_TRIANGLE,
    LV_DEMO_RENDER_SCENE_LAYER_NORMAL,
    LV_DEMO_RENDER_SCENE_BLEND_MODE,
    _LV_DEMO_RENDER_SCENE_NUM,
} lv_demo_render_scene_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Run the render verification for a scenario
 * @param id   ID of the scenario to run. Element of `lv_demo_render_scene_t`
 * @param opa  set this opacity for each object
 */
void lv_demo_render(lv_demo_render_scene_t id, lv_opa_t opa);

const char * lv_demo_render_get_scene_name(lv_demo_render_scene_t id);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_RENDER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_RENDER_H*/
