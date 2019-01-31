/**
 * @file lv_canvas.h
 *
 */

#ifndef LV_CANVAS_H
#define LV_CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_CANVAS != 0

#include "../lv_core/lv_obj.h"
#include "../lv_objx/lv_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of canvas*/
typedef struct {
    lv_img_ext_t img; /*Ext. of ancestor*/
    /*New data for this type */
    lv_img_dsc_t dsc;
} lv_canvas_ext_t;


/*Styles*/
enum {
    LV_CANVAS_STYLE_MAIN,
};
typedef uint8_t lv_canvas_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a canvas object
 * @param par pointer to an object, it will be the parent of the new canvas
 * @param copy pointer to a canvas object, if not NULL then the new object will be copied from it
 * @return pointer to the created canvas
 */
lv_obj_t * lv_canvas_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a buffer for the canvas.
 * @param buf a buffer where the content of the canvas will be.
 * The required size is (lv_img_color_format_get_px_size(cf) * w * h) / 8)
 * It can be allocated with `lv_mem_alloc()` or
 * it can be statically allocated array (e.g. static lv_color_t buf[100*50]) or
 * it can be an address in RAM or external SRAM
 * @param canvas pointer to a canvas object
 * @param w width of the canvas
 * @param h height of the canvas
 * @param cf color format. The following formats are supported:
 *      LV_IMG_CF_TRUE_COLOR, LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED, LV_IMG_CF_INDEXES_1/2/4/8BIT
 */
void lv_canvas_set_buffer(lv_obj_t * canvas, void * buf, lv_coord_t w, lv_coord_t h, lv_img_cf_t cf);

/**
 * Set the color of a pixel on the canvas
 * @param canvas
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param c color of the point
 */
void lv_canvas_set_px(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_color_t c);

/**
 * Set a style of a canvas.
 * @param canvas pointer to canvas object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_canvas_set_style(lv_obj_t * canvas, lv_canvas_style_t type, lv_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the color of a pixel on the canvas
 * @param canvas
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @return color of the point
 */
lv_color_t lv_canvas_get_px(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y);

/**
 * Get style of a canvas.
 * @param canvas pointer to canvas object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_canvas_get_style(const lv_obj_t * canvas, lv_canvas_style_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Copy a buffer to the canvas
 * @param canvas pointer to a canvas object
 * @param to_copy buffer to copy. The color format has to match with the canvas's buffer color format
 * @param w width of the buffer to copy
 * @param h height of the buffer to copy
 * @param x left side of the destination position
 * @param y top side of the destination position
 */
void lv_canvas_copy_buf(lv_obj_t * canvas, const void * to_copy, lv_coord_t w, lv_coord_t h, lv_coord_t x, lv_coord_t y);

/**
 * Multiply a buffer with the canvas
 * @param canvas pointer to a canvas object
 * @param to_copy buffer to copy (multiply). LV_IMG_CF_TRUE_COLOR_ALPHA is not supported
 * @param w width of the buffer to copy
 * @param h height of the buffer to copy
 * @param x left side of the destination position
 * @param y top side of the destination position
 */
void lv_canvas_mult_buf(lv_obj_t * canvas, void * to_copy, lv_coord_t w, lv_coord_t h, lv_coord_t x, lv_coord_t y);

/**
 * Draw circle function of the canvas
 * @param canvas pointer to a canvas object
 * @param x0 x coordinate of the circle
 * @param y0 y coordinate of the circle
 * @param radius radius of the circle
 * @param color border color of the circle
 */
void lv_canvas_draw_circle(lv_obj_t * canvas, lv_coord_t x0, lv_coord_t y0, lv_coord_t radius, lv_color_t color);

/**
 * Draw line function of the canvas
 * @param canvas pointer to a canvas object
 * @param point1 start point of the line
 * @param point2 end point of the line
 * @param color color of the line
 *
 * NOTE: The lv_canvas_draw_line function originates from https://github.com/jb55/bresenham-line.c.
 */
void lv_canvas_draw_line(lv_obj_t * canvas, lv_point_t point1, lv_point_t point2, lv_color_t color);

/**
 * Draw triangle function of the canvas
 * @param canvas pointer to a canvas object
 * @param points edge points of the triangle
 * @param color line color of the triangle
 */
void lv_canvas_draw_triangle(lv_obj_t * canvas, lv_point_t * points, lv_color_t color);

/**
 * Draw rectangle function of the canvas
 * @param canvas pointer to a canvas object
 * @param points edge points of the rectangle
 * @param color line color of the rectangle
 */
void lv_canvas_draw_rect(lv_obj_t * canvas, lv_point_t * points, lv_color_t color);

/**
 * Draw polygon function of the canvas
 * @param canvas pointer to a canvas object
 * @param points edge points of the polygon
 * @param size edge count of the polygon
 * @param color line color of the polygon
 */
void lv_canvas_draw_polygon(lv_obj_t * canvas, lv_point_t * points, size_t size, lv_color_t color);

/**
 * Fill polygon function of the canvas
 * @param canvas pointer to a canvas object
 * @param points edge points of the polygon
 * @param size edge count of the polygon
 * @param boundary_color line color of the polygon
 * @param fill_color fill color of the polygon
 */
void lv_canvas_fill_polygon(lv_obj_t * canvas, lv_point_t * points, size_t size, lv_color_t boundary_color, lv_color_t fill_color);
/**
 * Boundary fill function of the canvas
 * @param canvas pointer to a canvas object
 * @param x x coordinate of the start position (seed)
 * @param y y coordinate of the start position (seed)
 * @param boundary_color edge/boundary color of the area
 * @param fill_color fill color of the area 
 */
void lv_canvas_boundary_fill4(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_color_t boundary_color, lv_color_t fill_color);

/**
 * Flood fill function of the canvas
 * @param canvas pointer to a canvas object
 * @param x x coordinate of the start position (seed)
 * @param y y coordinate of the start position (seed)
 * @param fill_color fill color of the area
 * @param bg_color background color of the area
 */
void lv_canvas_flood_fill(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_color_t fill_color, lv_color_t bg_color);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_CANVAS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_CANVAS_H*/
