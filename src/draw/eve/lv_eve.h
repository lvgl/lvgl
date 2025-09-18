/**
 * @file lv_eve.h
 *
 */

/*  Created on: 8 jun 2023
 *      Author: juanj
 *
 *  Modified by LVGL
 */

#ifndef LV_EVE_H
#define LV_EVE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_eve.h"

#if LV_USE_DRAW_EVE

#include "../../misc/lv_types.h"
#include "../../misc/lv_color.h"
#include "../../libs/FT800-FT813/EVE.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_EVE_PRIMITIVE_ZERO_VALUE,
    LV_EVE_PRIMITIVE_BITMAPS = 1UL,      /* Bitmap drawing primitive */
    LV_EVE_PRIMITIVE_POINTS = 2UL,       /* Point drawing primitive */
    LV_EVE_PRIMITIVE_LINES = 3UL,        /* Line drawing primitive */
    LV_EVE_PRIMITIVE_LINE_STRIP = 4UL,   /* Line strip drawing primitive */
    LV_EVE_PRIMITIVE_EDGE_STRIP_R = 5UL, /* Edge strip right side drawing primitive */
    LV_EVE_PRIMITIVE_EDGE_STRIP_L = 6UL, /* Edge strip left side drawing primitive */
    LV_EVE_PRIMITIVE_EDGE_STRIP_A = 7UL, /* Edge strip above drawing primitive */
    LV_EVE_PRIMITIVE_EDGE_STRIP_B = 8UL, /* Edge strip below side drawing primitive */
    LV_EVE_PRIMITIVE_RECTS = 9UL,        /* Rectangle drawing primitive */
} lv_eve_primitive_t;


typedef struct {
    lv_eve_primitive_t primitive;
    lv_color_t color;
    lv_opa_t opa;
    int32_t line_width;
    uint16_t point_size;
    uint8_t color_mask[4];
    uint8_t stencil_func[3];
    uint8_t stencil_op[2];
    uint8_t blend_func[2];
    uint16_t scx;
    uint16_t scy;
} lv_eve_drawing_context_t;

/* drawing context that is not saved and restored
 * by SAVE_CONTEXT and RESTORE_CONTEXT
 */
typedef struct {
    uint32_t bitmap_source;
    uint32_t bitmap_size;
    uint32_t bitmap_size_h;
    uint32_t bitmap_layout;
    uint32_t bitmap_layout_h;
} lv_eve_drawing_state_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

void lv_eve_save_context(void);
void lv_eve_restore_context(void);
void lv_eve_scissor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lv_eve_primitive(uint8_t context);
void lv_eve_color(lv_color_t color);
void lv_eve_color_opa(lv_opa_t opa);
void lv_eve_line_width(int32_t width);
void lv_eve_point_size(uint16_t radius);
void lv_eve_vertex_2f(int16_t x, int16_t y);
void lv_eve_color_mask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void lv_eve_stencil_func(uint8_t func, uint8_t ref, uint8_t mask);
void lv_eve_stencil_op(uint8_t sfail, uint8_t spass);
void lv_eve_blend_func(uint8_t src, uint8_t dst);

void lv_eve_draw_circle_simple(int16_t coord_x1, int16_t coord_y1, uint16_t radius_t);
void lv_eve_draw_rect_simple(int16_t coord_x1, int16_t coord_y1, int16_t coord_x2, int16_t coord_y2,
                             uint16_t radius);
void lv_eve_mask_round(int16_t coord_x1, int16_t coord_y1, int16_t coord_x2, int16_t coord_y2, int16_t radius);

/**
 * Set the bitmap source to `addr`. SPI transmission will occur unless it is already set to this value.
 * The bitmap source is not part of the saved and restored context.
 * @param addr    the remote EVE memory address to set as the bitmap source
 */
void lv_eve_bitmap_source(uint32_t addr);

/**
 * Set the bitmap size and sampling parameters. SPI transmission will occur unless the currently set parameters are already these.
 * The bitmap size is not part of the saved and restored context.
 * @param filter   the sampling method. Either EVE_NEAREST or EVE_BILINEAR
 * @param wrapx    the out of bounds sampling behavior in the X direction. Either EVE_BORDER or EVE_REPEAT
 * @param wrapy    the out of bounds sampling behavior in the Y direction. Either EVE_BORDER or EVE_REPEAT
 * @param width    the width of the bitmap in pixels
 * @param height   the height of the bitmap in pixels
 */
void lv_eve_bitmap_size(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);

/**
 * Set the bitmap format/layout parameters. SPI transmission will occur unless the currently set parameters are already these.
 * The bitmap layout is not part of the saved and restored context.
 * @param format      an eve color format value like EVE_RGB565
 * @param linestride  the stride of the bitmap rows in bytes
 * @param height      the number of rows in the bitmap
 */
void lv_eve_bitmap_layout(uint8_t format, uint16_t linestride, uint16_t height);

/**********************
 *  EXTERN VARIABLES
 **********************/


/**********************
 *      MACROS
 **********************/

/**********************
 *   INLINE FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_EVE*/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE_H */
