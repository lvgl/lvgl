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
