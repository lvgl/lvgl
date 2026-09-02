/**
 * @file lv_barcode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_class_private.h"
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
static bool lv_barcode_change_buf_size(lv_obj_t * obj, int32_t w, int32_t h);
static void lv_barcode_clear(lv_obj_t * obj);
static bool barcode_store_data(lv_barcode_t * barcode, const char * data);
static void barcode_forget_data(lv_barcode_t * barcode);
static void barcode_drop_encoded(lv_barcode_t * barcode);
static uint8_t * barcode_encode(const lv_barcode_t * barcode, int32_t * bar_count);
static bool barcode_fit(lv_obj_t * obj);
static bool barcode_fit_needed(lv_obj_t * obj);
static bool barcode_fit_only(lv_obj_t * obj);
static lv_result_t barcode_fill(lv_obj_t * obj);
static lv_result_t barcode_render(lv_obj_t * obj);
static lv_result_t barcode_mark_dirty(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_barcode_class = {
    .constructor_cb = lv_barcode_constructor,
    .destructor_cb = lv_barcode_destructor,
    .event_cb = lv_barcode_event,
    .width_def = LV_SIZE_CONTENT,
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

void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(lv_color_eq(barcode->dark_color, color)) return;
    barcode->dark_color = color;

    /*Write the palette now so the color also applies to bars that are already drawn*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 1, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(lv_color_eq(barcode->light_color, color)) return;
    barcode->light_color = color;

    /*Write the palette now so the color also applies to bars that are already drawn*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 0, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG(scale > 0, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->scale == scale) return;
    barcode->scale = scale;
    barcode_mark_dirty(obj);
}

void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->direction == direction) return;
    barcode->direction = direction;
    barcode_mark_dirty(obj);
}

void lv_barcode_set_tiled(lv_obj_t * obj, bool tiled)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if((bool)barcode->tiled == tiled) return;
    barcode->tiled = tiled;
    lv_image_set_inner_align(obj, tiled ? LV_IMAGE_ALIGN_TILE : LV_IMAGE_ALIGN_DEFAULT);
    barcode_mark_dirty(obj);
}

void lv_barcode_set_encoding(lv_obj_t * obj, lv_barcode_encoding_t encoding)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->encoding == encoding) return;
    barcode->encoding = encoding;
    barcode_drop_encoded(barcode);
    barcode_mark_dirty(obj);
}

lv_result_t lv_barcode_set_text(lv_obj_t * obj, const char * text)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    LV_CHECK_ARG(text != NULL, return LV_RESULT_INVALID);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(text[0] == '\0') {
        LV_LOG_WARN("text is empty");
        barcode_forget_data(barcode);
        barcode->needs_update = false;
        barcode->render_failed = true;
        lv_barcode_clear(obj);
        return LV_RESULT_INVALID;
    }

    if(!barcode_store_data(barcode, text)) return LV_RESULT_INVALID;

    return barcode_mark_dirty(obj);
}

const char * lv_barcode_get_text(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return NULL);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->data;
}

lv_result_t lv_barcode_render(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);

    lv_result_t res = barcode_render(obj);

    if(res != LV_RESULT_OK) lv_barcode_clear(obj);
    else lv_obj_invalidate(obj);

    return res;
}

void lv_barcode_set_update_mode(lv_obj_t * obj, lv_barcode_update_mode_t mode)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(mode == LV_BARCODE_UPDATE_MODE_IMMEDIATE && barcode->needs_update && barcode->data != NULL) {
        if(lv_barcode_render(obj) != LV_RESULT_OK) {
            LV_LOG_ERROR("regenerating on the switch to immediate update mode failed; "
                         "call lv_barcode_render() before switching the mode to get the result");
        }
        else {
            LV_LOG_WARN("switching to immediate update mode while the bitmap was out of date; "
                        "call lv_barcode_render() before switching the mode to get the result");
        }
    }

    barcode->update_mode = mode;
}

lv_barcode_update_mode_t lv_barcode_get_update_mode(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_BARCODE_UPDATE_MODE_IMMEDIATE);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return (lv_barcode_update_mode_t)barcode->update_mode;
}

bool lv_barcode_get_render_failed(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return true);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->render_failed;
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

uint16_t lv_barcode_get_scale(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->scale;
}

lv_barcode_encoding_t lv_barcode_get_encoding(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    const lv_barcode_t * barcode = (const lv_barcode_t *)obj;
    return barcode->encoding;
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
    barcode->pattern = NULL;
    barcode->bar_count = 0;
    barcode->scale = 1;
    barcode->direction = LV_DIR_HOR;
    barcode->encoding = LV_BARCODE_ENCODING_CODE128_GS1;
    barcode->tiled = false;
    barcode->update_mode = LV_BARCODE_UPDATE_MODE_IMMEDIATE;
    barcode->needs_update = false;
    barcode->render_failed = true;   /*Nothing generated yet*/
    barcode->fitting = false;
    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_DEFAULT);
}

static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_ASSERT(obj != NULL);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode_forget_data(barcode);

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_image_cache_drop(draw_buf);

    /*@fixme destroy buffer in cache free_cb.*/
    lv_draw_buf_destroy(draw_buf);
}

static void lv_barcode_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_event_code_t code = lv_event_get_code(e);

    /*Call the ancestor's event handler*/
    lv_result_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(code == LV_EVENT_SIZE_CHANGED) {
        /*Refitting reallocates the canvas, which is itself a content size change and comes
         *back here: `fitting` catches that echo within the call, barcode_fit_needed() the
         *one a layout pass later. An out of date bitmap is always refitted, so a size that
         *could not be fitted before gets another go.*/
        if(!barcode->fitting && (barcode->needs_update || barcode_fit_needed(obj))) {
            barcode_mark_dirty(obj);
        }
    }
    else if(code == LV_EVENT_DRAW_MAIN_BEGIN) {
        /*A failed state is not retried here; only a change clears `render_failed`*/
        if(barcode->needs_update && !barcode->render_failed) {
            LV_ASSERT(barcode->update_mode == LV_BARCODE_UPDATE_MODE_DEFERRED);

            /*Safe: the setter already resized the canvas, so nothing is reallocated here*/
            LV_LOG_WARN("filling in the barcode during the redraw because lv_barcode_render() "
                        "was not called after the property changes; this adds the work to the "
                        "refresh and its result cannot be reported");

            if(barcode_fill(obj) != LV_RESULT_OK) {
                barcode->render_failed = true;
                LV_LOG_ERROR("the barcode could not be filled in during the redraw "
                             "(scale %d, %s); the bitmap is left blank",
                             (int)barcode->scale, barcode->direction == LV_DIR_VER ? "vertical" : "horizontal");
            }
        }
    }
}

static bool lv_barcode_change_buf_size(lv_obj_t * obj, int32_t w, int32_t h)
{
    LV_ASSERT_NULL(obj);
    if(w <= 0 || h <= 0) {
        LV_LOG_WARN("invalid size: %" LV_PRId32 " x %" LV_PRId32, w, h);
        return false;
    }

    lv_draw_buf_t * old_buf = lv_canvas_get_draw_buf(obj);

    /*Reuse the buffer at an unchanged geometry, to avoid the invalidation and relayout a
     *reallocation triggers*/
    if(old_buf != NULL &&
       (int32_t)old_buf->header.w == w &&
       (int32_t)old_buf->header.h == h &&
       old_buf->header.cf == LV_COLOR_FORMAT_I1) {
        return true;
    }

    lv_draw_buf_t * new_buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_I1, LV_STRIDE_AUTO);
    if(new_buf == NULL) {
        LV_LOG_ERROR("malloc failed for canvas buffer");
        return false;
    }

    lv_canvas_set_draw_buf(obj, new_buf);
    LV_LOG_INFO("set canvas buffer: %p, width = %" LV_PRId32, (void *)new_buf, w);

    if(old_buf != NULL) lv_draw_buf_destroy(old_buf);
    return true;
}

static void lv_barcode_clear(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(!draw_buf) {
        return;
    }

    lv_draw_buf_clear(draw_buf, NULL);
    lv_image_cache_drop(draw_buf);
    lv_obj_invalidate(obj);
}

static bool barcode_store_data(lv_barcode_t * barcode, const char * data)
{
    const size_t len = lv_strlen(data);

    /*Assign only on success, so a failed realloc leaves the previous data owned by
     *`barcode`. lv_realloc(NULL, n) allocates, so the first call needs no special case.*/
    char * new_data = lv_realloc(barcode->data, len + 1);
    LV_ASSERT_MALLOC(new_data);
    if(new_data == NULL) return false;

    lv_memcpy(new_data, data, len);
    new_data[len] = '\0';

    barcode->data = new_data;

    barcode_drop_encoded(barcode);
    return true;
}

static void barcode_forget_data(lv_barcode_t * barcode)
{
    lv_free(barcode->data);
    barcode->data = NULL;
    barcode_drop_encoded(barcode);
}

static void barcode_drop_encoded(lv_barcode_t * barcode)
{
    lv_free(barcode->pattern);
    barcode->pattern = NULL;
    barcode->bar_count = 0;
}

static uint8_t * barcode_encode(const lv_barcode_t * barcode, int32_t * bar_count)
{
    const char * data = barcode->data;
    LV_ASSERT_NULL(data);

    size_t len = code128_estimate_len(data);
    LV_LOG_INFO("data: %s, len = %zu", data, len);

    uint8_t * pattern = lv_malloc(len);
    LV_ASSERT_MALLOC(pattern);
    if(pattern == NULL) {
        LV_LOG_ERROR("malloc failed for the bar pattern");
        return NULL;
    }

    int32_t w = 0;
    switch(barcode->encoding) {
        case LV_BARCODE_ENCODING_CODE128_GS1:
            w = (int32_t) code128_encode_gs1(data, (char *)pattern, len);
            break;
        case LV_BARCODE_ENCODING_CODE128_RAW:
            w = (int32_t) code128_encode_raw(data, (char *)pattern, len);
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

static bool barcode_fit(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->bar_count == 0) {
        barcode->pattern = barcode_encode(barcode, &barcode->bar_count);
        if(barcode->pattern == NULL) return false;
    }

    const int32_t bar_count = barcode->bar_count;
    const int32_t scale = barcode->scale;

    if(bar_count > INT32_MAX / scale) {
        LV_LOG_WARN("%" LV_PRId32 " bars at scale %" LV_PRId32 " do not fit an int32_t",
                    bar_count, scale);
        return false;
    }
    const int32_t bars_px = bar_count * scale;

    int32_t buf_w;
    int32_t buf_h;

    if(barcode->tiled) {
        buf_w = (barcode->direction == LV_DIR_HOR) ? bars_px : 1;
        buf_h = (barcode->direction == LV_DIR_VER) ? bars_px : 1;
    }
    else {
        lv_obj_update_layout(obj);
        buf_w = (barcode->direction == LV_DIR_HOR) ? bars_px : lv_obj_get_width(obj);
        buf_h = (barcode->direction == LV_DIR_VER) ? bars_px : lv_obj_get_height(obj);
    }

    /*Reallocating changes the content size, which comes straight back as SIZE_CHANGED*/
    barcode->fitting = true;
    bool ok = lv_barcode_change_buf_size(obj, buf_w, buf_h);
    barcode->fitting = false;

    return ok;
}

static bool barcode_fit_needed(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->data == NULL) return false;

    if(barcode->tiled) return false;

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return true;

    const bool hor = (barcode->direction == LV_DIR_HOR);
    const int32_t have = hor ? (int32_t)draw_buf->header.h : (int32_t)draw_buf->header.w;
    const int32_t want = hor ? lv_obj_get_height(obj) : lv_obj_get_width(obj);
    return have != want;
}

static bool barcode_fit_only(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    barcode->render_failed = true;
    barcode->needs_update = true;

    if(barcode->data == NULL) return false;
    if(!barcode_fit(obj)) return false;

    barcode->render_failed = false;
    return true;
}

static lv_result_t barcode_fill(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->data == NULL) return LV_RESULT_INVALID;

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return LV_RESULT_INVALID;

    uint8_t * pattern = barcode->pattern;
    barcode->pattern = NULL;
    if(pattern == NULL) {
        pattern = barcode_encode(barcode, &barcode->bar_count);
        if(pattern == NULL) return LV_RESULT_INVALID;
    }

    const int32_t bar_count = barcode->bar_count;
    const int32_t scale = barcode->scale;
    const int32_t buf_w = (int32_t)draw_buf->header.w;
    const int32_t buf_h = (int32_t)draw_buf->header.h;

    const int32_t avail = (barcode->direction == LV_DIR_HOR) ? buf_w : buf_h;
    if(bar_count > INT32_MAX / scale || bar_count * scale > avail) {
        LV_LOG_WARN("the bars (%" LV_PRId32 " x %" LV_PRId32 " px) do not fit the %"
                    LV_PRId32 " px canvas", bar_count, scale, avail);
        lv_free(pattern);
        return LV_RESULT_INVALID;
    }

    /*Temporarily disable invalidation to improve the efficiency of lv_canvas_set_px*/
    lv_display_enable_invalidation(lv_obj_get_display(obj), false);

    uint32_t stride = draw_buf->header.stride;
    const lv_color_t color = lv_color_hex(1);

    lv_draw_buf_clear(draw_buf, NULL);

    /*Set the palette on the draw buffer, not via lv_canvas_set_palette(), to avoid an
     *invalidation here - the caller or the draw pass refreshes the object*/
    lv_draw_buf_set_palette(draw_buf, 0, lv_color_to_32(barcode->light_color, LV_OPA_COVER));
    lv_draw_buf_set_palette(draw_buf, 1, lv_color_to_32(barcode->dark_color, LV_OPA_COVER));

    for(int32_t x = 0; x < bar_count; x++) {
        /*skip empty data*/
        if(pattern[x] == 0) {
            continue;
        }

        for(int32_t i = 0; i < scale; i++) {
            int32_t offset = x * scale + i;
            if(barcode->direction == LV_DIR_HOR) {
                lv_canvas_set_px(obj, offset, 0, color, LV_OPA_COVER);
            }
            else { /*LV_DIR_VER*/
                if(barcode->tiled) {
                    lv_canvas_set_px(obj, 0, offset, color, LV_OPA_COVER);
                }
                else {
                    uint8_t * dest = lv_draw_buf_goto_xy(draw_buf, 0, offset);
                    lv_memset(dest, 0xFF, stride);
                }
            }
        }
    }

    if(!barcode->tiled && barcode->direction == LV_DIR_HOR && buf_h > 1) {
        int32_t h = buf_h - 1;
        const uint8_t * src = lv_draw_buf_goto_xy(draw_buf, 0, 0);
        uint8_t * dest = lv_draw_buf_goto_xy(draw_buf, 0, 1);
        while(h--) {
            lv_memcpy(dest, src, stride);
            dest += stride;
        }
    }

    lv_display_enable_invalidation(lv_obj_get_display(obj), true);
    lv_image_cache_drop(draw_buf);

    lv_free(pattern);

    barcode->needs_update = false;
    barcode->render_failed = false;
    return LV_RESULT_OK;
}

static lv_result_t barcode_render(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Assume failure so no early return can forget to record it; only barcode_fill()'s
     *success path clears these. `needs_update` survives a failure on purpose - the bitmap
     *still does not match the properties - and `render_failed` keeps the draw hook from
     *retrying a known-bad state every frame. Nothing is logged here: the result is
     *returned, and the caller reports it if nothing else will.*/
    barcode->render_failed = true;
    barcode->needs_update = true;

    if(barcode->data == NULL) return LV_RESULT_INVALID;
    if(!barcode_fit(obj)) return LV_RESULT_INVALID;

    return barcode_fill(obj);
}

static lv_result_t barcode_mark_dirty(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->data == NULL) return LV_RESULT_INVALID;

    barcode->render_failed = false;

    /*The canvas is resized in both modes, because the draw pass cannot do it*/
    const bool ok = (barcode->update_mode == LV_BARCODE_UPDATE_MODE_IMMEDIATE)
                    ? (barcode_render(obj) == LV_RESULT_OK)
                    : barcode_fit_only(obj);

    if(ok) lv_obj_invalidate(obj);
    else lv_barcode_clear(obj);

    return ok ? LV_RESULT_OK : LV_RESULT_INVALID;
}

#endif /*LV_USE_BARCODE*/
