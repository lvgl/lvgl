/**
 * @file lv_nanovg_3d.h
 * NanoVG 3D rendering extension for LVGL
 */

#ifndef LV_NANOVG_3D_H
#define LV_NANOVG_3D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG && LV_USE_3DTEXTURE

#include "../../misc/lv_types.h"
#include "../../misc/lv_area.h"
#include "../../libs/nanovg/nanovg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** 3D model handle */
typedef struct _lv_nanovg_3d_model_t lv_nanovg_3d_model_t;

/** 3D rendering context */
typedef struct _lv_nanovg_3d_ctx_t lv_nanovg_3d_ctx_t;

/** Camera configuration */
typedef struct {
    float eye[3];           /**< Camera position */
    float center[3];        /**< Look-at target */
    float up[3];            /**< Up vector */
    float fov;              /**< Field of view in degrees */
    float near_plane;       /**< Near clipping plane */
    float far_plane;        /**< Far clipping plane */
} lv_nanovg_3d_camera_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a 3D rendering context
 * @param vg NanoVG context to use
 * @return pointer to 3D context, or NULL on failure
 */
lv_nanovg_3d_ctx_t * lv_nanovg_3d_create(NVGcontext * vg);

/**
 * Destroy a 3D rendering context
 * @param ctx pointer to 3D context
 */
void lv_nanovg_3d_destroy(lv_nanovg_3d_ctx_t * ctx);

/**
 * Load a glTF model from file
 * @param ctx 3D rendering context
 * @param path file path to glTF/GLB file
 * @return pointer to loaded model, or NULL on failure
 */
lv_nanovg_3d_model_t * lv_nanovg_3d_load_model(lv_nanovg_3d_ctx_t * ctx, const char * path);

/**
 * Free a loaded model
 * @param ctx 3D rendering context
 * @param model model to free
 */
void lv_nanovg_3d_free_model(lv_nanovg_3d_ctx_t * ctx, lv_nanovg_3d_model_t * model);

/**
 * Begin 3D rendering - saves NanoVG OpenGL state
 * @param ctx 3D rendering context
 */
void lv_nanovg_3d_begin(lv_nanovg_3d_ctx_t * ctx);

/**
 * End 3D rendering - restores NanoVG OpenGL state
 * @param ctx 3D rendering context
 */
void lv_nanovg_3d_end(lv_nanovg_3d_ctx_t * ctx);

/**
 * Render a model to texture
 * @param ctx 3D rendering context
 * @param model model to render
 * @param width output texture width
 * @param height output texture height
 * @param camera camera configuration
 * @return OpenGL texture ID, or 0 on failure
 */
uint32_t lv_nanovg_3d_render_to_texture(lv_nanovg_3d_ctx_t * ctx, lv_nanovg_3d_model_t * model,
                                        int32_t width, int32_t height,
                                        const lv_nanovg_3d_camera_t * camera);

/**
 * Render a texture to the current NanoVG framebuffer
 * @param ctx 3D rendering context
 * @param texture_id OpenGL texture ID
 * @param dest_area destination area
 * @param opa opacity
 * @param layer_w layer width
 * @param layer_h layer height
 * @param clip_area clipping area
 * @param h_flip horizontal flip
 * @param v_flip vertical flip
 */
void lv_nanovg_3d_render_texture(lv_nanovg_3d_ctx_t * ctx, uint32_t texture_id,
                                 const lv_area_t * dest_area, lv_opa_t opa,
                                 int32_t layer_w, int32_t layer_h,
                                 const lv_area_t * clip_area,
                                 bool h_flip, bool v_flip);

/**
 * Render a model directly to the current framebuffer
 * @param ctx 3D rendering context
 * @param model model to render
 * @param dest_area destination area in screen coordinates
 * @param layer_w layer width for viewport setup
 * @param layer_h layer height for viewport setup
 * @param camera camera configuration
 */
void lv_nanovg_3d_render_direct(lv_nanovg_3d_ctx_t * ctx, lv_nanovg_3d_model_t * model,
                                const lv_area_t * dest_area, int32_t layer_w, int32_t layer_h,
                                const lv_nanovg_3d_camera_t * camera);

/**
 * Set rotation angle for auto-rotation
 * @param model model to rotate
 * @param angle_y rotation angle around Y axis in degrees
 */
void lv_nanovg_3d_model_set_rotation(lv_nanovg_3d_model_t * model, float angle_y);

/**
 * Initialize default camera configuration
 * @param camera pointer to camera structure
 */
void lv_nanovg_3d_camera_init(lv_nanovg_3d_camera_t * camera);

/**
 * Get model bounds (for auto-fitting camera)
 * @param model model to query
 * @param min_bounds output minimum bounds (3 floats)
 * @param max_bounds output maximum bounds (3 floats)
 */
void lv_nanovg_3d_model_get_bounds(lv_nanovg_3d_model_t * model, float * min_bounds, float * max_bounds);

/**
 * Get the global NanoVG 3D context
 * @return pointer to global 3D context, or NULL if not initialized
 */
lv_nanovg_3d_ctx_t * lv_draw_nanovg_3d_get_ctx(void);

/**
 * Initialize the global NanoVG 3D context
 * @param vg NanoVG context to use
 */
void lv_draw_nanovg_3d_init(NVGcontext * vg);

/**
 * Deinitialize the global NanoVG 3D context
 */
void lv_draw_nanovg_3d_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_NANOVG && LV_USE_3DTEXTURE */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_3D_H*/
