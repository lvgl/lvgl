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
#include "../../display/lv_display.h"
#include "../../draw/sw/lv_draw_sw.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/cache/lv_cache.h"
/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_canvas_class)

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
    .base_class = &lv_image_class,
    .name = "canvas",
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

void lv_canvas_set_buffer(lv_obj_t * obj, void * buf, int32_t w, int32_t h, lv_color_format_t cf)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(buf);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    uint32_t stride = lv_draw_buf_width_to_stride(w, cf);
    lv_draw_buf_init(&canvas->static_buf, w, h, cf, stride, buf, stride * h);
    canvas->draw_buf = &canvas->static_buf;

    const void * src = lv_image_get_src(obj);
    if(src) {
        lv_image_cache_drop(src);
    }

    lv_image_set_src(obj, canvas->draw_buf);
    lv_image_cache_drop(canvas->draw_buf);
}

void lv_canvas_set_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(draw_buf);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    canvas->draw_buf = draw_buf;

    const void * src = lv_image_get_src(obj);
    if(src) {
        lv_image_cache_drop(src);
    }

    lv_image_set_src(obj, draw_buf);
    lv_image_cache_drop(draw_buf);
}

void lv_canvas_set_px(lv_obj_t * obj, int32_t x, int32_t y, lv_color_t color, lv_opa_t opa)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    lv_draw_buf_t * draw_buf = canvas->draw_buf;

    lv_color_format_t cf = draw_buf->header.cf;
    uint32_t stride = draw_buf->header.stride;
    uint8_t * data = lv_draw_buf_goto_xy(draw_buf, x, y);

    if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        /*Indexed image bpp could be less than 8, calculate again*/
        uint8_t * buf = (uint8_t *)canvas->draw_buf->data;
        buf += 8;
        buf += y * stride;
        buf += x >> 3;
        uint32_t bit = 7 - (x & 0x7);
        uint32_t c_int = color.blue;

        *buf &= ~(1 << bit);
        *buf |= c_int << bit;
    }
    else if(cf == LV_COLOR_FORMAT_A8) {
        *data = opa;
    }
    else if(cf == LV_COLOR_FORMAT_RGB565) {
        lv_color16_t * buf = (lv_color16_t *)data;
        buf->red = color.red >> 3;
        buf->green = color.green >> 2;
        buf->blue = color.blue >> 3;
    }
    else if(cf == LV_COLOR_FORMAT_RGB888) {
        data[2] = color.red;
        data[1] = color.green;
        data[0] = color.blue;
    }
    else if(cf == LV_COLOR_FORMAT_XRGB8888) {
        data[2] = color.red;
        data[1] = color.green;
        data[0] = color.blue;
        data[3] = 0xFF;
    }
    else if(cf == LV_COLOR_FORMAT_ARGB8888) {
        lv_color32_t * buf = (lv_color32_t *)data;
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

    lv_image_dsc_t dsc;
    lv_draw_buf_to_image(canvas->draw_buf, &dsc);

    lv_image_buf_set_palette(&dsc, id, c);
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_draw_buf_t * lv_canvas_get_draw_buf(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    return canvas->draw_buf;
}

lv_color32_t lv_canvas_get_px(lv_obj_t * obj, int32_t x, int32_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_color32_t ret = { 0 };
    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    if(canvas->draw_buf == NULL) return ret;

    lv_image_header_t * header = &canvas->draw_buf->header;
    const uint8_t * px = lv_draw_buf_goto_xy(canvas->draw_buf, x, y);

    switch(header->cf) {
        case LV_COLOR_FORMAT_ARGB8888:
            ret = *(lv_color32_t *)px;
            break;
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_XRGB8888:
            ret.red = px[2];
            ret.green = px[1];
            ret.blue = px[0];
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
                lv_color_t alpha_color = lv_obj_get_style_image_recolor(obj, LV_PART_MAIN);
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

lv_image_dsc_t * lv_canvas_get_image(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    return (lv_image_dsc_t *)canvas->draw_buf;
}

const void * lv_canvas_get_buf(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    if(canvas->draw_buf)
        return canvas->draw_buf->unaligned_data;

    return NULL;
}

/*=====================
 * Other functions
 *====================*/

void lv_canvas_copy_buf(lv_obj_t * obj, const lv_area_t * canvas_area, lv_draw_buf_t * dest_buf,
                        const lv_area_t * dest_area)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(canvas_area && dest_buf);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    if(canvas->draw_buf == NULL) return;

    LV_ASSERT_MSG(canvas->draw_buf->header.cf != dest_buf->header.cf, "Color formats must be the same");

    lv_draw_buf_copy(canvas->draw_buf, canvas_area, dest_buf, dest_area);
}

void lv_canvas_fill_bg(lv_obj_t * obj, lv_color_t color, lv_opa_t opa)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    lv_draw_buf_t * draw_buf = canvas->draw_buf;
    if(draw_buf == NULL) return;

    lv_image_header_t * header = &draw_buf->header;
    uint32_t x;
    uint32_t y;

    uint32_t stride = header->stride;
    uint8_t * data = draw_buf->data;
    if(header->cf == LV_COLOR_FORMAT_RGB565) {
        uint16_t c16 = lv_color_to_u16(color);
        for(y = 0; y < header->h; y++) {
            uint16_t * buf16 = (uint16_t *)(data + y * stride);
            for(x = 0; x < header->w; x++) {
                buf16[x] = c16;
            }
        }
    }
    else if(header->cf == LV_COLOR_FORMAT_XRGB8888 || header->cf == LV_COLOR_FORMAT_ARGB8888) {
        uint32_t c32 = lv_color_to_u32(color);
        if(header->cf == LV_COLOR_FORMAT_ARGB8888) {
            c32 &= 0x00ffffff;
            c32 |= (uint32_t)opa << 24;
        }
        for(y = 0; y < header->h; y++) {
            uint32_t * buf32 = (uint32_t *)(data + y * stride);
            for(x = 0; x < header->w; x++) {
                buf32[x] = c32;
            }
        }
    }
    else if(header->cf == LV_COLOR_FORMAT_RGB888) {
        for(y = 0; y < header->h; y++) {
            uint8_t * buf8 = (uint8_t *)(data + y * stride);
            for(x = 0; x < header->w * 3; x += 3) {
                buf8[x + 0] = color.blue;
                buf8[x + 1] = color.green;
                buf8[x + 2] = color.red;
            }
        }
    }
    else {
        for(y = 0; y < header->h; y++) {
            for(x = 0; x < header->w; x++) {
                lv_canvas_set_px(obj, x, y, color, opa);
            }
        }
    }

    lv_obj_invalidate(obj);
}

void lv_canvas_init_layer(lv_obj_t * obj, lv_layer_t * layer)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(layer);
    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    if(canvas->draw_buf == NULL) return;

    lv_image_header_t * header = &canvas->draw_buf->header;
    lv_area_t canvas_area = {0, 0, header->w - 1,  header->h - 1};
    lv_memzero(layer, sizeof(*layer));

    layer->draw_buf = canvas->draw_buf;
    layer->color_format = header->cf;
    layer->buf_area = canvas_area;
    layer->_clip_area = canvas_area;
}

void lv_canvas_finish_layer(lv_obj_t * canvas, lv_layer_t * layer)
{
    while(layer->draw_task_head) {
        lv_draw_dispatch_wait_for_request();
        lv_draw_dispatch_layer(lv_obj_get_display(canvas), layer);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_canvas_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_UNUSED(obj);
}

static void lv_canvas_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    if(canvas->draw_buf == NULL) return;

    lv_image_cache_drop(&canvas->draw_buf);
}

#endif
