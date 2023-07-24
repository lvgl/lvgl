/**
 * @file lv_canvas.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_canvas.h"
#if LV_USE_CANVAS != 0
#include "../../misc/lv_assert.h"
#include "../../misc/lv_math.h"
#include "../../draw/lv_draw.h"
#include "../../core/lv_refr.h"
#include "../../disp/lv_disp.h"
#include "../../draw/sw/lv_draw_sw.h"
#include "../../stdlib/lv_string.h"

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

void lv_canvas_set_buffer(lv_obj_t * obj, void * buf, lv_coord_t w, lv_coord_t h, lv_color_format_t cf)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(buf);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    canvas->dsc.header.cf = cf;
    canvas->dsc.header.w  = w;
    canvas->dsc.header.h  = h;
    canvas->dsc.data      = buf;
    canvas->dsc.data_size = w * h * lv_color_format_get_size(cf);

    lv_img_set_src(obj, &canvas->dsc);
    lv_img_cache_invalidate_src(&canvas->dsc);
}

void lv_canvas_set_px(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    if(LV_COLOR_FORMAT_IS_INDEXED(canvas->dsc.header.cf)) {
        uint32_t stride = (canvas->dsc.header.w + 7) >> 3;
        uint8_t * buf = (uint8_t *)canvas->dsc.data;
        buf += 8;
        buf += y * stride;
        buf += x >> 3;
        uint32_t bit = 7 - (x & 0x7);
        uint32_t c_int = color.blue;

        *buf &= ~(1 << bit);
        *buf |= c_int << bit;
    }
    else if(canvas->dsc.header.cf == LV_COLOR_FORMAT_A8) {
        uint8_t * buf = (uint8_t *)canvas->dsc.data;
        buf += canvas->dsc.header.w * y + x;
        *buf = opa;
    }
    else if(canvas->dsc.header.cf == LV_COLOR_FORMAT_RGB565) {
        lv_color16_t * buf = (lv_color16_t *)canvas->dsc.data;
        buf += canvas->dsc.header.w * y + x;
        buf->red = color.red >> 3;
        buf->green = color.green >> 2;
        buf->blue = color.blue >> 3;
    }
    else if(canvas->dsc.header.cf == LV_COLOR_FORMAT_RGB888) {
        uint8_t * buf = (uint8_t *)canvas->dsc.data;
        buf += canvas->dsc.header.w * y * 3 + x * 3;
        buf[2] = color.red;
        buf[1] = color.green;
        buf[0] = color.blue;
    }
    else if(canvas->dsc.header.cf == LV_COLOR_FORMAT_XRGB8888) {
        uint8_t * buf = (uint8_t *)canvas->dsc.data;
        buf += canvas->dsc.header.w * y * 4 + x * 4;
        buf[2] = color.red;
        buf[1] = color.green;
        buf[0] = color.blue;
        buf[3] = 0xFF;
    }
    else if(canvas->dsc.header.cf == LV_COLOR_FORMAT_ARGB8888) {
        lv_color32_t * buf = (lv_color32_t *)canvas->dsc.data;
        buf += canvas->dsc.header.w * y + x;
        buf->red = color.red;
        buf->green = color.green;
        buf->blue = color.blue;
        buf->alpha = opa;
    }
    lv_obj_invalidate(obj);
}

void lv_canvas_set_palette(lv_obj_t * obj, uint8_t id, lv_color32_t c)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;

    lv_img_buf_set_palette(&canvas->dsc, id, c);
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_color32_t lv_canvas_get_px(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    uint8_t px_size = lv_color_format_get_size(canvas->dsc.header.cf);
    const uint8_t * px = canvas->dsc.data + canvas->dsc.header.w * y * px_size + x * px_size;
    lv_color32_t ret;

    switch(canvas->dsc.header.cf) {
        case LV_COLOR_FORMAT_ARGB8888:
            ret.red = px[0];
            ret.green = px[1];
            ret.blue = px[2];
            ret.alpha = px[3];
            break;
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_XRGB8888:
            ret.red = px[0];
            ret.green = px[1];
            ret.blue = px[2];
            ret.alpha = 0xFF;
            break;
        case LV_COLOR_FORMAT_RGB565: {
                lv_color16_t * c16 = (lv_color16_t *) px;
                ret.red = (c16[x].red * 2106) >> 8;  /*To make it rounded*/
                ret.green = (c16[x].green * 1037) >> 8;
                ret.blue = (c16[x].blue * 2106) >> 8;
                ret.alpha = 0xFF;
                break;
            }
        case LV_COLOR_FORMAT_A8: {
                lv_color_t alpha_color = lv_obj_get_style_img_recolor(obj, LV_PART_MAIN);
                ret.red = alpha_color.red;
                ret.green = alpha_color.green;
                ret.blue = alpha_color.blue;
                ret.alpha = px[0];
                break;
            }
        default:
            lv_memzero(&ret, sizeof(lv_color32_t));
            break;
    }

    return ret;
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

    if(x + w - 1 >= (lv_coord_t)canvas->dsc.header.w || y + h - 1 >= (lv_coord_t)canvas->dsc.header.h) {
        LV_LOG_WARN("x or y out of the canvas");
        return;
    }

    uint32_t px_size   = lv_color_format_get_size(canvas->dsc.header.cf) >> 3;
    uint32_t px        = canvas->dsc.header.w * y * px_size + x * px_size;
    uint8_t * to_copy8 = (uint8_t *)to_copy;
    lv_coord_t i;
    for(i = 0; i < h; i++) {
        lv_memcpy((void *)&canvas->dsc.data[px], to_copy8, w * px_size);
        px += canvas->dsc.header.w * px_size;
        to_copy8 += w * px_size;
    }
}

void lv_canvas_fill_bg(lv_obj_t * obj, lv_color_t color, lv_opa_t opa)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_img_dsc_t * dsc = lv_canvas_get_img(obj);

    uint32_t x;
    uint32_t y;

    if(dsc->header.cf == LV_COLOR_FORMAT_RGB565) {
        uint16_t c16 = lv_color_to_u16(color);
        for(y = 0; y < dsc->header.h; y++) {
            uint16_t * buf16 = (uint16_t *)(dsc->data + y * dsc->header.w * 2);
            for(x = 0; x < dsc->header.w; x++) {
                buf16[x] = c16;
            }
        }
    }
    else if(dsc->header.cf == LV_COLOR_FORMAT_XRGB8888 || dsc->header.cf == LV_COLOR_FORMAT_ARGB8888) {
        uint32_t c32 = lv_color_to_u32(color);
        if(dsc->header.cf == LV_COLOR_FORMAT_ARGB8888) {
            c32 &= 0x00ffffff;
            c32 |= opa << 24;
        }
        for(y = 0; y < dsc->header.h; y++) {
            uint32_t * buf32 = (uint32_t *)(dsc->data + y * dsc->header.w * 4);
            for(x = 0; x < dsc->header.w; x++) {
                buf32[x] = c32;
            }
        }
    }
    else if(dsc->header.cf == LV_COLOR_FORMAT_RGB888) {
        for(y = 0; y < dsc->header.h; y++) {
            uint8_t * buf8 = (uint8_t *)(dsc->data + y * dsc->header.w * 3);
            for(x = 0; x < dsc->header.w * 3; x += 3) {
                buf8[x + 0] = color.blue;
                buf8[x + 1] = color.green;
                buf8[x + 2] = color.red;
            }
        }
    }
    else {
        for(y = 0; y < dsc->header.h; y++) {
            for(x = 0; x < dsc->header.w; x++) {
                lv_canvas_set_px(obj, x, y, color, opa);
            }
        }
    }

    lv_obj_invalidate(obj);
}



void lv_canvas_init_layer(lv_obj_t * canvas, lv_layer_t * layer)
{
    LV_ASSERT_NULL(canvas);
    LV_ASSERT_NULL(layer);

    lv_img_dsc_t * dsc = lv_canvas_get_img(canvas);
    lv_area_t canvas_area = {0, 0, dsc->header.w - 1,  dsc->header.h - 1};
    lv_memzero(layer, sizeof(*layer));

    layer->color_format = dsc->header.cf;
    layer->buf_area = canvas_area;
    layer->clip_area = canvas_area;
    layer->buf = (uint8_t *)dsc->data;
}


void lv_canvas_finish_layer(lv_obj_t * canvas, lv_layer_t * layer)
{
    while(layer->draw_task_head) {
        lv_draw_dispatch_wait_for_request();
        lv_draw_dispatch_layer(lv_obj_get_disp(canvas), layer);
    }
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
    canvas->dsc.header.cf          = LV_COLOR_FORMAT_NATIVE;
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
