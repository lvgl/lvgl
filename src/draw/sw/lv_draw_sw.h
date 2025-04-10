/**
 * @file lv_draw_sw.h
 *
 */

#ifndef LV_DRAW_SW_H
#define LV_DRAW_SW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../display/lv_display.h"
#include "../../osal/lv_os.h"

#include "../lv_draw_vector.h"
#include "../lv_draw_triangle.h"
#include "../lv_draw_label.h"
#include "../lv_draw_image.h"
#include "../lv_draw_line.h"
#include "../lv_draw_arc.h"
#include "lv_draw_sw_utils.h"
#include "blend/lv_draw_sw_blend.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the SW renderer. Called in internally.
 * It creates as many SW renderers as defined in LV_DRAW_SW_DRAW_UNIT_CNT
 */
void lv_draw_sw_init(void);

/**
 * Deinitialize the SW renderers
 */
void lv_draw_sw_deinit(void);

/**
 * Fill an area using SW render. Handle gradient and radius.
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_sw_fill(lv_draw_task_t * t, lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw border with SW render.
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_sw_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw box shadow with SW render.
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the rectangle for which the box shadow should be drawn
 */
void lv_draw_sw_box_shadow(lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw an image with SW render. It handles image decoding, tiling, transformations, and recoloring.
 * @param t             pointer to a draw task
 * @param draw_dsc      the draw descriptor
 * @param coords        the coordinates of the image
 */
void lv_draw_sw_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                      const lv_area_t * coords);

void lv_draw_sw_letter(lv_draw_task_t * t, const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw a label with SW render.
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the label
 */
void lv_draw_sw_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw an arc with SW render.
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the arc
 */
void lv_draw_sw_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw a line with SW render.
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 */
void lv_draw_sw_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);

/**
 * Blend a layer with SW render
 * @param t             pointer to a draw task
 * @param draw_dsc      the draw descriptor
 * @param coords        the coordinates of the layer
 */
void lv_draw_sw_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

/**
 * Draw a triangle with SW render.
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 */
void lv_draw_sw_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc);

/**
 * Mask out a rectangle with radius from a current layer
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the mask
 */
void lv_draw_sw_mask_rect(lv_draw_task_t * t, const lv_draw_mask_rect_dsc_t * dsc);

/**
 * Used internally to get a transformed are of an image
 * @param dest_area     area to calculate, i.e. get this area from the transformed image
 * @param src_buf       source buffer
 * @param src_w         source buffer width in pixels
 * @param src_h         source buffer height in pixels
 * @param src_stride    source buffer stride in bytes
 * @param draw_dsc      draw descriptor
 * @param sup           supplementary data
 * @param cf            color format of the source buffer
 * @param dest_buf      the destination buffer
 */
void lv_draw_sw_transform(const lv_area_t * dest_area, const void * src_buf,
                          int32_t src_w, int32_t src_h, int32_t src_stride,
                          const lv_draw_image_dsc_t * draw_dsc, const lv_draw_image_sup_t * sup, lv_color_format_t cf, void * dest_buf);

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
/**
 * Draw vector graphics with SW render.
 * @param t             pointer to a draw task
 * @param dsc           the draw descriptor
 */
void lv_draw_sw_vector(lv_draw_task_t * t, lv_draw_vector_task_dsc_t * dsc);
#endif

/**
 * Register a custom blend handler for a color format.
 * Handler will be called when blending a color or an
 * image to a buffer with the given color format.
 * At most one handler can be registered for a color format.
 * Subsequent registrations will overwrite the previous handler.
 *
 * @param handler pointer to a blend handler
 * @return true if the handler was registered, false if the handler could not be registered
 */
bool lv_draw_sw_register_blend_handler(lv_draw_sw_custom_blend_handler_t * handler);

/**
 * Unregister a custom blend handler for a color format.
 * @param dest_cf color format
 * @return true if a handler was unregistered, false if no handler was registered
 */
bool lv_draw_sw_unregister_blend_handler(lv_color_format_t dest_cf);

/**
 * Get the blend handler for a color format.
 * @param dest_cf color format
 * @return pointer to the blend handler or NULL if no handler is registered
 */
lv_draw_sw_blend_handler_t lv_draw_sw_get_blend_handler(lv_color_format_t dest_cf);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_H*/
