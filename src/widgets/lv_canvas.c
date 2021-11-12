/**
 * @file lv_canvas.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_canvas.h"
#include "../misc/lv_assert.h"
#include "../misc/lv_math.h"
#include "../draw/lv_draw.h"
#include "../core/lv_refr.h"

#if LV_USE_CANVAS != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_canvas_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_canvas_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_canvas_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_canvas_class = {
    .constructor_cb = lv_canvas_constructor,
    .destructor_cb = lv_canvas_destructor,
    .instance_size = sizeof(lv_canvas_t),
    .base_class = &lv_img_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_canvas_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_canvas_set_buffer(lv_obj_t * obj, void * buf, lv_coord_t w, lv_coord_t h, lv_img_cf_t cf)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(buf);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    canvas->dsc.header.cf = cf;
    canvas->dsc.header.w  = w;
    canvas->dsc.header.h  = h;
    canvas->dsc.data      = buf;

    lv_img_set_src(obj, &canvas->dsc);
}

void lv_canvas_set_px_color(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_color_t c)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    lv_img_buf_set_px_color(&canvas->dsc, x, y, c);
    lv_obj_invalidate(obj);
}

void lv_canvas_set_px_opa(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_opa_t opa)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    lv_img_buf_set_px_alpha(&canvas->dsc, x, y, opa);
    lv_obj_invalidate(obj);
}

void lv_canvas_set_palette(lv_obj_t * obj, uint8_t id, lv_color_t c)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    lv_img_buf_set_palette(&canvas->dsc, id, c);
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_color_t lv_canvas_get_px(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    lv_color_t color = lv_obj_get_style_img_recolor(obj, LV_PART_MAIN);

    return lv_img_buf_get_px_color(&canvas->dsc, x, y, color);
}

lv_img_dsc_t * lv_canvas_get_img(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    return &canvas->dsc;
}

/*=====================
 * Other functions
 *====================*/

void lv_canvas_copy_buf(lv_obj_t * obj, const void * to_copy, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(to_copy);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    if(x + w >= (lv_coord_t)canvas->dsc.header.w || y + h >= (lv_coord_t)canvas->dsc.header.h) {
        LV_LOG_WARN("lv_canvas_copy_buf: x or y out of the canvas");
        return;
    }

    uint32_t px_size   = lv_img_cf_get_px_size(canvas->dsc.header.cf) >> 3;
    uint32_t px        = canvas->dsc.header.w * y * px_size + x * px_size;
    uint8_t * to_copy8 = (uint8_t *)to_copy;
    lv_coord_t i;
    for(i = 0; i < h; i++) {
        lv_memcpy((void *)&canvas->dsc.data[px], to_copy8, w * px_size);
        px += canvas->dsc.header.w * px_size;
        to_copy8 += w * px_size;
    }
}

void lv_canvas_transform(lv_obj_t * obj, lv_img_dsc_t * img, int16_t angle, uint16_t zoom, lv_coord_t offset_x,
                         lv_coord_t offset_y,
                         int32_t pivot_x, int32_t pivot_y, bool antialias)
{
#if LV_DRAW_COMPLEX
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(img);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    lv_color_t color = lv_obj_get_style_img_recolor(obj, LV_PART_MAIN);

    int32_t dest_width  = canvas->dsc.header.w;
    int32_t dest_height = canvas->dsc.header.h;

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
                    lv_img_buf_set_px_color(&canvas->dsc, x + offset_x, y + offset_y, dsc.res.color);
                }
                else {
                    lv_color_t bg_color = lv_img_buf_get_px_color(&canvas->dsc, x + offset_x, y + offset_y, dsc.cfg.color);

                    /*If the canvas has no alpha but the image has mix the image's color with
                     * canvas*/
                    if(lv_img_cf_has_alpha(canvas->dsc.header.cf) == false) {
                        if(dsc.res.opa < LV_OPA_MAX) dsc.res.color = lv_color_mix(dsc.res.color, bg_color, dsc.res.opa);
                        lv_img_buf_set_px_color(&canvas->dsc, x + offset_x, y + offset_y, dsc.res.color);
                    }
                    /*Both the image and canvas has alpha channel. Some extra calculation is
                       required*/
                    else {
                        lv_opa_t bg_opa = lv_img_buf_get_px_alpha(&canvas->dsc, x + offset_x, y + offset_y);
                        /*Pick the foreground if it's fully opaque or the Background is fully
                         *transparent*/
                        if(dsc.res.opa >= LV_OPA_MAX || bg_opa <= LV_OPA_MIN) {
                            lv_img_buf_set_px_color(&canvas->dsc, x + offset_x, y + offset_y, dsc.res.color);
                            lv_img_buf_set_px_alpha(&canvas->dsc, x + offset_x, y + offset_y, dsc.res.opa);
                        }
                        /*Opaque background: use simple mix*/
                        else if(bg_opa >= LV_OPA_MAX) {
                            lv_img_buf_set_px_color(&canvas->dsc, x + offset_x, y + offset_y,
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

                            lv_img_buf_set_px_color(&canvas->dsc, x + offset_x, y + offset_y,
                                                    lv_color_mix(dsc.res.color, bg_color, ratio));
                            lv_img_buf_set_px_alpha(&canvas->dsc, x + offset_x, y + offset_y, opa_res_2);
                        }
                    }
                }
            }
        }
    }

    lv_obj_invalidate(obj);
#else
    LV_UNUSED(obj);
    LV_UNUSED(img);
    LV_UNUSED(angle);
    LV_UNUSED(zoom);
    LV_UNUSED(offset_x);
    LV_UNUSED(offset_y);
    LV_UNUSED(pivot_x);
    LV_UNUSED(pivot_y);
    LV_UNUSED(antialias);
    LV_LOG_WARN("Can't transform canvas with LV_DRAW_COMPLEX == 0");
#endif
}

void lv_canvas_blur_hor(lv_obj_t * obj, const lv_area_t * area, uint16_t r)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(r == 0) return;

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    lv_area_t a;
    if(area) {
        lv_area_copy(&a, area);
        if(a.x1 < 0) a.x1 = 0;
        if(a.y1 < 0) a.y1 = 0;
        if(a.x2 > canvas->dsc.header.w - 1) a.x2 = canvas->dsc.header.w - 1;
        if(a.y2 > canvas->dsc.header.h - 1) a.y2 = canvas->dsc.header.h - 1;
    }
    else {
        a.x1 = 0;
        a.y1 = 0;
        a.x2 = canvas->dsc.header.w - 1;
        a.y2 = canvas->dsc.header.h - 1;
    }

    lv_color_t color = lv_obj_get_style_img_recolor(obj, LV_PART_MAIN);

    uint16_t r_back = r / 2;
    uint16_t r_front = r / 2;

    if((r & 0x1) == 0) r_back--;

    bool has_alpha = lv_img_cf_has_alpha(canvas->dsc.header.cf);

    lv_coord_t line_w = lv_img_buf_get_img_size(canvas->dsc.header.w, 1, canvas->dsc.header.cf);
    uint8_t * line_buf = lv_mem_buf_get(line_w);

    lv_img_dsc_t line_img;
    line_img.data = line_buf;
    line_img.header.always_zero = 0;
    line_img.header.w = canvas->dsc.header.w;
    line_img.header.h = 1;
    line_img.header.cf = canvas->dsc.header.cf;

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
        lv_memcpy(line_buf, &canvas->dsc.data[y * line_w], line_w);

        for(x = a.x1 - r_back; x <= a.x1 + r_front; x++) {
            x_safe = x < 0 ? 0 : x;
            x_safe = x_safe > canvas->dsc.header.w - 1 ? canvas->dsc.header.w - 1 : x_safe;

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

                lv_img_buf_set_px_color(&canvas->dsc, x, y, c);
            }
            if(has_alpha) lv_img_buf_set_px_alpha(&canvas->dsc, x, y, opa);

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
            x_safe = x_safe > canvas->dsc.header.w - 1 ? canvas->dsc.header.w - 1 : x_safe;
            c = lv_img_buf_get_px_color(&line_img, x_safe, 0, lv_color_white());
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
    lv_obj_invalidate(obj);

    lv_mem_buf_release(line_buf);
}

void lv_canvas_blur_ver(lv_obj_t * obj, const lv_area_t * area, uint16_t r)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(r == 0) return;

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    lv_area_t a;
    if(area) {
        lv_area_copy(&a, area);
        if(a.x1 < 0) a.x1 = 0;
        if(a.y1 < 0) a.y1 = 0;
        if(a.x2 > canvas->dsc.header.w - 1) a.x2 = canvas->dsc.header.w - 1;
        if(a.y2 > canvas->dsc.header.h - 1) a.y2 = canvas->dsc.header.h - 1;
    }
    else {
        a.x1 = 0;
        a.y1 = 0;
        a.x2 = canvas->dsc.header.w - 1;
        a.y2 = canvas->dsc.header.h - 1;
    }

    lv_color_t color = lv_obj_get_style_img_recolor(obj, LV_PART_MAIN);

    uint16_t r_back = r / 2;
    uint16_t r_front = r / 2;

    if((r & 0x1) == 0) r_back--;

    bool has_alpha = lv_img_cf_has_alpha(canvas->dsc.header.cf);
    lv_coord_t col_w = lv_img_buf_get_img_size(1, canvas->dsc.header.h, canvas->dsc.header.cf);
    uint8_t * col_buf = lv_mem_buf_get(col_w);
    lv_img_dsc_t line_img;

    line_img.data = col_buf;
    line_img.header.always_zero = 0;
    line_img.header.w = 1;
    line_img.header.h = canvas->dsc.header.h;
    line_img.header.cf = canvas->dsc.header.cf;

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
            y_safe = y_safe > canvas->dsc.header.h - 1 ? canvas->dsc.header.h - 1 : y_safe;

            c = lv_img_buf_get_px_color(&canvas->dsc, x, y_safe, color);
            if(has_alpha) opa = lv_img_buf_get_px_alpha(&canvas->dsc, x, y_safe);

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

                lv_img_buf_set_px_color(&canvas->dsc, x, y, c);
            }
            if(has_alpha) lv_img_buf_set_px_alpha(&canvas->dsc, x, y, opa);

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
            y_safe = y_safe > canvas->dsc.header.h - 1 ? canvas->dsc.header.h - 1 : y_safe;

            c = lv_img_buf_get_px_color(&canvas->dsc, x, y_safe, color);
            if(has_alpha) opa = lv_img_buf_get_px_alpha(&canvas->dsc, x, y_safe);

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

    lv_obj_invalidate(obj);

    lv_mem_buf_release(col_buf);
}

void lv_canvas_fill_bg(lv_obj_t * canvas, lv_color_t color, lv_opa_t opa)
{
    LV_ASSERT_OBJ(canvas, MY_CLASS);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf == LV_IMG_CF_INDEXED_1BIT) {
        uint32_t row_byte_cnt = (dsc->header.w + 7) >> 3;
        /*+8 skip the palette*/
        lv_memset((uint8_t *)dsc->data + 8, color.full ? 0xff : 0x00, row_byte_cnt * dsc->header.h);
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT) {
        uint32_t row_byte_cnt = (dsc->header.w + 7) >> 3;
        lv_memset((uint8_t *)dsc->data, opa > LV_OPA_50 ? 0xff : 0x00, row_byte_cnt * dsc->header.h);
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

void lv_canvas_draw_rect(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
                         const lv_draw_rect_dsc_t * draw_dsc)
{
    LV_ASSERT_OBJ(canvas, MY_CLASS);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_rect: can't draw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /*Create a dummy display to fool the lv_draw function.
     *It will think it draws to real screen.*/
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
    /*Allocate the fake driver on the stack as the entire display doesn't outlive this function*/
    lv_disp_drv_t driver;
    lv_memset_00(&disp, sizeof(lv_disp_t));
    disp.driver = &driver;

    lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&draw_buf.area, &mask);

    lv_disp_drv_init(disp.driver);

    disp.driver->draw_buf = &draw_buf;
    disp.driver->hor_res = dsc->header.w;
    disp.driver->ver_res = dsc->header.h;

    lv_disp_drv_use_generic_set_px_cb(disp.driver, dsc->header.cf);

    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    lv_color_t ctransp = LV_COLOR_CHROMA_KEY;
    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
       draw_dsc->bg_color.full == ctransp.full) {
        disp.driver->antialiasing = 0;
    }

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_rect(&coords, &mask, draw_dsc);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

void lv_canvas_draw_text(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t max_w,
                         lv_draw_label_dsc_t * draw_dsc, const char * txt)
{
    LV_ASSERT_OBJ(canvas, MY_CLASS);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_text: can't draw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /*Create a dummy display to fool the lv_draw function.
     *It will think it draws to real screen.*/
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
    /*Allocate the fake driver on the stack as the entire display doesn't outlive this function*/
    lv_disp_drv_t driver;
    lv_memset_00(&disp, sizeof(lv_disp_t));
    disp.driver = &driver;

    lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&draw_buf.area, &mask);

    lv_disp_drv_init(disp.driver);

    disp.driver->draw_buf = &draw_buf;
    disp.driver->hor_res = dsc->header.w;
    disp.driver->ver_res = dsc->header.h;

    lv_disp_drv_use_generic_set_px_cb(disp.driver, dsc->header.cf);

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_label(&coords, &mask, draw_dsc, txt, NULL);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

void lv_canvas_draw_img(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, const void * src,
                        const lv_draw_img_dsc_t * draw_dsc)
{
    LV_ASSERT_OBJ(canvas, MY_CLASS);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_img: can't draw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /*Create a dummy display to fool the lv_draw function.
     *It will think it draws to real screen.*/
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
    /*Allocate the fake driver on the stack as the entire display doesn't outlive this function*/
    lv_disp_drv_t driver;
    lv_memset_00(&disp, sizeof(lv_disp_t));
    disp.driver = &driver;

    lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&draw_buf.area, &mask);

    lv_disp_drv_init(disp.driver);

    disp.driver->draw_buf = &draw_buf;
    disp.driver->hor_res = dsc->header.w;
    disp.driver->ver_res = dsc->header.h;

    lv_disp_drv_use_generic_set_px_cb(disp.driver, dsc->header.cf);

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_img(&coords, &mask, src, draw_dsc);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

void lv_canvas_draw_line(lv_obj_t * canvas, const lv_point_t points[], uint32_t point_cnt,
                         const lv_draw_line_dsc_t * draw_dsc)
{
    LV_ASSERT_OBJ(canvas, MY_CLASS);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_line: can't draw to LV_IMG_CF_INDEXED canvas");
        return;
    }
    /*Create a dummy display to fool the lv_draw function.
     *It will think it draws to real screen.*/
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_disp_t disp;
    /*Allocate the fake driver on the stack as the entire display doesn't outlive this function*/
    lv_disp_drv_t driver;
    lv_memset_00(&disp, sizeof(lv_disp_t));
    disp.driver = &driver;

    lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&draw_buf.area, &mask);

    lv_disp_drv_init(disp.driver);

    disp.driver->draw_buf = &draw_buf;
    disp.driver->hor_res = dsc->header.w;
    disp.driver->ver_res = dsc->header.h;

    lv_disp_drv_use_generic_set_px_cb(disp.driver, dsc->header.cf);

    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    lv_color_t ctransp = LV_COLOR_CHROMA_KEY;
    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
       draw_dsc->color.full == ctransp.full) {
        disp.driver->antialiasing = 0;
    }

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    uint32_t i;
    for(i = 0; i < point_cnt - 1; i++) {
        lv_draw_line(&points[i], &points[i + 1], &mask, draw_dsc);
    }

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

void lv_canvas_draw_polygon(lv_obj_t * canvas, const lv_point_t points[], uint32_t point_cnt,
                            const lv_draw_rect_dsc_t * draw_dsc)
{
    LV_ASSERT_OBJ(canvas, MY_CLASS);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_polygon: can't draw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /*Create a dummy display to fool the lv_draw function.
     *It will think it draws to real screen.*/
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_disp_t disp;
    /*Allocate the fake driver on the stack as the entire display doesn't outlive this function*/
    lv_disp_drv_t driver;
    lv_memset_00(&disp, sizeof(lv_disp_t));
    disp.driver = &driver;

    lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&draw_buf.area, &mask);

    lv_disp_drv_init(disp.driver);

    disp.driver->draw_buf = &draw_buf;
    disp.driver->hor_res = dsc->header.w;
    disp.driver->ver_res = dsc->header.h;

    lv_disp_drv_use_generic_set_px_cb(disp.driver, dsc->header.cf);

    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    lv_color_t ctransp = LV_COLOR_CHROMA_KEY;
    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
       draw_dsc->bg_color.full == ctransp.full) {
        disp.driver->antialiasing = 0;
    }

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_polygon(points, point_cnt, &mask, draw_dsc);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
}

void lv_canvas_draw_arc(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, lv_coord_t r, int32_t start_angle,
                        int32_t end_angle, const lv_draw_arc_dsc_t * draw_dsc)
{
#if LV_DRAW_COMPLEX
    LV_ASSERT_OBJ(canvas, MY_CLASS);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);

    if(dsc->header.cf >= LV_IMG_CF_INDEXED_1BIT && dsc->header.cf <= LV_IMG_CF_INDEXED_8BIT) {
        LV_LOG_WARN("lv_canvas_draw_arc: can't draw to LV_IMG_CF_INDEXED canvas");
        return;
    }

    /*Create a dummy display to fool the lv_draw function.
     *It will think it draws to real screen.*/
    lv_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    lv_disp_t disp;
    /*Allocate the fake driver on the stack as the entire display doesn't outlive this function*/
    lv_disp_drv_t driver;
    lv_memset_00(&disp, sizeof(lv_disp_t));
    disp.driver = &driver;

    lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    lv_area_copy(&draw_buf.area, &mask);

    lv_disp_drv_init(disp.driver);

    disp.driver->draw_buf = &draw_buf;
    disp.driver->hor_res = dsc->header.w;
    disp.driver->ver_res = dsc->header.h;

    lv_disp_drv_use_generic_set_px_cb(disp.driver, dsc->header.cf);

    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    lv_color_t ctransp = LV_COLOR_CHROMA_KEY;
    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
       draw_dsc->color.full == ctransp.full) {
        disp.driver->antialiasing = 0;
    }

    lv_disp_t * refr_ori = _lv_refr_get_disp_refreshing();
    _lv_refr_set_disp_refreshing(&disp);

    lv_draw_arc(x, y, r,  start_angle, end_angle, &mask, draw_dsc);

    _lv_refr_set_disp_refreshing(refr_ori);

    lv_obj_invalidate(canvas);
#else
    LV_UNUSED(canvas);
    LV_UNUSED(x);
    LV_UNUSED(y);
    LV_UNUSED(r);
    LV_UNUSED(start_angle);
    LV_UNUSED(end_angle);
    LV_UNUSED(draw_dsc);
    LV_LOG_WARN("Can't draw arc with LV_DRAW_COMPLEX == 0");
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_canvas_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    canvas->dsc.header.always_zero = 0;
    canvas->dsc.header.cf          = LV_IMG_CF_TRUE_COLOR;
    canvas->dsc.header.h           = 0;
    canvas->dsc.header.w           = 0;
    canvas->dsc.data_size          = 0;
    canvas->dsc.data               = NULL;

    lv_img_set_src(obj, &canvas->dsc);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_canvas_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    lv_img_cache_invalidate_src(&canvas->dsc);
}

#endif
