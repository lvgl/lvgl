/**
 * @file lv_barcode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_class_private.h"
#include "../../misc/lv_area_private.h"
#include "../../draw/lv_draw_image_private.h"
#include "../../draw/lv_draw_private.h"
#include "../../lvgl_public.h"
#include "lv_barcode_private.h"

#if LV_USE_BARCODE

#include "../../libs/barcode/code128.h"
#include "../../misc/cache/lv_cache.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_barcode_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_barcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_barcode_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);
static void get_bar_area(lv_obj_t * obj, lv_area_t * area);
static bool barcode_store_data(lv_barcode_t * barcode, const char * data);
static uint8_t * barcode_encode_data(const char * data, lv_barcode_encoding_t encoding, int32_t * bar_count);
static void barcode_update(lv_obj_t * obj);
static void barcode_free_buf(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_barcode_class = {
    .constructor_cb = lv_barcode_constructor,
    .destructor_cb = lv_barcode_destructor,
    .event_cb = lv_barcode_event,
    /*As long as the bars need, and thick enough to be scannable out of the box*/
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_DPI_DEF / 2,
    .instance_size = sizeof(lv_barcode_t),
    .base_class = &lv_canvas_class,
    .name = "lv_barcode",
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

void lv_barcode_set_data(lv_obj_t * obj, const char * data)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG_MSG(data != NULL, return, "data must not be NULL");

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Empty data encodes to the guard bars alone, which is not a barcode anyone wants.
     *Forget what was there and show nothing.*/
    if(data[0] == '\0') {
        LV_LOG_WARN("data is empty, clearing the barcode");
        lv_free(barcode->data);
        barcode->data = NULL;
    }
    /*Keep a copy of the data so a later property change can regenerate the bars*/
    else if(!barcode_store_data(barcode, data)) {
        LV_LOG_WARN("couldn't allocate the data");
        return;
    }

    barcode_update(obj);
}

void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG_MSG(scale > 0, return, "scale must be at least 1");

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->scale == scale) return;
    barcode->scale = scale;

    barcode_update(obj);
}

void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG_MSG(direction == LV_DIR_HOR || direction == LV_DIR_VER, return,
                     "direction must be LV_DIR_HOR or LV_DIR_VER");

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->direction == direction) return;
    barcode->direction = direction;

    barcode_update(obj);
}

void lv_barcode_set_encoding(lv_obj_t * obj, lv_barcode_encoding_t encoding)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->encoding == encoding) return;
    barcode->encoding = encoding;

    barcode_update(obj);
}

void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Nothing to do if the color is unchanged: skip the palette write and cache drop*/
    if(lv_color_eq(barcode->dark_color, color)) return;
    barcode->dark_color = color;

    /*Only the palette changes, the bars stay as they are*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 1, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Nothing to do if the color is unchanged: skip the palette write and cache drop*/
    if(lv_color_eq(barcode->light_color, color)) return;
    barcode->light_color = color;

    /*Only the palette changes, the bars stay as they are*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 0, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

const char * lv_barcode_get_data(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return NULL);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->data;
}

uint16_t lv_barcode_get_scale(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->scale;
}

lv_dir_t lv_barcode_get_direction(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_DIR_HOR);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->direction;
}

lv_barcode_encoding_t lv_barcode_get_encoding(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_BARCODE_ENCODING_CODE128_GS1);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->encoding;
}

lv_color_t lv_barcode_get_dark_color(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return (lv_color_t) {
        0
    });

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->dark_color;
}

lv_color_t lv_barcode_get_light_color(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return (lv_color_t) {
        0
    });

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->light_color;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_barcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_ASSERT(obj != NULL);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->dark_color = lv_color_black();
    barcode->light_color = lv_color_white();
    barcode->data = NULL;
    barcode->scale = 1;
    barcode->direction = LV_DIR_HOR;
    barcode->encoding = LV_BARCODE_ENCODING_CODE128_GS1;
}

static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_ASSERT(obj != NULL);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    lv_free(barcode->data);
    barcode->data = NULL;

    /*Don't go through barcode_free_buf(): it would update the image of a Widget being deleted*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_image_cache_drop(draw_buf);

    /*@fixme destroy buffer in cache free_cb.*/
    lv_draw_buf_destroy(draw_buf);
}

static void lv_barcode_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    LV_ASSERT(e != NULL);

    lv_event_code_t code = lv_event_get_code(e);

    /*The bars are drawn here, centered in the content area, instead of by lv_image which
     *would tile them over the whole Widget. Skip lv_image and lv_canvas, and call only
     *lv_obj's handler to draw the background, outline, etc.*/
    if(code == LV_EVENT_DRAW_MAIN || code == LV_EVENT_DRAW_POST || code == LV_EVENT_COVER_CHECK) {
        lv_result_t res = lv_obj_event_base(&lv_image_class, e);
        if(res != LV_RESULT_OK) return;

        if(code == LV_EVENT_DRAW_MAIN) draw_main(e);
        return;
    }

    /*Call the ancestor's event handler*/
    lv_obj_event_base(MY_CLASS, e);
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;

    lv_area_t bar_area;
    get_bar_area(obj, &bar_area);

    /*Clipped bars can't be scanned, so draw nothing if they don't fit*/
    lv_area_t content_area;
    lv_obj_get_content_coords(obj, &content_area);
    if(!lv_area_is_in(&bar_area, &content_area, 0)) {
        LV_LOG_WARN("the bars don't fit into the content area");
        return;
    }

    lv_layer_t * layer = lv_event_get_layer(e);
    lv_area_t clip_area_ori = layer->_clip_area;
    if(!lv_area_intersect(&layer->_clip_area, &clip_area_ori, &bar_area)) {
        /*lv_area_intersect() writes the result even if there is no intersection*/
        layer->_clip_area = clip_area_ori;
        return;
    }

    lv_draw_image_dsc_t draw_dsc;
    lv_draw_image_dsc_init(&draw_dsc);
    draw_dsc.base.layer = layer;
    lv_obj_init_draw_image_dsc(obj, LV_PART_MAIN, &draw_dsc);
    draw_dsc.src = draw_buf;
    draw_dsc.clip_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);

    /*The buffer is a strip one pixel thick; tile it across the thickness*/
    lv_area_set(&draw_dsc.image_area, bar_area.x1, bar_area.y1,
                bar_area.x1 + draw_buf->header.w - 1, bar_area.y1 + draw_buf->header.h - 1);
    draw_dsc.tile = 1;

    lv_draw_image(layer, &draw_dsc, &layer->_clip_area);
    layer->_clip_area = clip_area_ori;
}

/**
 * Get the area of the bars: centered along the bars in the content area and as thick
 * as the content area.
 */
static void get_bar_area(lv_obj_t * obj, lv_area_t * area)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);

    lv_obj_get_content_coords(obj, area);
    if(barcode->direction == LV_DIR_HOR) {
        const int32_t len = draw_buf->header.w;
        area->x1 += (lv_area_get_width(area) - len) / 2;
        area->x2 = area->x1 + len - 1;
    }
    else {
        const int32_t len = draw_buf->header.h;
        area->y1 += (lv_area_get_height(area) - len) / 2;
        area->y2 = area->y1 + len - 1;
    }
}

static bool barcode_store_data(lv_barcode_t * barcode, const char * data)
{
    LV_ASSERT(barcode != NULL);
    LV_ASSERT(data != NULL);

    const size_t len = lv_strlen(data);

    /*Assign only on success, so a failed realloc leaves the previous data owned by
     *`barcode`. lv_realloc(NULL, n) allocates, so the first call needs no special case.*/
    char * new_data = lv_realloc(barcode->data, len + 1);
    LV_ASSERT_MALLOC(new_data);
    if(new_data == NULL) return false;

    lv_memcpy(new_data, data, len);
    new_data[len] = '\0';

    barcode->data = new_data;
    return true;
}

/**
 * Turn the data into one byte per bar: 0 for a light bar, non-zero for a dark one.
 * @return the bars, to be freed by the caller, or NULL if the data cannot be encoded
 */
static uint8_t * barcode_encode_data(const char * data, lv_barcode_encoding_t encoding, int32_t * bar_count)
{
    LV_ASSERT(data != NULL);
    LV_ASSERT(bar_count != NULL);

    size_t len = code128_estimate_len(data);
    LV_LOG_INFO("data: %s, len = %zu", data, len);

    uint8_t * pattern = lv_malloc(len);
    LV_ASSERT_MALLOC(pattern);
    if(pattern == NULL) {
        LV_LOG_ERROR("malloc failed for the bar pattern");
        return NULL;
    }

    int32_t w = 0;
    switch(encoding) {
        case LV_BARCODE_ENCODING_CODE128_GS1:
            w = (int32_t)code128_encode_gs1(data, (char *)pattern, len);
            break;
        case LV_BARCODE_ENCODING_CODE128_RAW:
            w = (int32_t)code128_encode_raw(data, (char *)pattern, len);
            break;
        default:
            LV_ASSERT(false);
            break;
    }
    LV_LOG_INFO("barcode width = %" LV_PRId32, w);

    if(w <= 0) {
        LV_LOG_WARN("the data could not be encoded");
        lv_free(pattern);
        return NULL;
    }

    *bar_count = w;
    return pattern;
}

/**
 * Encode the data and draw the bars into a new canvas buffer which is a strip one pixel
 * thick and `bar_count * scale` long. Without data, or if the encoding fails, the
 * buffer is freed, so the Widget's content size becomes zero.
 */
static void barcode_update(lv_obj_t * obj)
{
    LV_ASSERT(obj != NULL);
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->data == NULL) {
        barcode_free_buf(obj);
        return;
    }

    int32_t bar_count = 0;
    uint8_t * pattern = barcode_encode_data(barcode->data, barcode->encoding, &bar_count);
    if(pattern == NULL) {
        barcode_free_buf(obj);
        return;
    }

    if(bar_count > LV_COORD_MAX / barcode->scale) {
        LV_LOG_WARN("%" LV_PRId32 " bars with %d px scale are too long", bar_count, (int)barcode->scale);
        lv_free(pattern);
        barcode_free_buf(obj);
        return;
    }

    const bool hor = (barcode->direction == LV_DIR_HOR);
    const int32_t len = bar_count * barcode->scale;
    const int32_t w = hor ? len : 1;
    const int32_t h = hor ? 1 : len;

    lv_draw_buf_t * new_buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_I1, LV_STRIDE_AUTO);
    if(new_buf == NULL) {
        LV_LOG_ERROR("couldn't allocate the canvas buffer");
        lv_free(pattern);
        barcode_free_buf(obj);
        return;
    }

    lv_draw_buf_set_palette(new_buf, 0, lv_color_to_32(barcode->light_color, LV_OPA_COVER));
    lv_draw_buf_set_palette(new_buf, 1, lv_color_to_32(barcode->dark_color, LV_OPA_COVER));

    /*Clear to palette index 0, so only the dark bars need to be set*/
    lv_draw_buf_clear(new_buf, NULL);

    uint8_t * buf = lv_draw_buf_goto_xy(new_buf, 0, 0);
    const uint32_t stride = new_buf->header.stride;
    for(int32_t bar = 0; bar < bar_count; bar++) {
        if(pattern[bar] == 0) continue;

        for(int32_t i = 0; i < barcode->scale; i++) {
            const int32_t px = bar * barcode->scale + i;
            if(hor) buf[px >> 3] |= 0x80 >> (px & 0x7);
            else buf[px * stride] |= 0x80;
        }
    }

    lv_free(pattern);

    /*Setting the new source also updates the self size, so LV_SIZE_CONTENT follows the bars*/
    lv_draw_buf_t * old_buf = lv_canvas_get_draw_buf(obj);
    lv_canvas_set_draw_buf(obj, new_buf);
    if(old_buf) lv_draw_buf_destroy(old_buf);
}

/**
 * Free the canvas buffer and remove it from the image, so the Widget's content size
 * becomes zero
 */
static void barcode_free_buf(lv_obj_t * obj)
{
    lv_canvas_t * canvas = (lv_canvas_t *)obj;
    lv_draw_buf_t * draw_buf = canvas->draw_buf;
    if(draw_buf == NULL) return;

    lv_image_cache_drop(draw_buf);
    lv_image_set_src(obj, NULL);
    canvas->draw_buf = NULL;

    /*@fixme destroy buffer in cache free_cb.*/
    lv_draw_buf_destroy(draw_buf);
}

#endif /*LV_USE_BARCODE*/
