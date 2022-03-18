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
/* upload lvgl buffer to opengl texture */
void lv_draw_gles_utils_upload_texture(lv_draw_ctx_t * draw_ctx);
void lv_draw_gles_utils_download_texture(lv_draw_ctx_t * draw_ctx);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_SDL_GLES*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GLES_UTILS_H*/
