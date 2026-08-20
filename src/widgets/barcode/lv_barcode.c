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
static char * barcode_encode(const lv_barcode_t * barcode, int32_t * barcode_w);
static bool barcode_resize(lv_obj_t * obj);
static lv_result_t barcode_draw(lv_obj_t * obj);
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
    barcode->dark_color = color;

    /*Apply the color right away so it takes effect even if it is set after the
     *bitmap was already rendered. This only rewrites the palette, no re-encode.*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 1, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->light_color = color;

    /*Apply the color right away so it takes effect even if it is set after the
     *bitmap was already rendered. This only rewrites the palette, no re-encode.*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 0, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG(scale > 0 && scale < 0x0FFF, return) /*0 is invalid; the scale is stored in a 12-bit field */

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->scale = scale;
    barcode_mark_dirty(obj);
}

void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->direction = direction;
    barcode_mark_dirty(obj);
}

void lv_barcode_set_tiled(lv_obj_t * obj, bool tiled)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->tiled = tiled;
    lv_image_set_inner_align(obj, tiled ? LV_IMAGE_ALIGN_TILE : LV_IMAGE_ALIGN_DEFAULT);
    barcode_mark_dirty(obj);
}

void lv_barcode_set_encoding(lv_obj_t * obj, lv_barcode_encoding_t encoding)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->encoding = encoding;
    barcode_mark_dirty(obj);
}

lv_result_t lv_barcode_set_data(lv_obj_t * obj, const char * data)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    LV_CHECK_ARG(data != NULL, return LV_RESULT_INVALID, "data must not be NULL");

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(data[0] == '\0') {
        /*Empty data: nothing to encode. Clear any previous barcode and forget the data
         *so get_data()/update() stay consistent (no stale "" reported as valid).*/
        if(barcode->data) {
            lv_free(barcode->data);
            barcode->data = NULL;
        }
        barcode->needs_draw = false;
        lv_barcode_clear(obj);
        return LV_RESULT_INVALID;
    }

    /*Store a copy of the data so the bitmap can be regenerated later
     *(e.g. when the scale, direction or encoding changes after the data was set)*/
    if(!barcode_store_data(barcode, data)) return LV_RESULT_INVALID;

    return barcode_mark_dirty(obj);
}

lv_result_t lv_barcode_update(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*The bitmap is already up to date: keep two consecutive calls no more
     *expensive than one. Report validity from whether any data is stored.*/
    if(!barcode->needs_draw) {
        return barcode->data != NULL ? LV_RESULT_OK : LV_RESULT_INVALID;
    }

    /*The canvas was already (re)sized when the property changed; just fill it*/
    lv_result_t res = barcode_draw(obj);
    lv_obj_invalidate(obj);
    return res;
}

lv_result_t lv_barcode_force_update(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    if(!barcode_resize(obj)) return LV_RESULT_INVALID;
    lv_result_t res = barcode_draw(obj);
    lv_obj_invalidate(obj);
    return res;
}

lv_result_t lv_barcode_set_auto_update(lv_obj_t * obj, bool enable)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->auto_update = enable;

    /*If auto update is turned back on while the bitmap is out of date, draw it
     *right away and report the result of that update*/
    if(enable && barcode->needs_draw) return lv_barcode_update(obj);
    return LV_RESULT_OK;
}

bool lv_barcode_get_auto_update(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return false);
    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->auto_update;
}

const char * lv_barcode_get_data(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return NULL);
    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->data;
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

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->data = NULL;
    barcode->dark_color = lv_color_black();
    barcode->light_color = lv_color_white();
    barcode->scale = 1;
    barcode->direction = LV_DIR_HOR;
    barcode->encoding = LV_BARCODE_ENCODING_CODE128_GS1;
    barcode->auto_update = true;
    barcode->needs_draw = false;
    barcode->resizing = false;
    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_DEFAULT);
}

static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    if(barcode->data) {
        lv_free(barcode->data);
        barcode->data = NULL;
    }

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

    if(code == LV_EVENT_DRAW_MAIN_BEGIN) {
        lv_obj_t * obj = lv_event_get_current_target(e);
        lv_barcode_t * barcode = (lv_barcode_t *)obj;
        if(barcode->needs_draw) {
            LV_LOG_WARN("barcode properties changed without a manual lv_barcode_update(); "
                        "drawing on redraw. Call lv_barcode_update() after setting properties.");
            barcode_draw(obj);
        }
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        /*The non-tiled bitmap size depends on the object size, so re-fit the canvas.
         *This runs during layout (before rendering), so reallocating the canvas here
         *is safe. `barcode->resizing` skips the size change our own reallocation causes.*/
        lv_obj_t * obj = lv_event_get_current_target(e);
        lv_barcode_t * barcode = (lv_barcode_t *)obj;
        if(barcode->data != NULL && !barcode->resizing) barcode_mark_dirty(obj);
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

    /*Reuse the current buffer if it already has the required geometry, to avoid a
     *needless reallocation (and the invalidation / relayout it triggers) when the
     *bitmap is regenerated at an unchanged size, e.g. re-encoding the same data.*/
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
    uint32_t len = lv_strlen(data);
    char * new_data = lv_malloc(len + 1);
    LV_ASSERT_MALLOC(new_data);
    if(new_data == NULL) return false;

    lv_memcpy(new_data, data, len);
    new_data[len] = '\0';

    if(barcode->data) lv_free(barcode->data);
    barcode->data = new_data;
    return true;
}

/*Apply a change that affects the bitmap size: (re)size the canvas now - which is
 *safe because every caller is a setter or the SIZE_CHANGED (layout) event, never the
 *draw pass - then either draw immediately (auto) or defer the fill (manual / redraw).*/
static lv_result_t barcode_mark_dirty(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Nothing to render until there is data*/
    if(barcode->data == NULL) return LV_RESULT_INVALID;

    if(!barcode_resize(obj)) {
        barcode->needs_draw = false;
        return LV_RESULT_INVALID;
    }

    barcode->needs_draw = true;

    if(barcode->auto_update) {
        lv_result_t res = barcode_draw(obj);
        lv_obj_invalidate(obj);
        return res;
    }

    /*Manual mode: the canvas is sized; fill it on the next lv_barcode_update() or redraw*/
    lv_obj_invalidate(obj);
    return LV_RESULT_OK;
}

static char * barcode_encode(const lv_barcode_t * barcode, int32_t * barcode_w)
{
    const char * data = barcode->data;
    size_t len = code128_estimate_len(data);
    LV_LOG_INFO("data: %s, len = %zu", data, len);

    char * out_buf = lv_malloc(len);
    LV_ASSERT_MALLOC(out_buf);
    if(out_buf == NULL) {
        LV_LOG_ERROR("malloc failed for out_buf");
        return NULL;
    }

    int32_t w = 0;
    switch(barcode->encoding) {
        case LV_BARCODE_ENCODING_CODE128_GS1:
            w = (int32_t) code128_encode_gs1(data, out_buf, len);
            break;
        case LV_BARCODE_ENCODING_CODE128_RAW:
            w = (int32_t) code128_encode_raw(data, out_buf, len);
            break;
    }
    LV_LOG_INFO("barcode width = %" LV_PRId32, w);
    *barcode_w = w;
    return out_buf;
}

static bool barcode_resize(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    const char * data = barcode->data;
    if(data == NULL || lv_strlen(data) == 0) {
        LV_LOG_WARN("data is empty");
        lv_barcode_clear(obj);
        return false;
    }

    /*Encode only to learn the bar count needed to size the canvas; the bar pattern
     *itself is re-encoded when the canvas is actually filled in barcode_draw()*/
    int32_t barcode_w = 0;
    char * out_buf = barcode_encode(barcode, &barcode_w);
    if(out_buf == NULL) {
        lv_barcode_clear(obj);
        return false;
    }
    lv_free(out_buf);

    LV_ASSERT(barcode->scale > 0);
    uint16_t scale = barcode->scale;

    int32_t buf_w;
    int32_t buf_h;

    if(barcode->tiled) {
        buf_w = (barcode->direction == LV_DIR_HOR) ? barcode_w * scale : 1;
        buf_h = (barcode->direction == LV_DIR_VER) ? barcode_w * scale : 1;
    }
    else {
        lv_obj_update_layout(obj);
        buf_w = (barcode->direction == LV_DIR_HOR) ? barcode_w * scale : lv_obj_get_width(obj);
        buf_h = (barcode->direction == LV_DIR_VER) ? barcode_w * scale : lv_obj_get_height(obj);
    }

    /*Guard against the re-entrant SIZE_CHANGED that reallocating may trigger*/
    barcode->resizing = true;
    bool ok = lv_barcode_change_buf_size(obj, buf_w, buf_h);
    barcode->resizing = false;

    if(!ok) {
        lv_barcode_clear(obj);
        return false;
    }
    return true;
}

static lv_result_t barcode_draw(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*This only fills the already-sized canvas (no reallocation), so it is safe to run
     *during the draw pass. The canvas is up to date afterwards.*/
    barcode->needs_draw = false;

    const char * data = barcode->data;
    if(data == NULL || lv_strlen(data) == 0) return LV_RESULT_INVALID;

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return LV_RESULT_INVALID;

    int32_t barcode_w = 0;
    char * out_buf = barcode_encode(barcode, &barcode_w);
    if(out_buf == NULL) return LV_RESULT_INVALID;

    uint16_t scale = barcode->scale;
    int32_t buf_w = (int32_t)draw_buf->header.w;
    int32_t buf_h = (int32_t)draw_buf->header.h;

    /*Make sure the encoded bars fit the buffer barcode_resize() allocated; if the
     *buffer is stale for the current state, skip drawing rather than overrun it.*/
    int32_t scaled = barcode_w * scale;
    int32_t avail = (barcode->direction == LV_DIR_HOR) ? buf_w : buf_h;
    if(scaled > avail) {
        lv_free(out_buf);
        return LV_RESULT_INVALID;
    }

    /* Temporarily disable invalidation to improve the efficiency of lv_canvas_set_px */
    lv_display_enable_invalidation(lv_obj_get_display(obj), false);

    uint32_t stride = draw_buf->header.stride;
    const lv_color_t color = lv_color_hex(1);

    /* Clear the canvas */
    lv_draw_buf_clear(draw_buf, NULL);

    /* Set the palette directly on the draw buffer to avoid an extra invalidation here */
    lv_draw_buf_set_palette(draw_buf, 0, lv_color_to_32(barcode->light_color, LV_OPA_COVER));
    lv_draw_buf_set_palette(draw_buf, 1, lv_color_to_32(barcode->dark_color, LV_OPA_COVER));

    for(int32_t x = 0; x < barcode_w; x++) {
        /*skip empty data*/
        if(out_buf[x] == 0) {
            continue;
        }

        for(uint16_t i = 0; i < scale; i++) {
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

    /* Copy pixels by row */
    if(!barcode->tiled && barcode->direction == LV_DIR_HOR && buf_h > 1) {
        /* Skip the first row */
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

    lv_free(out_buf);
    return LV_RESULT_OK;
}

#endif /*LV_USE_BARCODE*/
