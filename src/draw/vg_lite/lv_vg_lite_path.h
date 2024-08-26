/**
 * @file lv_vg_lite_path.h
 *
 */

#ifndef LV_VG_LITE_PATH_H
#define LV_VG_LITE_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_utils.h"

#if LV_USE_DRAW_VG_LITE

/*********************
 *      DEFINES
 *********************/

typedef struct lv_vg_lite_path_t lv_vg_lite_path_t;
typedef struct lv_draw_vg_lite_unit_t lv_draw_vg_lite_unit_t;

typedef void (*lv_vg_lite_path_iter_cb_t)(void * user_data, uint8_t op_code, const float * data, uint32_t len);

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_vg_lite_path_init(lv_draw_vg_lite_unit_t * unit);

void lv_vg_lite_path_deinit(lv_draw_vg_lite_unit_t * unit);

lv_vg_lite_path_t * lv_vg_lite_path_create(vg_lite_format_t data_format);

void lv_vg_lite_path_destroy(lv_vg_lite_path_t * path);

lv_vg_lite_path_t * lv_vg_lite_path_get(lv_draw_vg_lite_unit_t * unit, vg_lite_format_t data_format);

void lv_vg_lite_path_drop(lv_draw_vg_lite_unit_t * unit, lv_vg_lite_path_t * path);

void lv_vg_lite_path_reset(lv_vg_lite_path_t * path, vg_lite_format_t data_format);

void lv_vg_lite_path_set_bonding_box_area(lv_vg_lite_path_t * path, const lv_area_t * area);

void lv_vg_lite_path_set_bonding_box(lv_vg_lite_path_t * path,
                                     float min_x, float min_y,
                                     float max_x, float max_y);

void lv_vg_lite_path_get_bonding_box(lv_vg_lite_path_t * path,
                                     float * min_x, float * min_y,
                                     float * max_x, float * max_y);

bool lv_vg_lite_path_update_bonding_box(lv_vg_lite_path_t * path);

void lv_vg_lite_path_set_quality(lv_vg_lite_path_t * path, vg_lite_quality_t quality);

vg_lite_path_t * lv_vg_lite_path_get_path(lv_vg_lite_path_t * path);

void lv_vg_lite_path_move_to(lv_vg_lite_path_t * path,
                             float x, float y);

void lv_vg_lite_path_line_to(lv_vg_lite_path_t * path,
                             float x, float y);

void lv_vg_lite_path_quad_to(lv_vg_lite_path_t * path,
                             float cx, float cy,
                             float x, float y);

void lv_vg_lite_path_cubic_to(lv_vg_lite_path_t * path,
                              float cx1, float cy1,
                              float cx2, float cy2,
                              float x, float y);

void lv_vg_lite_path_close(lv_vg_lite_path_t * path);

void lv_vg_lite_path_end(lv_vg_lite_path_t * path);

void lv_vg_lite_path_append_rect(lv_vg_lite_path_t * path,
                                 float x, float y,
                                 float w, float h,
                                 float r);

void lv_vg_lite_path_append_circle(lv_vg_lite_path_t * path,
                                   float cx, float cy,
                                   float rx, float ry);

void lv_vg_lite_path_append_arc_right_angle(lv_vg_lite_path_t * path,
                                            float start_x, float start_y,
                                            float center_x, float center_y,
                                            float end_x, float end_y);

void lv_vg_lite_path_append_arc(lv_vg_lite_path_t * path,
                                float cx, float cy,
                                float radius,
                                float start_angle,
                                float sweep,
                                bool pie);

void lv_vg_lite_path_append_path(lv_vg_lite_path_t * dest, const lv_vg_lite_path_t * src);

uint8_t lv_vg_lite_vlc_op_arg_len(uint8_t vlc_op);

uint8_t lv_vg_lite_path_format_len(vg_lite_format_t format);

void lv_vg_lite_path_for_each_data(const vg_lite_path_t * path, lv_vg_lite_path_iter_cb_t cb, void * user_data);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VG_LITE_PATH_H*/
