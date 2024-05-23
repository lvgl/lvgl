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

#include "../../draw/lv_draw_vector.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
    volatile bool inited;
    volatile bool exit_status;
#endif
    uint32_t idx;
} lv_draw_sw_unit_t;

#if LV_DRAW_SW_SHADOW_CACHE_SIZE
typedef struct {
    uint8_t cache[LV_DRAW_SW_SHADOW_CACHE_SIZE * LV_DRAW_SW_SHADOW_CACHE_SIZE];
    int32_t cache_size;
    int32_t cache_r;
} lv_draw_sw_shadow_cache_t;
#endif

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
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_sw_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw border with SW render.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the rectangle
 */
void lv_draw_sw_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw box shadow with SW render.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the rectangle for which the box shadow should be drawn
 */
void lv_draw_sw_box_shadow(lv_draw_unit_t * draw_unit, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw an image with SW render. It handles image decoding, tiling, transformations, and recoloring.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the image
 */
void lv_draw_sw_image(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                      const lv_area_t * coords);

/**
 * Draw a label with SW render.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the label
 */
void lv_draw_sw_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw an arc with SW render.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the arc
 */
void lv_draw_sw_arc(lv_draw_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

/**
 * Draw a line with SW render.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 */
void lv_draw_sw_line(lv_draw_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc);

/**
 * Blend a layer with SW render
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the layer
 */
void lv_draw_sw_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

/**
 * Draw a triangle with SW render.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 */
void lv_draw_sw_triangle(lv_draw_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc);

/**
 * Mask out a rectangle with radius from a current layer
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 * @param coords        the coordinates of the mask
 */
void lv_draw_sw_mask_rect(lv_draw_unit_t * draw_unit, const lv_draw_mask_rect_dsc_t * dsc, const lv_area_t * coords);

/**
 * Used internally to get a transformed are of an image
 * @param draw_unit     pointer to a draw unit
 * @param dest_area     the area to calculate, i.e. get this area from the transformed image
 * @param src_buf       the source buffer
 * @param src_w         source buffer width in pixels
 * @param src_h         source buffer height in pixels
 * @param src_stride    source buffer stride in bytes
 * @param dsc           the draw descriptor
 * @param sup           supplementary data
 * @param cf            color format of the source buffer
 * @param dest_buf      the destination buffer
 */
void lv_draw_sw_transform(lv_draw_unit_t * draw_unit, const lv_area_t * dest_area, const void * src_buf,
                          int32_t src_w, int32_t src_h, int32_t src_stride,
                          const lv_draw_image_dsc_t * draw_dsc, const lv_draw_image_sup_t * sup, lv_color_format_t cf, void * dest_buf);

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
/**
 * Draw vector graphics with SW render.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           the draw descriptor
 */
void lv_draw_sw_vector(lv_draw_unit_t * draw_unit, const lv_draw_vector_task_dsc_t * dsc);
#endif

/**
 * Swap the upper and lower byte of an RGB565 buffer.
 * Might be required if a 8bit parallel port or an SPI port send the bytes in the wrong order.
 * The bytes will be swapped in place.
 * @param buf_size_px   number of pixels in the buffer
 */
void lv_draw_sw_rgb565_swap(void * buf, uint32_t buf_size_px);

/**
 * Rotate a buffer into an other buffer
 * @param src           the source buffer
 * @param dest          the destination buffer
 * @param src_width     source width in pixels
 * @param src_height    source height in pixels
 * @param src_sride     source stride in bytes (number of bytes in a row)
 * @param dest_stride   destination stride in bytes (number of bytes in a row)
 * @param rotation      LV_DISPLAY_ROTATION_0/90/180/270
 * @param color_format  LV_COLOR_FORMAT_RGB565/RGB888/XRGB8888/ARGB8888
 */
void lv_draw_sw_rotate(const void * src, void * dest, int32_t src_width, int32_t src_height, int32_t src_sride,
                       int32_t dest_stride, lv_display_rotation_t rotation, lv_color_format_t color_format);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#include "blend/lv_draw_sw_blend.h"

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_H*/
