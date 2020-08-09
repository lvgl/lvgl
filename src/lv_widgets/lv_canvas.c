/**
 * @file lv_canvas.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include "lv_canvas.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_misc/lv_math.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_refr.h"
#include "../lv_themes/lv_theme.h"

#if LV_USE_CANVAS != 0

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_canvas"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_canvas_signal(lv_obj_t * canvas, lv_signal_t sign, void * param);
static void set_set_px_cb(lv_disp_drv_t * disp_drv, lv_img_cf_t cf);

static void set_px_true_color_alpha(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x,
                                    lv_coord_t y,
                                    lv_color_t color, lv_opa_t opa);

static void set_px_cb_alpha1(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa);

static void set_px_cb_alpha2(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa);

static void set_px_cb_alpha4(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa);


static void set_px_cb_alpha8(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa);

static void set_px_alpha_generic(lv_img_dsc_t * d, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);

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
    LV_ASSERT_MEM(new_canvas);
    if(new_canvas == NULL) return NULL;

    /*Allocate the canvas type specific extended data*/
    lv_canvas_ext_t * ext = lv_obj_allocate_ext_attr(new_canvas, sizeof(lv_canvas_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(new_canvas);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_canvas);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_canvas);

    /*Initialize the allocated 'ext' */
    ext->dsc.header.always_zero = 0;
    ext->dsc.header.cf          = LV_IMG_CF_TRUE_COLOR;
    ext->dsc.header.h           = 0;
    ext->dsc.header.w           = 0;
    ext->dsc.data_size          = 0;
    ext->dsc.data               = NULL;

    lv_img_set_src(new_canvas, &ext->dsc);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_canvas, lv_canvas_signal);

    /*Init the new canvas canvas*/
    if(copy == NULL) {
        lv_theme_apply(new_canvas, LV_THEME_CANVAS);
    }
    /*Copy an existing canvas*/
    else {
        /*Do not copy the image data because each canvas needs it's own buffer*/

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
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);
    LV_ASSERT_NULL(buf);

    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    ext->dsc.header.cf = cf;
    ext->dsc.header.w  = w;
    ext->dsc.header.h  = h;
    ext->dsc.data      = buf;

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
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    lv_img_buf_set_px_color(&ext->dsc, x, y, c);
    lv_obj_invalidate(canvas);
}

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
void lv_canvas_set_palette(lv_obj_t * canvas, uint8_t id, lv_color_t c)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    lv_img_buf_set_palette(&ext->dsc, id, c);
    lv_obj_invalidate(canvas);
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
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_canvas_ext_t * ext    = lv_obj_get_ext_attr(canvas);
    lv_color_t color = lv_obj_get_style_image_recolor(canvas, LV_CANVAS_PART_MAIN);

    return lv_img_buf_get_px_color(&ext->dsc, x, y, color);
}

/**
 * Get the image of the canvas as a pointer to an `lv_img_dsc_t` variable.
 * @param canvas pointer to a canvas object
 * @return pointer to the image descriptor.
 */
lv_img_dsc_t * lv_canvas_get_img(lv_obj_t * canvas)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    return &ext->dsc;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Copy a buffer to the canvas
 * @param canvas pointer to a canvas object
 * @param to_copy buffer to copy. The color format has to match with the canvas's buffer color
 * format
 * @param w width of the buffer to copy
 * @param h height of the buffer to copy
 * @param x left side of the destination position
 * @param y top side of the destination position
 */
void lv_canvas_copy_buf(lv_obj_t * canvas, const void * to_copy, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);
    LV_ASSERT_NULL(to_copy);

    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);
    if(x + w >= (lv_coord_t)ext->dsc.header.w || y + h >= (lv_coord_t)ext->dsc.header.h) {
        LV_LOG_WARN("lv_canvas_copy_buf: x or y out of the canvas");
        return;
    }

    uint32_t px_size   = lv_img_cf_get_px_size(ext->dsc.header.cf) >> 3;
    uint32_t px        = ext->dsc.header.w * y * px_size + x * px_size;
    uint8_t * to_copy8 = (uint8_t *)to_copy;
    lv_coord_t i;
    for(i = 0; i < h; i++) {
        _lv_memcpy((void *)&ext->dsc.data[px], to_copy8, w * px_size);
        px += ext->dsc.header.w * px_size;
        to_copy8 += w * px_size;
    }
}

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
                         int32_t pivot_x, int32_t pivot_y, bool antialias)
{
#if LV_USE_IMG_TRANSFORM
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);
    LV_ASSERT_NULL(img);

    lv_canvas_ext_t * ext_dst = lv_obj_get_ext_attr(canvas);
    lv_color_t color = lv_obj_get_style_image_recolor(canvas, LV_CANVAS_PART_MAIN);

    int32_t dest_width  = ext_dst->dsc.header.w;
    int32_t dest_height = ext_dst->dsc.header.h;

    int32_t x;
    int32_t y;
    bool ret;

    lv_img_transform_dsc_t dsc;
    dsc.cfg.angle = angle;
    dsc.cfg.zoom = zoom;
    dsc.cfg.src = img->data;
    dsc.cfg.src_w = img->header.w;
    dsc.cfg.src_h = img->header.h;
    dsc.cfg.cf = img->header.cf;
    dsc.cfg.pivot_x = pivot_x;
    dsc.cfg.pivot_y = pivot_y;
    dsc.cfg.color = color;
    dsc.cfg.antialias = antialias;
    _lv_img_buf_transform_init(&dsc);

    for(y = -offset_y; y < dest_height - offset_y; y++) {
        for(x = -offset_x; x < dest_width - offset_x; x++) {

            ret = _lv_img_buf_transform(&dsc, x, y);

            if(ret == false) continue;

            if(x + offset_x >= 0 && x + offset_x < dest_width && y + offset_y >= 0 && y + offset_y < dest_height) {
                /*If the image has no alpha channel just simple set the result color on the canvas*/
                if(lv_img_cf_has_alpha(img->header.cf) == false) {
                    lv_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y, dsc.res.color);
                }
                else {
                    lv_color_t bg_color = lv_img_buf_get_px_color(&ext_dst->dsc, x + offset_x, y + offset_y, dsc.cfg.color);

                    /*If the canvas has no alpha but the image has mix the image's color with
                     * canvas*/
                    if(lv_img_cf_has_alpha(ext_dst->dsc.header.cf) == false) {
                        if(dsc.res.opa < LV_OPA_MAX) dsc.res.color = lv_color_mix(dsc.res.color, bg_color, dsc.res.opa);
                        lv_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y, dsc.res.color);
                    }
                    /*Both the image and canvas has alpha channel. Some extra calculation is
                       required*/
                    else {
                        lv_opa_t bg_opa = lv_img_buf_get_px_alpha(&ext_dst->dsc, x + offset_x, y + offset_y);
                        /* Pick the foreground if it's fully opaque or the Background is fully
                         * transparent*/
                        if(dsc.res.opa >= LV_OPA_MAX || bg_opa <= LV_OPA_MIN) {
                            lv_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y, dsc.res.color);
                            lv_img_buf_set_px_alpha(&ext_dst->dsc, x + offset_x, y + offset_y, dsc.res.opa);
                        }
                        /*Opaque background: use simple mix*/
                        else if(bg_opa >= LV_OPA_MAX) {
                            lv_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y,
                                                    lv_color_mix(dsc.res.color, bg_color, dsc.res.opa));
                        }
                        /*Both colors have alpha. Expensive calculation need to be applied*/
                        else {

                            /*Info:
                             * https://en.wikipedia.org/wiki/Alpha_compositing#Analytical_derivation_of_the_over_operator*/
                            lv_opa_t opa_res_2 = 255 - ((uint16_t)((uint16_t)(255 - dsc.res.opa) * (255 - bg_opa)) >> 8);
                            if(opa_res_2 == 0) {
                                opa_res_2 = 1; /*never happens, just to be sure*/
                            }
                            lv_opa_t ratio = (uint16_t)((uint16_t)dsc.res.opa * 255) / opa_res_2;

                            lv_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y,
                                                    lv_color_mix(dsc.res.color, bg_color, ratio));
                            lv_img_buf_set_px_alpha(&ext_dst->dsc, x + offset_x, y + offset_y, opa_res_2);
                        }
                    }
                }
            }
        }
    }

    lv_obj_invalidate(canvas);
#else
    LV_UNUSED(canvas);
    LV_UNUSED(img);
    LV_UNUSED(angle);
    LV_UNUSED(zoom);
    LV_UNUSED(offset_x);
    LV_UNUSED(offset_y);
    LV_UNUSED(pivot_x);
    LV_UNUSED(pivot_y);
    LV_UNUSED(antialias);
    LV_LOG_WARN("LV_USE_IMG_TRANSFORM is disabled in lv_conf.h");
#endif
}


/**
 * Apply horizontal blur on the canvas
 * @param canvas pointer to a canvas object
 * @param area the area to blur. If `NULL` the whole canvas will be blurred.
 * @param r radius of the blur
 */
void lv_canvas_blur_hor(lv_obj_t * canvas, const lv_area_t * area, uint16_t r)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    if(r == 0) return;

    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    lv_area_t a;
    if(area) {
        lv_area_copy(&a, area);
        if(a.x1 < 0) a.x1 = 0;
        if(a.y1 < 0) a.y1 = 0;
        if(a.x2 > ext->dsc.header.w - 1) a.x2 = ext->dsc.header.w - 1;
        if(a.y2 > ext->dsc.header.h - 1) a.y2 = ext->dsc.header.h - 1;
    }
    else {
        a.x1 = 0;
        a.y1 = 0;
        a.x2 = ext->dsc.header.w - 1;
        a.y2 = ext->dsc.header.h - 1;
    }

    lv_color_t color = lv_obj_get_style_image_recolor(canvas, LV_CANVAS_PART_MAIN);

    uint16_t r_back = r / 2;
    uint16_t r_front = r / 2;

    if((r & 0x1) == 0) r_back--;

    bool has_alpha = lv_img_cf_has_alpha(ext->dsc.header.cf);

    lv_coord_t line_w = lv_img_buf_get_img_size(ext->dsc.header.w, 1, ext->dsc.header.cf);
    uint8_t * line_buf = _lv_mem_buf_get(line_w);

    lv_img_dsc_t line_img;
    line_img.data = line_buf;
    line_img.header.always_zero = 0;
    line_img.header.w = ext->dsc.header.w;
    line_img.header.h = 1;
    line_img.header.cf = ext->dsc.header.cf;

    lv_coord_t x;
    lv_coord_t y;
    lv_coord_t x_safe;

    for(y = a.y1; y <= a.y2; y++) {
        uint32_t asum = 0;
        uint32_t rsum = 0;
        uint32_t gsum = 0;
        uint32_t bsum = 0;

        lv_color_t c;
        lv_opa_t opa = LV_OPA_TRANSP;
        _lv_memcpy(line_buf, &ext->dsc.data[y * line_w], line_w);


        for(x = a.x1 - r_back; x <= a.x1 + r_front; x++) {
            x_safe = x < 0 ? 0 : x;
            x_safe = x_safe > ext->dsc.header.w - 1 ? ext->dsc.header.w - 1 : x_safe;

            c = lv_img_buf_get_px_color(&line_img, x_safe, 0, color);
            if(has_alpha) opa = lv_img_buf_get_px_alpha(&line_img, x_safe, 0);

            rsum += c.ch.red;
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
            gsum += (c.ch.green_h << 3) + c.ch.green_l;
#else
            gsum += c.ch.green;
#endif
            bsum += c.ch.blue;
            if(has_alpha) asum += opa;
        }

        /*Just to indicate that the px is visible*/
        if(has_alpha == false) asum = LV_OPA_COVER;

        for(x = a.x1; x <= a.x2; x++) {

            if(asum) {
                c.ch.red = rsum / r;
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
                uint8_t gtmp = gsum / r;
                c.ch.green_h = gtmp >> 3;
                c.ch.green_l = gtmp & 0x7;
#else
                c.ch.green = gsum / r;
#endif
                c.ch.blue = bsum / r;
                if(has_alpha) opa = asum / r;

                lv_img_buf_set_px_color(&ext->dsc, x, y, c);
            }
            if(has_alpha) lv_img_buf_set_px_alpha(&ext->dsc, x, y, opa);

            x_safe = x - r_back;
            x_safe = x_safe < 0 ? 0 : x_safe;
            c = lv_img_buf_get_px_color(&line_img, x_safe, 0, color);
            if(has_alpha) opa = lv_img_buf_get_px_alpha(&line_img, x_safe, 0);

            rsum -= c.ch.red;
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
            gsum -= (c.ch.green_h << 3) + c.ch.green_l;
#else
            gsum -= c.ch.green;
#endif
            bsum -= c.ch.blue;
            if(has_alpha) asum -= opa;

            x_safe = x + 1 + r_front;
            x_safe = x_safe > ext->dsc.header.w - 1 ? ext->dsc.header.w - 1 : x_safe;
            c = lv_img_buf_get_px_color(&line_img, x_safe, 0, LV_COLOR_RED);
            if(has_alpha) opa = lv_img_buf_get_px_alpha(&line_img, x_safe, 0);

            rsum += c.ch.red;
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
            gsum += (c.ch.green_h << 3) + c.ch.green_l;
#else
            gsum += c.ch.green;
#endif
            bsum += c.ch.blue;
            if(has_alpha) asum += opa;
        }
    }
    lv_obj_invalidate(canvas);

    _lv_mem_buf_release(line_buf);
}


/**
 * Apply vertical blur on the canvas
 * @param canvas pointer to a canvas object
 * @param area the area to blur. If `NULL` the whole canvas will be blurred.
 * @param r radius of the blur
 */
void lv_canvas_blur_ver(lv_obj_t * canvas, const lv_area_t * area, uint16_t r)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    if(r == 0) return;

    lv_canvas_ext_t * ext = lv_obj_get_ext_attr(canvas);

    lv_area_t a;
    if(area) {
        lv_area_copy(&a, area);
        if(a.x1 < 0) a.x1 = 0;
        if(a.y1 < 0) a.y1 = 0;
        if(a.x2 > ext->dsc.header.w - 1) a.x2 = ext->dsc.header.w - 1;
        if(a.y2 > ext->dsc.header.h - 1) a.y2 = ext->dsc.header.h - 1;
    }
    else {
        a.x1 = 0;
        a.y1 = 0;
        a.x2 = ext->dsc.header.w - 1;
        a.y2 = ext->dsc.header.h - 1;
    }

    lv_color_t color = lv_obj_get_style_image_recolor(canvas, LV_CANVAS_PART_MAIN);

    uint16_t r_back = r / 2;
    uint16_t r_front = r / 2;

    if((r & 0x1) == 0) r_back--;

    bool has_alpha = lv_img_cf_has_alpha(ext->dsc.header.cf);
    lv_coord_t col_w = lv_img_buf_get_img_size(1, ext->dsc.header.h, ext->dsc.header.cf);
    uint8_t * col_buf = _lv_mem_buf_get(col_w);
    lv_img_dsc_t line_img;

    line_img.data = col_buf;
    line_img.header.always_zero = 0;
    line_img.header.w = 1;
    line_img.header.h = ext->dsc.header.h;
    line_img.header.cf = ext->dsc.header.cf;

    lv_coord_t x;
    lv_coord_t y;
    lv_coord_t y_safe;

    for(x = a.x1; x <= a.x2; x++) {
        uint32_t asum = 0;
        uint32_t rsum = 0;
        uint32_t gsum = 0;
        uint32_t bsum = 0;

        lv_color_t c;
        lv_opa_t opa = LV_OPA_COVER;

        for(y = a.y1 - r_back; y <= a.y1 + r_front; y++) {
            y_safe = y < 0 ? 0 : y;
            y_safe = y_safe > ext->dsc.header.h - 1 ? ext->dsc.header.h - 1 : y_safe;

            c = lv_img_buf_get_px_color(&ext->dsc, x, y_safe, color);
            if(has_alpha) opa = lv_img_buf_get_px_alpha(&ext->dsc, x, y_safe);

            lv_img_buf_set_px_color(&line_img, 0, y_safe, c);
            if(has_alpha) lv_img_buf_set_px_alpha(&line_img, 0, y_safe, opa);

            rsum += c.ch.red;
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
            gsum += (c.ch.green_h << 3) + c.ch.green_l;
#else
            gsum += c.ch.green;
#endif
            bsum += c.ch.blue;
            if(has_alpha) asum += opa;
        }

        /*Just to indicate that the px is visible*/
        if(has_alpha == false) asum = LV_OPA_COVER;

        for(y = a.y1; y <= a.y2; y++) {
            if(asum) {
                c.ch.red = rsum / r;
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
                uint8_t gtmp = gsum / r;
                c.ch.green_h = gtmp >> 3;
                c.ch.green_l = gtmp & 0x7;
#else
                c.ch.green = gsum / r;
#endif
                c.ch.blue = bsum / r;
                if(has_alpha) opa = asum / r;

                lv_img_buf_set_px_color(&ext->dsc, x, y, c);
            }
            if(has_alpha) lv_img_buf_set_px_alpha(&ext->dsc, x, y, opa);

            y_safe = y - r_back;
            y_safe = y_safe < 0 ? 0 : y_safe;
            c = lv_img_buf_get_px_color(&line_img, 0, y_safe, color);
            if(has_alpha) opa = lv_img_buf_get_px_alpha(&line_img, 0, y_safe);

            rsum -= c.ch.red;
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
            gsum -= (c.ch.green_h << 3) + c.ch.green_l;
#else
            gsum -= c.ch.green;
#endif
            bsum -= c.ch.blue;
            if(has_alpha) asum -= opa;

            y_safe = y + 1 + r_front;
            y_safe = y_safe > ext->dsc.header.h - 1 ? ext->dsc.header.h - 1 : y_safe;

            c = lv_img_buf_get_px_color(&ext->dsc, x, y_safe, color);
            if(has_alpha) opa = lv_img_buf_get_px_alpha(&ext->dsc, x, y_safe);

            lv_img_buf_set_px_color(&line_img, 0, y_safe, c);
            if(has_alpha) lv_img_buf_set_px_alpha(&line_img, 0, y_safe, opa);

            rsum += c.ch.red;
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
            gsum += (c.ch.green_h << 3) + c.ch.green_l;
#else
            gsum += c.ch.green;
#endif
            bsum += c.ch.blue;
            if(has_alpha) asum += opa;
        }
    }

    lv_obj_invalidate(canvas);

    _lv_mem_buf_release(col_buf);
}

/**
 * Fill the canvas with color
 * @param canvas pointer to a canvas
 * @param color the background color
 * @param opa the desired opacity
 */
void lv_canvas_fill_bg(lv_obj_t * canvas, lv_color_t color, lv_opa_t opa)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf == LV_IMG_CF_INDEXED_1BIT) {
        uint32_t row_byte_cnt = (dsc->header.w + 7) >> 3;
        /*+8 skip the palette*/
        _lv_memset((uint8_t *)dsc->data + 8, color.full ? 0xff : 0x00, row_byte_cnt * dsc->header.h);
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT) {
        uint32_t row_byte_cnt = (dsc->header.w + 7) >> 3;
        _lv_memset((uint8_t *)dsc->data, opa > LV_OPA_50 ? 0xff : 0x00, row_byte_cnt * dsc->header.h);
    }
    else {
        uint32_t x;
        uint32_t y;
        for(y = 0; y < dsc->header.h; y++) {
            for(x = 0; x < dsc->header.w; x++) {
                lv_img_buf_set_px_color(dsc, x, y, color);
                lv_img_buf_set_px_alpha(dsc, x, y, opa);
            }
        }
    }

    lv_obj_invalidate(canvas);
}

/**
 * Draw a rectangle on the canvas
 * @param canvas pointer to a canvas object
 * @param x left coordinate of the rectangle
 * @param y top coordinate of the rectangle
 * @param w width of the rectangle
 * @param h height of the rectangle
 * @param rect_dsc descriptor of the rectangle
 */
void lv_canvas_draw_rect(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
                         const lv_draw_rect_dsc_t * rect_dsc)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_rect: can't raw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /* Create a dummy display to fool the lv_draw function.
     * It will think it draws to real screen. */
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_area_t coords;
    coords.x1 = x;
    coords.y1 = y;
    coords.x2 = x + w - 1;
    coords.y2 = y + h - 1;

    lv_disp_t disp;
    _lv_memset_00(&disp, sizeof(lv_disp_t));

    lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&disp_buf.area, &mask);

    lv_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

    set_set_px_cb(&disp.driver, dsc->header.cf);

#if LV_ANTIALIAS

    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    lv_color_t ctransp = LV_COLOR_TRANSP;
    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
       rect_dsc->bg_color.full == ctransp.full) {
        disp.driver.antialiasing = 0;
    }
#endif

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_rect(&coords, &mask, rect_dsc);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

/**
 * Draw a text on the canvas.
 * @param canvas pointer to a canvas object
 * @param x left coordinate of the text
 * @param y top coordinate of the text
 * @param max_w max width of the text. The text will be wrapped to fit into this size
 * @param label_draw_dsc pointer to a valid label descriptor `lv_draw_label_dsc_t`
 * @param txt text to display
 * @param align align of the text (`LV_LABEL_ALIGN_LEFT/RIGHT/CENTER`)
 */
void lv_canvas_draw_text(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t max_w,
                         lv_draw_label_dsc_t * label_draw_dsc,
                         const char * txt, lv_label_align_t align)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_text: can't raw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /* Create a dummy display to fool the lv_draw function.
     * It will think it draws to real screen. */
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_area_t coords;
    coords.x1 = x;
    coords.y1 = y;
    coords.x2 = x + max_w - 1;
    coords.y2 = dsc->header.h - 1;

    lv_disp_t disp;
    _lv_memset_00(&disp, sizeof(lv_disp_t));

    lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&disp_buf.area, &mask);

    lv_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

    set_set_px_cb(&disp.driver, dsc->header.cf);

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_txt_flag_t flag;
    switch(align) {
        case LV_LABEL_ALIGN_LEFT:
            flag = LV_TXT_FLAG_NONE;
            break;
        case LV_LABEL_ALIGN_RIGHT:
            flag = LV_TXT_FLAG_RIGHT;
            break;
        case LV_LABEL_ALIGN_CENTER:
            flag = LV_TXT_FLAG_CENTER;
            break;
        default:
            flag = LV_TXT_FLAG_NONE;
            break;
    }

    label_draw_dsc->flag = flag;

    lv_draw_label(&coords, &mask, label_draw_dsc, txt, NULL);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

/**
 * Draw an image on the canvas
 * @param canvas pointer to a canvas object
 * @param src image source. Can be a pointer an `lv_img_dsc_t` variable or a path an image.
 * @param img_draw_dsc pointer to a valid label descriptor `lv_draw_img_dsc_t`
 */
void lv_canvas_draw_img(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, const void * src,
                        const lv_draw_img_dsc_t * img_draw_dsc)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_img: can't raw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /* Create a dummy display to fool the lv_draw function.
     * It will think it draws to real screen. */
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_img_header_t header;
    lv_res_t res = lv_img_decoder_get_info(src, &header);
    if(res != LV_RES_OK) {
        LV_LOG_WARN("lv_canvas_draw_img: Couldn't get the image data.");
        return;
    }

    lv_area_t coords;
    coords.x1 = x;
    coords.y1 = y;
    coords.x2 = x + header.w - 1;
    coords.y2 = y + header.h - 1;

    lv_disp_t disp;
    _lv_memset_00(&disp, sizeof(lv_disp_t));

    lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&disp_buf.area, &mask);

    lv_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

    set_set_px_cb(&disp.driver, dsc->header.cf);

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_img(&coords, &mask, src, img_draw_dsc);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

/**
 * Draw a line on the canvas
 * @param canvas pointer to a canvas object
 * @param points point of the line
 * @param point_cnt number of points
 * @param line_draw_dsc pointer to an initialized `lv_draw_line_dsc_t` variable
 */
void lv_canvas_draw_line(lv_obj_t * canvas, const lv_point_t points[], uint32_t point_cnt,
                         const lv_draw_line_dsc_t * line_draw_dsc)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_line: can't raw to LV_IMG_CF_INDEXED canvas");
        return;
    }
    /* Create a dummy display to fool the lv_draw function.
     * It will think it draws to real screen. */
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_disp_t disp;
    _lv_memset_00(&disp, sizeof(lv_disp_t));

    lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&disp_buf.area, &mask);

    lv_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

    set_set_px_cb(&disp.driver, dsc->header.cf);

#if LV_ANTIALIAS
    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    lv_color_t ctransp = LV_COLOR_TRANSP;
    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
       line_draw_dsc->color.full == ctransp.full) {
        disp.driver.antialiasing = 0;
    }
#endif

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    uint32_t i;
    for(i = 0; i < point_cnt - 1; i++) {
        lv_draw_line(&points[i], &points[i + 1], &mask, line_draw_dsc);
    }

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

/**
 * Draw a polygon on the canvas
 * @param canvas pointer to a canvas object
 * @param points point of the polygon
 * @param point_cnt number of points
 * @param poly_draw_dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_canvas_draw_polygon(lv_obj_t * canvas, const lv_point_t points[], uint32_t point_cnt,
                            const lv_draw_rect_dsc_t * poly_draw_dsc)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_polygon: can't raw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /* Create a dummy display to fool the lv_draw function.
     * It will think it draws to real screen. */
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_disp_t disp;
    _lv_memset_00(&disp, sizeof(lv_disp_t));

    lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&disp_buf.area, &mask);

    lv_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

    set_set_px_cb(&disp.driver, dsc->header.cf);

#if LV_ANTIALIAS
    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    lv_color_t ctransp = LV_COLOR_TRANSP;
    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
       poly_draw_dsc->bg_color.full == ctransp.full) {
        disp.driver.antialiasing = 0;
    }
#endif

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_polygon(points, point_cnt, &mask, poly_draw_dsc);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

/**
 * Draw an arc on the canvas
 * @param canvas pointer to a canvas object
 * @param x origo x  of the arc
 * @param y origo y of the arc
 * @param r radius of the arc
 * @param start_angle start angle in degrees
 * @param end_angle end angle in degrees
 * @param arc_draw_dsc pointer to an initialized `lv_draw_line_dsc_t` variable
 */
void lv_canvas_draw_arc(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t r, int32_t start_angle,
                        int32_t end_angle, const lv_draw_line_dsc_t * arc_draw_dsc)
{
    LV_ASSERT_OBJ(canvas, LV_OBJX_NAME);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_arc: can't raw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /* Create a dummy display to fool the lv_draw function.
     * It will think it draws to real screen. */
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_disp_t disp;
    _lv_memset_00(&disp, sizeof(lv_disp_t));

    lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&disp_buf.area, &mask);

    lv_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

    set_set_px_cb(&disp.driver, dsc->header.cf);

#if LV_ANTIALIAS
    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    lv_color_t ctransp = LV_COLOR_TRANSP;
    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
       arc_draw_dsc->color.full == ctransp.full) {
        disp.driver.antialiasing = 0;
    }
#endif

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_arc(x, y, r,  start_angle, end_angle, &mask, arc_draw_dsc);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
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
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}

static void set_set_px_cb(lv_disp_drv_t * disp_drv, lv_img_cf_t cf)
{
    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
            disp_drv->set_px_cb = set_px_true_color_alpha;
            break;
        case LV_IMG_CF_ALPHA_1BIT:
            disp_drv->set_px_cb = set_px_cb_alpha1;
            break;
        case LV_IMG_CF_ALPHA_2BIT:
            disp_drv->set_px_cb = set_px_cb_alpha2;
            break;
        case LV_IMG_CF_ALPHA_4BIT:
            disp_drv->set_px_cb = set_px_cb_alpha4;
            break;
        case LV_IMG_CF_ALPHA_8BIT:
            disp_drv->set_px_cb = set_px_cb_alpha8;
            break;
        default:
            disp_drv->set_px_cb = NULL;
    }
}

static void set_px_cb_alpha1(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa)
{
    (void) disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN) return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_ALPHA_1BIT;

    set_px_alpha_generic(&d, x, y, color, opa);
}

static void set_px_cb_alpha2(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa)
{
    (void) disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN) return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_ALPHA_2BIT;

    set_px_alpha_generic(&d, x, y, color, opa);
}

static void set_px_cb_alpha4(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa)
{
    (void) disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN) return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_ALPHA_4BIT;

    set_px_alpha_generic(&d, x, y, color, opa);
}

static void set_px_cb_alpha8(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa)
{
    (void) disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN) return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_ALPHA_8BIT;

    set_px_alpha_generic(&d, x, y, color, opa);
}

static void set_px_alpha_generic(lv_img_dsc_t * d, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
{
    d->header.always_zero = 0;
    d->header.h = LV_VER_RES_MAX;

    uint8_t br = lv_color_brightness(color);
    if(opa < LV_OPA_MAX) {
        uint8_t bg = lv_img_buf_get_px_alpha(d, x, y);
        br = (uint16_t)((uint16_t)br * opa + (bg * (255 - opa))) >> 8;
    }

    lv_img_buf_set_px_alpha(d, x, y, br);
}


static void set_px_true_color_alpha(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x,
                                    lv_coord_t y,
                                    lv_color_t color, lv_opa_t opa)
{
    (void) disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN) return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.always_zero = 0;
    d.header.h = LV_VER_RES_MAX;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;

    lv_color_t bg_color = lv_img_buf_get_px_color(&d, x, y, LV_COLOR_BLACK);
    lv_opa_t bg_opa = lv_img_buf_get_px_alpha(&d, x, y);

    lv_opa_t res_opa;
    lv_color_t res_color;

    lv_color_mix_with_alpha(bg_color, bg_opa, color, opa, &res_color, &res_opa);


    lv_img_buf_set_px_alpha(&d, x, y, res_opa);
    lv_img_buf_set_px_color(&d, x, y, res_color);
}

#endif
