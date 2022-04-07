/**
 * @file lv_draw_gles_utils.h
 *
 */
#ifndef LV_DRAW_GLES_UTILS_H
#define LV_DRAW_GLES_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_GPU_GLES

#include LV_GPU_GLES_GLAD_INCLUDE_PATH

#include "lv_draw_gles_priv.h"
#include "../lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_gles_utils_internals_init(lv_draw_gles_context_internals_t * internals);
void lv_color_to_vec4_color(const lv_color_t * in, vec4 out);
void lv_color_to_vec4_color_with_opacity(const lv_color_t * in, lv_opa_t opa, vec4 out);

#ifdef LV_USE_GPU_GLES_SW_MIXED
/* upload lvgl buffer to opengl texture */
void lv_draw_gles_utils_upload_texture(lv_draw_ctx_t * draw_ctx);
void lv_draw_gles_utils_download_texture(lv_draw_ctx_t * draw_ctx);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_GLES*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GLES_UTILS_H*/
