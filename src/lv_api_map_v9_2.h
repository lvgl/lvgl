/**
 * @file lv_api_map_v9_2.h
 *
 */

#ifndef LV_API_MAP_V9_2_H
#define LV_API_MAP_V9_2_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#define LV_SCR_LOAD_ANIM_NONE          LV_SCREEN_LOAD_ANIM_NONE
#define LV_SCR_LOAD_ANIM_OVER_LEFT     LV_SCREEN_LOAD_ANIM_OVER_LEFT
#define LV_SCR_LOAD_ANIM_OVER_RIGHT    LV_SCREEN_LOAD_ANIM_OVER_RIGHT
#define LV_SCR_LOAD_ANIM_OVER_TOP      LV_SCREEN_LOAD_ANIM_OVER_TOP
#define LV_SCR_LOAD_ANIM_OVER_BOTTOM   LV_SCREEN_LOAD_ANIM_OVER_BOTTOM
#define LV_SCR_LOAD_ANIM_MOVE_LEFT     LV_SCREEN_LOAD_ANIM_MOVE_LEFT
#define LV_SCR_LOAD_ANIM_MOVE_RIGHT    LV_SCREEN_LOAD_ANIM_MOVE_RIGHT
#define LV_SCR_LOAD_ANIM_MOVE_TOP      LV_SCREEN_LOAD_ANIM_MOVE_TOP
#define LV_SCR_LOAD_ANIM_MOVE_BOTTOM   LV_SCREEN_LOAD_ANIM_MOVE_BOTTOM
#define LV_SCR_LOAD_ANIM_FADE_IN       LV_SCREEN_LOAD_ANIM_FADE_IN
#define LV_SCR_LOAD_ANIM_FADE_ON       LV_SCREEN_LOAD_ANIM_FADE_ON
#define LV_SCR_LOAD_ANIM_FADE_OUT      LV_SCREEN_LOAD_ANIM_FADE_OUT
#define LV_SCR_LOAD_ANIM_OUT_LEFT      LV_SCREEN_LOAD_ANIM_OUT_LEFT
#define LV_SCR_LOAD_ANIM_OUT_RIGHT     LV_SCREEN_LOAD_ANIM_OUT_RIGHT
#define LV_SCR_LOAD_ANIM_OUT_TOP       LV_SCREEN_LOAD_ANIM_OUT_TOP
#define LV_SCR_LOAD_ANIM_OUT_BOTTOM    LV_SCREEN_LOAD_ANIM_OUT_BOTTOM

#define lv_ft81x_spi_operation lv_ft81x_spi_operation_t

#define lv_glfw_window_t                 lv_opengles_window_t
#define lv_glfw_texture_t                lv_opengles_window_texture_t
#define lv_glfw_window_create            lv_opengles_glfw_window_create
#define lv_glfw_window_delete            lv_opengles_window_delete
#define lv_glfw_window_add_texture       lv_opengles_window_add_texture
#define lv_glfw_texture_remove           lv_opengles_window_texture_remove
#define lv_glfw_texture_set_x            lv_opengles_window_texture_set_x
#define lv_glfw_texture_set_y            lv_opengles_window_texture_set_y
#define lv_glfw_texture_set_opa          lv_opengles_window_texture_set_opa
#define lv_glfw_texture_get_mouse_indev  lv_opengles_window_texture_get_mouse_indev
#define lv_glfw_window_get_glfw_window   lv_opengles_glfw_window_get_glfw_window

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_API_MAP_V9_2_H */
