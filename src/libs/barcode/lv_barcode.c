/**
 * @file lv_barcode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_barcode.h"
#if LV_USE_BARCODE

#include "code128.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_barcode_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_barcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static bool lv_barcode_change_buf_size(lv_obj_t * obj, lv_coord_t w);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_barcode_class = {
    .constructor_cb = lv_barcode_constructor,
    .destructor_cb = lv_barcode_destructor,
    .width_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_barcode_t),
    .base_class = &lv_canvas_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_barcode_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->dark_color = color;
}

void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->light_color = color;
}

void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(scale == 0) {
        scale = 1;
    }

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->scale = scale;
}

lv_res_t lv_barcode_update(lv_obj_t * obj, const char * data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(data);

    lv_res_t res = LV_RES_INV;
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(data == NULL || lv_strlen(data) == 0) {
        LV_LOG_WARN("data is empty");
        return LV_RES_INV;
    }

    size_t len = code128_estimate_len(data);
    LV_LOG_INFO("data: %s, len = %zu", data, len);

    char * out_buf = lv_malloc(len);
    LV_ASSERT_MALLOC(out_buf);
    if(!out_buf) {
        LV_LOG_ERROR("malloc failed for out_buf");
        return LV_RES_INV;
    }

    lv_coord_t barcode_w = code128_encode_gs1(data, out_buf, len);
    LV_LOG_INFO("barcode width = %d", (int)barcode_w);

    LV_ASSERT(barcode->scale > 0);
    uint16_t scale = barcode->scale;

    if(!lv_barcode_change_buf_size(obj, barcode_w * scale)) {
        goto failed;
    }

    lv_canvas_set_palette(obj, 0, barcode->dark_color);
    lv_canvas_set_palette(obj, 1, barcode->light_color);

    for(lv_coord_t x = 0; x < barcode_w; x++) {
        lv_color_t color;
        color.full = out_buf[x] ? 0 : 1;
        for(uint16_t i = 0; i < scale; i++) {
            lv_canvas_set_px_color(obj, x * scale + i, 0, color);
        }
    }

    res = LV_RES_OK;

failed:
    lv_free(out_buf);
    return res;
}

lv_color_t lv_barcode_get_dark_color(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->dark_color;
}

lv_color_t lv_barcode_get_light_color(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->light_color;
}

uint16_t lv_barcode_get_scale(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->scale;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_barcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->dark_color = lv_color_black();
    barcode->light_color = lv_color_white();
    barcode->scale = 1;
}

static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_img_dsc_t * img = lv_canvas_get_img(obj);
    lv_img_cache_invalidate_src(img);

    if(!img->data) {
        LV_LOG_INFO("canvas buffer is NULL");
        return;
    }

    LV_LOG_INFO("free canvas buffer: %p", img->data);

    lv_free((void *)img->data);
    img->data = NULL;
}

static bool lv_barcode_change_buf_size(lv_obj_t * obj, lv_coord_t w)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT(w > 0);

    lv_img_dsc_t * img = lv_canvas_get_img(obj);
    void * buf = (void *)img->data;

    uint32_t buf_size = LV_CANVAS_BUF_SIZE_INDEXED_1BIT(w, 1);
    buf = lv_realloc(buf, buf_size);
    LV_ASSERT_MALLOC(buf);

    if(!buf) {
        LV_LOG_ERROR("malloc failed for canvas budffer");
        return false;
    }

    lv_canvas_set_buffer(obj, buf, w, 1, LV_IMG_CF_INDEXED_1BIT);
    LV_LOG_INFO("set canvas buffer: %p, width = %d", buf, (int)w);
    return true;
}

#endif /*LV_USE_BARCODE*/
