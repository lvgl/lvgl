/**
 * @file lv_canvas.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include "lv_canvas.h"
#include "../lv_misc/lv_math.h"
#include "../lv_draw/lv_draw_img.h"
#if LV_USE_CANVAS != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_canvas_signal(lv_obj_t * canvas, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a canvas object
 * @param par pointer to an object, it will be the parent of the new canvas
 * @param copy pointer to a canvas object, if not NULL then the new object will be copied from it
 * @return pointer to the created canvas
 */
lv_obj_t * lv_canvas_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("canvas create started");

    /*Create the ancestor of canvas*/
    lv_obj_t * new_canvas = lv_img_create(par, copy);
    lv_mem_assert(new_canvas);
    if(new_canvas == NULL) return NULL;

    /*Allocate the canvas type specific extended data*/
    lv_canvas_ext_t * ext = lv_obj_allocate_ext_attr(new_canvas, sizeof(lv_canvas_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_canvas);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_canvas);

    /*Initialize the allocated 'ext' */
    ext->dsc.header.always_zero = 0;
    ext->dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    ext->dsc.header.h = 0;
    ext->dsc.header.w = 0;
    ext->dsc.data_size = 0;
    ext->dsc.data = NULL;

    lv_img_set_src(new_canvas, &ext->dsc);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_canvas, lv_canvas_signal);

    /*Init the new canvas canvas*/
    if(copy == NULL) {

    }
    /*Copy an existing canvas*/
    else {
        //lv_canvas_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_canvas);
    }

    LV_LOG_INFO("canvas created");

    return new_canvas;
}

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
 *
 */
void lv_canvas_set_buffer(lv_obj_t * canvas, void * buf, lv_coord_t w, lv_coord_t h, lv_img_cf_t cf)
{
    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    ext->dsc.header.cf = cf;
    ext->dsc.header.w = w;
    ext->dsc.header.h = h;
    ext->dsc.data = buf;
    ext->dsc.data_size = (lv_img_color_format_get_px_size(cf) * w * h) / 8;

    lv_img_set_src(canvas, &ext->dsc);
}

/**
 * Set the color of a pixel on the canvas
 * @param canvas pointer to canvas object
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param c color of the point
 */
void lv_canvas_set_px(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_color_t c)
{
    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    lv_img_buf_set_px(&ext->dsc, x, y, c);
    lv_obj_invalidate(canvas);

}

/**
 * Set a style of a canvas.
 * @param canvas pointer to canvas object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_canvas_set_style(lv_obj_t * canvas, lv_canvas_style_t type, lv_style_t * style)
{
    switch(type) {
    case LV_CANVAS_STYLE_MAIN:
        lv_img_set_style(canvas, style);
        break;
    }
}

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
lv_color_t lv_canvas_get_px(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y)
{
    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    return lv_img_buf_get_px(&ext->dsc, x, y);
}

/**
 * Get the image of the canvas as a pointer to an `lv_img_dsc_t` variable.
 * @param canvas pointer to a canvas object
 * @return pointer to the image descriptor.
 */
lv_img_dsc_t * lv_canvas_get_img(lv_obj_t * canvas)
{
    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    return &ext->img;
}

/**
 * Get style of a canvas.
 * @param canvas pointer to canvas object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_canvas_get_style(const lv_obj_t * canvas, lv_canvas_style_t type)
{
    // lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);
    lv_style_t * style = NULL;

    switch(type) {
    case LV_CANVAS_STYLE_MAIN:
        style = lv_img_get_style(canvas);
        break;
    default:
        style =  NULL;
    }

    return style;
}

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
void lv_canvas_copy_buf(lv_obj_t * canvas, const void * to_copy, lv_coord_t w, lv_coord_t h, lv_coord_t x, lv_coord_t y)
{
    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);
    if(x + w >= ext->dsc.header.w || y + h >= ext->dsc.header.h) {
        LV_LOG_WARN("lv_canvas_copy_buf: x or y out of the canvas");
        return;
    }

    uint32_t px_size = lv_img_color_format_get_px_size(ext->dsc.header.cf) >> 3;
    uint32_t px = ext->dsc.header.w * y * px_size + x * px_size;
    uint8_t * to_copy8 = (uint8_t *) to_copy;
    lv_coord_t i;
    for(i = 0; i < h; i++) {
        memcpy((void*)&ext->dsc.data[px], to_copy8, w * px_size);
        px += ext->dsc.header.w * px_size;
        to_copy8 += w * px_size;
    }
}

/**
 * Multiply a buffer with the canvas
 * @param canvas pointer to a canvas object
 * @param to_copy buffer to copy (multiply). LV_IMG_CF_TRUE_COLOR_ALPHA is not supported
 * @param w width of the buffer to copy
 * @param h height of the buffer to copy
 * @param x left side of the destination position
 * @param y top side of the destination position
 */
void lv_canvas_mult_buf(lv_obj_t * canvas, void * to_copy, lv_coord_t w, lv_coord_t h, lv_coord_t x, lv_coord_t y)
{
    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);
    if(x + w >= ext->dsc.header.w || y + h >= ext->dsc.header.h) {
        LV_LOG_WARN("lv_canvas_mult_buf: x or y out of the canvas");
        return;
    }

    if(ext->dsc.header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        LV_LOG_WARN("lv_canvas_mult_buf: LV_IMG_CF_TRUE_COLOR_ALPHA is not supported");
        return;
    }

    uint32_t px_size = lv_img_color_format_get_px_size(ext->dsc.header.cf) >> 3;
    uint32_t px = ext->dsc.header.w * y * px_size + x * px_size;
    lv_color_t * copy_buf_color = (lv_color_t *) to_copy;
    lv_color_t * canvas_buf_color = (lv_color_t *) &ext->dsc.data[px];

    lv_coord_t i;
    lv_coord_t j;
    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
#if LV_COLOR_DEPTH == 32
            canvas_buf_color[j].ch.red = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.red * copy_buf_color[j].ch.red) >> 8;
            canvas_buf_color[j].ch.green = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.green * copy_buf_color[j].ch.green) >> 8;
            canvas_buf_color[j].ch.blue = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.blue * copy_buf_color[j].ch.blue) >> 8;
#elif LV_COLOR_DEPTH == 16

            canvas_buf_color[j].ch.red = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.red * copy_buf_color[j].ch.red) >> 5;
            canvas_buf_color[j].ch.blue = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.blue * copy_buf_color[j].ch.blue) >> 5;
#  if LV_COLOR_16_SWAP == 0
            canvas_buf_color[j].ch.green = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.green * copy_buf_color[j].ch.green) >> 6;
#  else
            canvas_buf_color[j].ch.red = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.red * copy_buf_color[j].ch.red) >> 6;
            canvas_buf_color[j].ch.blue = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.blue * copy_buf_color[j].ch.blue) >> 6;
            canvas_buf_color[j].ch.red = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.red * copy_buf_color[j].ch.red) >> 6;
#  endif    /*LV_COLOR_16_SWAP*/

#elif LV_COLOR_DEPTH == 8
            canvas_buf_color[j].ch.red = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.red * copy_buf_color[j].ch.red) >> 3;
            canvas_buf_color[j].ch.green = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.green * copy_buf_color[j].ch.green) >> 3;
            canvas_buf_color[j].ch.blue = (uint16_t) ((uint16_t) canvas_buf_color[j].ch.blue * copy_buf_color[j].ch.blue) >> 2;
#endif
        }
        copy_buf_color += w;
        canvas_buf_color += ext->dsc.header.w;
    }
}

/**
 * Rotate and image and store the result on a canvas.
 * @param canvas pointer to a canvas object
 * @param img pointer to an image descriptor.
 *             Can be the image descriptor of an other canvas too (`lv_canvas_get_img()`).
 * @param angle the angle of rotation (0..360);
 * @param offset_x offset X to tell where to put the result data on destination canvas
 * @param offset_y offset X to tell where to put the result data on destination canvas
 * @param pivot_x pivot X of rotation. Relative to the source canvas
 *                Set to `source width / 2` to rotate around the center
 * @param pivot_y pivot Y of rotation. Relative to the source canvas
 *                Set to `source height / 2` to rotate around the center
 */
void lv_canvas_rotate(lv_obj_t * canvas, lv_img_dsc_t * img, int16_t angle,lv_coord_t offset_x, lv_coord_t offset_y, int32_t pivot_x, int32_t pivot_y)
{
    lv_canvas_ext_t * ext_dst = lv_obj_get_ext_attr(canvas);

    int32_t sinma = lv_trigo_sin(-angle);
    int32_t cosma = lv_trigo_sin(-angle + 90); /* cos */

    int32_t img_width = img->header.w;
    int32_t img_height = img->header.h;
    int32_t dest_width = ext_dst->dsc.header.w;
    int32_t dest_height = ext_dst->dsc.header.h;

    int32_t x;
    int32_t y;
    for (x = -offset_x; x < dest_width - offset_x; x++) {
        for (y = -offset_y; y < dest_height - offset_y; y++) {
            /*Get the target point relative coordinates to the pivot*/
            int32_t xt = x - pivot_x;
            int32_t yt = y - pivot_y;

            /*Get the source pixel from the upscaled image*/
            int32_t xs = ((cosma * xt - sinma * yt) >> (LV_TRIGO_SHIFT - 8)) + pivot_x * 256;
            int32_t ys = ((sinma * xt + cosma * yt) >> (LV_TRIGO_SHIFT - 8)) + pivot_y * 256;

            /*Get the integer part of the source pixel*/
            int xs_int = xs >> 8;
            int ys_int = ys >> 8;


            if(xs_int >= img_width) continue;
            else if(xs_int < 0) continue;


            if(ys_int >= img_height) continue;
            else if(ys_int < 0) continue;

            /*Get the fractional part of the source pixel*/
            int xs_fract = xs & 0xff;
            int ys_fract = ys & 0xff;

            /* If the fractional < 0x70 mix the source pixel with the left/top pixel
             * If the fractional > 0x90 mix the source pixel with the right/bottom pixel
             * In the 0x70..0x90 range use the unchanged source pixel */

            int xn;           /*x neightboor*/
            lv_opa_t xr;      /*x mix ratio*/
            if(xs_fract < 0x70) {
                xn = xs_int - 1;
                xr = xs_fract * 2;
            }
            else if(xs_fract > 0x90) {
                xn = xs_int + 1;
                xr = (0xFF - xs_fract) * 2;
            }
            else {
                xn = xs_int;
                xr = 0xFF;
            }

            /*Handle under/overflow*/
            if(xn >= img_width) continue;
            else if(xn < 0) continue;

            int yn;            /*y neightboor*/
            lv_opa_t yr;       /*y mix ratio*/
            if(ys_fract < 0x70) {
                yn = ys_int - 1;
                yr = ys_fract * 2;
            }
            else if(ys_fract > 0x90) {
                yn = ys_int + 1;
                yr = (0xFF - ys_fract) * 2;
            }
            else {
                yn = ys_int;
                yr = 0xFF;
            }

            /*Handle under/overflow*/
            if(yn >= img_height) continue;
            else if(yn < 0) continue;

            /*Get the mixture of the original source and the neightboor pixels in both directions*/
            lv_color_t c_dest_int = lv_img_buf_get_px(img, xs_int, ys_int);
            lv_color_t c_dest_xn = lv_img_buf_get_px(img, xn, ys_int);
            lv_color_t c_dest_yn = lv_img_buf_get_px(img, xs_int, yn);
            lv_color_t x_dest = lv_color_mix(c_dest_int, c_dest_xn, xr);
            lv_color_t y_dest = lv_color_mix(c_dest_int, c_dest_yn, yr);

            if (x + offset_x >= 0 && x + offset_x < dest_width && y + offset_y >= 0 && y + offset_y < dest_height)
            {
                /*The result color as the average of the x/y mixed colors*/
                lv_img_buf_set_px(&ext_dst->dsc, x + offset_x, y + offset_y, lv_color_mix(x_dest, y_dest, LV_OPA_50));
            }
        }
    }

    lv_obj_invalidate(canvas);

}

/**
 * Draw circle function of the canvas
 * @param canvas pointer to a canvas object
 * @param x0 x coordinate of the circle
 * @param y0 y coordinate of the circle
 * @param radius radius of the circle
 * @param color border color of the circle
 */
void lv_canvas_draw_circle(lv_obj_t * canvas, lv_coord_t x0, lv_coord_t y0, lv_coord_t radius, lv_color_t color)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        lv_canvas_set_px(canvas, x0 + x, y0 + y, color);
        lv_canvas_set_px(canvas, x0 + y, y0 + x, color);
        lv_canvas_set_px(canvas, x0 - y, y0 + x, color);
        lv_canvas_set_px(canvas, x0 - x, y0 + y, color);
        lv_canvas_set_px(canvas, x0 - x, y0 - y, color);
        lv_canvas_set_px(canvas, x0 - y, y0 - x, color);
        lv_canvas_set_px(canvas, x0 + y, y0 - x, color);
        lv_canvas_set_px(canvas, x0 + x, y0 - y, color);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }

        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

/**
 * Draw line function of the canvas
 * @param canvas pointer to a canvas object
 * @param point1 start point of the line
 * @param point2 end point of the line
 * @param color color of the line
 *
 * NOTE: The lv_canvas_draw_line function originates from https://github.com/jb55/bresenham-line.c.
 */
void lv_canvas_draw_line(lv_obj_t * canvas, lv_point_t point1, lv_point_t point2, lv_color_t color)
{
    lv_coord_t x0, y0, x1, y1;

    x0 = point1.x;
    y0 = point1.y;
    x1 = point2.x;
    y1 = point2.y;

    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;

    for(;;){
        lv_canvas_set_px(canvas, x0, y0, color);

        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

/**
 * Draw triangle function of the canvas
 * @param canvas pointer to a canvas object
 * @param points edge points of the triangle
 * @param color line color of the triangle
 */
void lv_canvas_draw_triangle(lv_obj_t * canvas, lv_point_t * points, lv_color_t color)
{
    lv_canvas_draw_polygon(canvas, points, 3, color);
}

/**
 * Draw rectangle function of the canvas
 * @param canvas pointer to a canvas object
 * @param points edge points of the rectangle
 * @param color line color of the rectangle
 */
void lv_canvas_draw_rect(lv_obj_t * canvas, lv_point_t * points, lv_color_t color)
{
    lv_canvas_draw_polygon(canvas, points, 4, color);
}

/**
 * Draw polygon function of the canvas
 * @param canvas pointer to a canvas object
 * @param points edge points of the polygon
 * @param size edge count of the polygon
 * @param color line color of the polygon
 */
void lv_canvas_draw_polygon(lv_obj_t * canvas, lv_point_t * points, size_t size, lv_color_t color)
{
    uint8_t i;

    for(i=0; i < (size - 1); i++) {
        lv_canvas_draw_line(canvas, points[i], points[i + 1], color);
    }

    lv_canvas_draw_line(canvas, points[size - 1], points[0], color);
}

/**
 * Fill polygon function of the canvas
 * @param canvas pointer to a canvas object
 * @param points edge points of the polygon
 * @param size edge count of the polygon
 * @param boundary_color line color of the polygon
 * @param fill_color fill color of the polygon
 */
void lv_canvas_fill_polygon(lv_obj_t * canvas, lv_point_t * points, size_t size, lv_color_t boundary_color, lv_color_t fill_color)
{
    uint32_t x = 0, y = 0;
    uint8_t i;

    for(i=0; i<size; i++) {
        x += points[i].x;
        y += points[i].y;
    }

    x = x / size;
    y = y / size;

    lv_canvas_boundary_fill4(canvas, (lv_coord_t) x, (lv_coord_t) y, boundary_color, fill_color);
}

/**
 * Boundary fill function of the canvas
 * @param canvas pointer to a canvas object
 * @param x x coordinate of the start position (seed)
 * @param y y coordinate of the start position (seed)
 * @param boundary_color edge/boundary color of the area
 * @param fill_color fill color of the area
 */
void lv_canvas_boundary_fill4(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_color_t boundary_color, lv_color_t fill_color)
{
    lv_color_t c;

    c = lv_canvas_get_px(canvas, x, y);

    if(c.full != boundary_color.full &&
            c.full != fill_color.full)
    {
        lv_canvas_set_px(canvas, x, y, fill_color);

        lv_canvas_boundary_fill4(canvas, x + 1,     y, boundary_color, fill_color);
        lv_canvas_boundary_fill4(canvas,     x, y + 1, boundary_color, fill_color);
        lv_canvas_boundary_fill4(canvas, x - 1,     y, boundary_color, fill_color);
        lv_canvas_boundary_fill4(canvas,     x, y - 1, boundary_color, fill_color);
    }
}

/**
 * Flood fill function of the canvas
 * @param canvas pointer to a canvas object
 * @param x x coordinate of the start position (seed)
 * @param y y coordinate of the start position (seed)
 * @param fill_color fill color of the area
 * @param bg_color background color of the area
 */
void lv_canvas_flood_fill(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_color_t fill_color, lv_color_t bg_color)
{
    lv_color_t c;

    c = lv_canvas_get_px(canvas, x, y);

    if(c.full == bg_color.full)
    {
        lv_canvas_set_px(canvas, x, y, fill_color);

        lv_canvas_flood_fill(canvas, x+1,   y, fill_color, bg_color);
        lv_canvas_flood_fill(canvas,   x, y+1, fill_color, bg_color);
        lv_canvas_flood_fill(canvas, x-1,   y, fill_color, bg_color);
        lv_canvas_flood_fill(canvas,   x, y-1, fill_color, bg_color);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the canvas
 * @param canvas pointer to a canvas object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_canvas_signal(lv_obj_t * canvas, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(canvas, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_canvas";
    }

    return res;
}

#endif
