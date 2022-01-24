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
#include "../lv_conf_internal.h"

#if LV_USE_CANVAS != 0

#include "../core/lv_obj.h"
#include "../widgets/lv_img.h"
#include "../draw/lv_draw_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
extern const lv_obj_class_t lv_canvas_class;

/*Data of canvas*/
typedef struct {
    lv_img_t img;
    lv_img_dsc_t dsc;
} lv_canvas_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a canvas object
 * @param parent     pointer to an object, it will be the parent of the new canvas
 * @return           pointer to the created canvas
 */
lv_obj_t * lv_canvas_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a buffer for the canvas.
 * @param buf a buffer where the content of the canvas will be.
 * The required size is (lv_img_color_format_get_px_size(cf) * w) / 8 * h)
 * It can be allocated with `lv_mem_alloc()` or
 * it can be statically allocated array (e.g. static lv_color_t buf[100*50]) or
 * it can be an address in RAM or external SRAM
 * @param canvas pointer to a canvas object
 * @param w width of the canvas
 * @param h height of the canvas
 * @param cf color format. `LV_IMG_CF_...`
 */
void lv_canvas_set_buffer(lv_obj_t * canvas, void * buf, lv_coord_t w, lv_coord_t h, lv_img_cf_t cf);

/**
 * Set the color of a pixel on the canvas
 * @param canvas
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param c color of the pixel
 */
void lv_canvas_set_px_color(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_color_t c);

/**
 * DEPRECATED: added only for backward compatibility
 */
static inline void lv_canvas_set_px(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_color_t c)
{
    lv_canvas_set_px_color(canvas, x, y, c);
}

/**
 * Set the opacity of a pixel on the canvas
 * @param canvas
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param opa opacity of the pixel (0..255)
 */
void lv_canvas_set_px_opa(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_opa_t opa);


/**
 * Set the palette color of a canvas with index format. Valid only for `LV_IMG_CF_INDEXED1/2/4/8`
 * @param canvas pointer to canvas object
 * @param id the palette color to set:
 *   - for `LV_IMG_CF_INDEXED1`: 0..1
 *   - for `LV_IMG_CF_INDEXED2`: 0..3
 *   - for `LV_IMG_CF_INDEXED4`: 0..15
 *   - for `LV_IMG_CF_INDEXED8`: 0..255
 * @param c the color to set
 */
void lv_canvas_set_palette(lv_obj_t * canvas, uint8_t id, lv_color_t c);

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
 * Get the image of the canvas as a pointer to an `lv_img_dsc_t` variable.
 * @param canvas pointer to a canvas object
 * @return pointer to the image descriptor.
 */
lv_img_dsc_t * lv_canvas_get_img(lv_obj_t * canvas);

/*=====================
 * Other functions
 *====================*/

/**
 * Copy a buffer to the canvas
 * @param canvas pointer to a canvas object
 * @param to_copy buffer to copy. The color format has to match with the canvas's buffer color
 * format
 * @param x left side of the destination position
 * @param y top side of the destination position
 * @param w width of the buffer to copy
 * @param h height of the buffer to copy
 */
void lv_canvas_copy_buf(lv_obj_t * canvas, const void * to_copy, lv_coord_t x, lv_coord_t y, lv_coord_t w,
                        lv_coord_t h);

/**
 * Transform and image and store the result on a canvas.
 * @param canvas pointer to a canvas object to store the result of the transformation.
 * @param img pointer to an image descriptor to transform.
 *             Can be the image descriptor of an other canvas too (`lv_canvas_get_img()`).
 * @param angle the angle of rotation (0..3600), 0.1 deg resolution
 * @param zoom zoom factor (256 no zoom);
 * @param offset_x offset X to tell where to put the result data on destination canvas
 * @param offset_y offset X to tell where to put the result data on destination canvas
 * @param pivot_x pivot X of rotation. Relative to the source canvas
 *                Set to `source width / 2` to rotate around the center
 * @param pivot_y pivot Y of rotation. Relative to the source canvas
 *                Set to `source height / 2` to rotate around the center
 * @param antialias apply anti-aliasing during the transformation. Looks better but slower.
 */
void lv_canvas_transform(lv_obj_t * canvas, lv_img_dsc_t * img, int16_t angle, uint16_t zoom, lv_coord_t offset_x,
                         lv_coord_t offset_y,
                         int32_t pivot_x, int32_t pivot_y, bool antialias);

/**
 * Apply horizontal blur on the canvas
 * @param canvas pointer to a canvas object
 * @param area the area to blur. If `NULL` the whole canvas will be blurred.
 * @param r radius of the blur
 */
void lv_canvas_blur_hor(lv_obj_t * canvas, const lv_area_t * area, uint16_t r);

/**
 * Apply vertical blur on the canvas
 * @param canvas pointer to a canvas object
 * @param area the area to blur. If `NULL` the whole canvas will be blurred.
 * @param r radius of the blur
 */
void lv_canvas_blur_ver(lv_obj_t * canvas, const lv_area_t * area, uint16_t r);

/**
 * Fill the canvas with color
 * @param canvas pointer to a canvas
 * @param color the background color
 * @param opa the desired opacity
 */
void lv_canvas_fill_bg(lv_obj_t * canvas, lv_color_t color, lv_opa_t opa);

/**
 * Draw a rectangle on the canvas
 * @param canvas   pointer to a canvas object
 * @param x        left coordinate of the rectangle
 * @param y        top coordinate of the rectangle
 * @param w        width of the rectangle
 * @param h        height of the rectangle
 * @param draw_dsc descriptor of the rectangle
 */
void lv_canvas_draw_rect(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
                         const lv_draw_rect_dsc_t * draw_dsc);

/**
 * Draw a text on the canvas.
 * @param canvas   pointer to a canvas object
 * @param x        left coordinate of the text
 * @param y        top coordinate of the text
 * @param max_w    max width of the text. The text will be wrapped to fit into this size
 * @param draw_dsc pointer to a valid label descriptor `lv_draw_label_dsc_t`
 * @param txt      text to display
 */
void lv_canvas_draw_text(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t max_w,
                         lv_draw_label_dsc_t * draw_dsc, const char * txt);

/**
 * Draw an image on the canvas
 * @param canvas   pointer to a canvas object
 * @param x        left coordinate of the image
 * @param y        top coordinate of the image
 * @param src      image source. Can be a pointer an `lv_img_dsc_t` variable or a path an image.
 * @param draw_dsc pointer to a valid label descriptor `lv_draw_img_dsc_t`
 */
void lv_canvas_draw_img(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, const void * src,
                        const lv_draw_img_dsc_t * draw_dsc);

/**
 * Draw a line on the canvas
 * @param canvas     pointer to a canvas object
 * @param points     point of the line
 * @param point_cnt  number of points
 * @param draw_dsc   pointer to an initialized `lv_draw_line_dsc_t` variable
 */
void lv_canvas_draw_line(lv_obj_t * canvas, const lv_point_t points[], uint32_t point_cnt,
                         const lv_draw_line_dsc_t * draw_dsc);

/**
 * Draw a polygon on the canvas
 * @param canvas    pointer to a canvas object
 * @param points    point of the polygon
 * @param point_cnt number of points
 * @param draw_dsc  pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_canvas_draw_polygon(lv_obj_t * canvas, const lv_point_t points[], uint32_t point_cnt,
                            const lv_draw_rect_dsc_t * draw_dsc);

/**
 * Draw an arc on the canvas
 * @param canvas pointer to a canvas object
 * @param x      origo x  of the arc
 * @param y      origo y of the arc
 * @param r      radius of the arc
 * @param start_angle start angle in degrees
 * @param end_angle   end angle in degrees
 * @param draw_dsc    pointer to an initialized `lv_draw_line_dsc_t` variable
 */
void lv_canvas_draw_arc(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t r, int32_t start_angle,
                        int32_t end_angle, const lv_draw_arc_dsc_t * draw_dsc);

/**********************
 *      MACROS
 **********************/
#define LV_CANVAS_BUF_SIZE_TRUE_COLOR(w, h) LV_IMG_BUF_SIZE_TRUE_COLOR(w, h)
#define LV_CANVAS_BUF_SIZE_TRUE_COLOR_CHROMA_KEYED(w, h) LV_IMG_BUF_SIZE_TRUE_COLOR_CHROMA_KEYED(w, h)
#define LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(w, h) LV_IMG_BUF_SIZE_TRUE_COLOR_ALPHA(w, h)

/*+ 1: to be sure no fractional row*/
#define LV_CANVAS_BUF_SIZE_ALPHA_1BIT(w, h) LV_IMG_BUF_SIZE_ALPHA_1BIT(w, h)
#define LV_CANVAS_BUF_SIZE_ALPHA_2BIT(w, h) LV_IMG_BUF_SIZE_ALPHA_2BIT(w, h)
#define LV_CANVAS_BUF_SIZE_ALPHA_4BIT(w, h) LV_IMG_BUF_SIZE_ALPHA_4BIT(w, h)
#define LV_CANVAS_BUF_SIZE_ALPHA_8BIT(w, h) LV_IMG_BUF_SIZE_ALPHA_8BIT(w, h)

/*4 * X: for palette*/
#define LV_CANVAS_BUF_SIZE_INDEXED_1BIT(w, h) LV_IMG_BUF_SIZE_INDEXED_1BIT(w, h)
#define LV_CANVAS_BUF_SIZE_INDEXED_2BIT(w, h) LV_IMG_BUF_SIZE_INDEXED_2BIT(w, h)
#define LV_CANVAS_BUF_SIZE_INDEXED_4BIT(w, h) LV_IMG_BUF_SIZE_INDEXED_4BIT(w, h)
#define LV_CANVAS_BUF_SIZE_INDEXED_8BIT(w, h) LV_IMG_BUF_SIZE_INDEXED_8BIT(w, h)

#endif /*LV_USE_CANVAS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CANVAS_H*/
