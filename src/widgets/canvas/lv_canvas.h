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
#include "../../lv_conf_internal.h"

#if LV_USE_CANVAS != 0

#include "../image/lv_image.h"
#include "../../draw/lv_draw_image.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_canvas_class;

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
 *
 * Use lv_canvas_set_draw_buf() instead if you need to set a buffer with alignment requirement.
 *
 * @param obj    pointer to a canvas object
 * @param buf    buffer where content of canvas will be.
 *                 The required size is (lv_image_color_format_get_px_size(cf) * w) / 8 * h)
 *                 It can be allocated with `lv_malloc()` or
 *                 it can be statically allocated array (e.g. static lv_color_t buf[100*50]) or
 *                 it can be an address in RAM or external SRAM
 * @param w      width of canvas
 * @param h      height of canvas
 * @param cf     color format. `LV_COLOR_FORMAT...`
 */
void lv_canvas_set_buffer(lv_obj_t * obj, void * buf, int32_t w, int32_t h, lv_color_format_t cf);

/**
 * Set a draw buffer for the canvas. A draw buffer either can be allocated by `lv_draw_buf_create()`
 * or defined statically by `LV_DRAW_BUF_DEFINE_STATIC`. When buffer start address and stride has alignment
 * requirement, it's recommended to use `lv_draw_buf_create`.
 * @param obj       pointer to a canvas object
 * @param draw_buf  pointer to a draw buffer
 */
void lv_canvas_set_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf);

/**
 * Set a pixel's color and opacity
 * @param obj   pointer to a canvas
 * @param x     X coordinate of the pixel
 * @param y     Y coordinate of the pixel
 * @param color the color
 * @param opa   the opacity
 * @note        The following color formats are supported
 *              LV_COLOR_FORMAT_I1/2/4/8, LV_COLOR_FORMAT_A8,
 *              LV_COLOR_FORMAT_RGB565, LV_COLOR_FORMAT_RGB888,
 *              LV_COLOR_FORMAT_XRGB8888, LV_COLOR_FORMAT_ARGB8888
 */
void lv_canvas_set_px(lv_obj_t * obj, int32_t x, int32_t y, lv_color_t color, lv_opa_t opa);

/**
 * Set the palette color of a canvas for index format. Valid only for `LV_COLOR_FORMAT_I1/2/4/8`
 * @param obj       pointer to canvas object
 * @param index     the palette color to set:
 *                  - for `LV_COLOR_FORMAT_I1`: 0..1
 *                  - for `LV_COLOR_FORMAT_I2`: 0..3
 *                  - for `LV_COLOR_FORMAT_I4`: 0..15
 *                  - for `LV_COLOR_FORMAT_I8`: 0..255
 * @param color     the color to set
 */
void lv_canvas_set_palette(lv_obj_t * obj, uint8_t index, lv_color32_t color);

/*=====================
 * Getter functions
 *====================*/

lv_draw_buf_t * lv_canvas_get_draw_buf(lv_obj_t * obj);

/**
 * Get a pixel's color and opacity
 * @param obj   pointer to a canvas
 * @param x     X coordinate of the pixel
 * @param y     Y coordinate of the pixel
 * @return      ARGB8888 color of the pixel
 */
lv_color32_t lv_canvas_get_px(lv_obj_t * obj, int32_t x, int32_t y);

/**
 * Get the image of the canvas as a pointer to an `lv_image_dsc_t` variable.
 * @param canvas    pointer to a canvas object
 * @return          pointer to the image descriptor.
 */
lv_image_dsc_t * lv_canvas_get_image(lv_obj_t * canvas);

/**
 * Return the pointer for the buffer.
 * It's recommended to use this function instead of the buffer form the
 * return value of lv_canvas_get_image() as is can be aligned
 * @param canvas    pointer to a canvas object
 * @return          pointer to the buffer
 */
const void * lv_canvas_get_buf(lv_obj_t * canvas);

/*=====================
 * Other functions
 *====================*/

/**
 * Copy a buffer to the canvas
 * @param obj           pointer to a canvas object
 * @param canvas_area   the area of the canvas to copy
 * @param dest_buf      pointer to a buffer to store the copied data
 * @param dest_area     the area of the destination buffer to copy to. If omitted NULL, copy to the whole `dest_buf`
 */
void lv_canvas_copy_buf(lv_obj_t * obj, const lv_area_t * canvas_area, lv_draw_buf_t * dest_buf,
                        const lv_area_t * dest_area);

/**
 * Fill the canvas with color
 * @param obj       pointer to a canvas
 * @param color     the background color
 * @param opa       the desired opacity
 */
void lv_canvas_fill_bg(lv_obj_t * obj, lv_color_t color, lv_opa_t opa);

/**
 * Initialize a layer to use LVGL's generic draw functions (lv_draw_rect/label/...) on the canvas.
 * Needs to be usd in pair with `lv_canvas_finish_layer`.
 * @param canvas    pointer to a canvas
 * @param layer     pointer to a layer variable to initialize
 */
void lv_canvas_init_layer(lv_obj_t * canvas, lv_layer_t * layer);

/**
 * Wait until all the drawings are finished on layer.
 * Needs to be usd in pair with `lv_canvas_init_layer`.
 * @param canvas    pointer to a canvas
 * @param layer     pointer to a layer to finalize
 */
void lv_canvas_finish_layer(lv_obj_t * canvas, lv_layer_t * layer);

/**********************
 *      MACROS
 **********************/

#define LV_CANVAS_BUF_SIZE(w, h, bpp, stride) (((((w * bpp + 7) >> 3) + stride - 1) & ~(stride - 1)) * h + LV_DRAW_BUF_ALIGN)

/**
 * Just a wrapper to `LV_CANVAS_BUF_SIZE` for bindings.
 */
uint32_t lv_canvas_buf_size(int32_t w, int32_t h, uint8_t bpp, uint8_t stride);

#endif /*LV_USE_CANVAS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CANVAS_H*/
