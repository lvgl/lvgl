/**
 * @file lv_opengles_driver.h
 *
 */

#ifndef LV_OPENGLES_DRIVER_H
#define LV_OPENGLES_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize OpenGL
 * @note    it is not necessary to call this if you use `lv_glfw_window_create`
 */
void lv_opengles_init(void);

/**
 * Deinitialize OpenGL
 * @note    it is not necessary to call this if you use `lv_glfw_window_create`
 */
void lv_opengles_deinit(void);

/**
 * Render a texture
 * @param texture        OpenGL texture ID
 * @param texture_area   the area in the window to render the texture in
 * @param opa            opacity to blend the texture with existing contents
 * @param disp_w         width of the window being rendered to
 * @param disp_h         height of the window being rendered to
 */
void lv_opengles_render_texture(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa, int32_t disp_w,
                                int32_t disp_h);

/**
 * Clear the window/display
 */
void lv_opengles_render_clear(void);

/**
 * Set the OpenGL viewport
 * @param x        x position of the viewport
 * @param y        y position of the viewport
 * @param w        width of the viewport
 * @param h        height of the viewport
 */
void lv_opengles_viewport(int32_t x, int32_t y, int32_t w, int32_t h);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_DRIVER_H */
