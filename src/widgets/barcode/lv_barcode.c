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
static bool barcode_resize(lv_obj_t * obj);
static void barcode_clear(lv_obj_t * obj);
static bool barcode_store_data(lv_barcode_t * barcode, const char * data);
static uint8_t * barcode_encode_data(const char * data, lv_barcode_encoding_t encoding, int32_t * bar_count);
static void barcode_mark_dirty(lv_obj_t * obj);
static lv_result_t barcode_generate(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_barcode_class = {
    .constructor_cb = lv_barcode_constructor,
    .destructor_cb = lv_barcode_destructor,
    .event_cb = lv_barcode_event,
    /*As long as the bars need, and thick enough to be scannable out of the box. The
     *thickness is a plain size, so lv_obj_set_height() overrides it as usual.*/
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_DPI_DEF,
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

void lv_barcode_set_size(lv_obj_t * obj, int32_t size)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG_MSG(size > 0, return, "size must be at least 1");

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->size == size) return;

    const int32_t old_size = barcode->size;
    barcode->size = size;
    if(!barcode_resize(obj)) {
        /*Put back what the canvas still holds, so asking for this size again retries*/
        barcode->size = old_size;
        return;
    }

    /*The new buffer is empty; regenerate the bars into it*/
    barcode_mark_dirty(obj);
}

void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->scale == scale) return;
    barcode->scale = scale;
    barcode_mark_dirty(obj);
}

void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG_MSG(direction == LV_DIR_HOR || direction == LV_DIR_VER, return,
                     "direction must be LV_DIR_HOR or LV_DIR_VER");

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->direction == direction) return;

    /*The canvas is one pixel thick, so the two directions need a different buffer*/
    const lv_dir_t old_direction = barcode->direction;
    barcode->direction = direction;
    if(!barcode_resize(obj)) {
        /*Put back what the canvas still holds, so asking for this direction again retries*/
        barcode->direction = old_direction;
        return;
    }

    barcode_mark_dirty(obj);
}

void lv_barcode_set_encoding(lv_obj_t * obj, lv_barcode_encoding_t encoding)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(barcode->encoding == encoding) return;
    barcode->encoding = encoding;
    barcode_mark_dirty(obj);
}

void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Nothing to do if the color is unchanged: skip the palette write and cache drop*/
    if(lv_color_eq(barcode->dark_color, color)) return;
    barcode->dark_color = color;

    /*Apply the color right away so it takes effect even if the bars have already been
     *generated (e.g. the color is set after the data)*/
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

    /*Apply the color right away so it takes effect even if the bars have already been
     *generated (e.g. the color is set after the data)*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 0, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

lv_result_t lv_barcode_set_data(lv_obj_t * obj, const char * data)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    LV_CHECK_ARG_MSG(data != NULL, return LV_RESULT_INVALID, "data must not be NULL");

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Empty data encodes to the guard bars alone, which is not a barcode anyone wants.
     *Forget what was there and show nothing.*/
    if(data[0] == '\0') {
        LV_LOG_WARN("data is empty, clearing the barcode");
        lv_free(barcode->data);
        barcode->data = NULL;
        barcode->needs_update = false;
        barcode->render_valid = false;
        barcode_clear(obj);
        return LV_RESULT_INVALID;
    }

    /*Keep a copy of the data so a later property change can regenerate the bars*/
    if(!barcode_store_data(barcode, data)) return LV_RESULT_INVALID;

    /*New data leaves the bitmap out of date, and makes any earlier failure moot*/
    barcode->needs_update = true;
    barcode->render_valid = true;

    /*Setting the data always generates right away, in either update mode*/
    return lv_barcode_render(obj);
}

lv_result_t lv_barcode_render(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);

    lv_result_t res = barcode_generate(obj);
    lv_obj_invalidate(obj);
    return res;
}

void lv_barcode_set_update_mode(lv_obj_t * obj, lv_barcode_update_mode_t mode)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Going back to immediate mode has to apply whatever was deferred, but this setter
     *returns void, so a failure would be dropped. Warn and generate anyway: the order that
     *can report the failure is lv_barcode_render() first, then switch mode.*/
    if(mode == LV_BARCODE_UPDATE_MODE_IMMEDIATE && barcode->needs_update) {
        barcode_generate(obj);
        if(!barcode->render_valid) {
            LV_LOG_ERROR("regenerating on the switch to immediate update mode failed; "
                         "call lv_barcode_render() before switching the mode to get the result");
        }
        else {
            LV_LOG_WARN("switching to immediate update mode while the bitmap was out of date; "
                        "call lv_barcode_render() before switching the mode to get the result");
        }
        lv_obj_invalidate(obj);
    }

    barcode->update_mode = mode;
}

lv_barcode_update_mode_t lv_barcode_get_update_mode(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_BARCODE_UPDATE_MODE_IMMEDIATE);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return (lv_barcode_update_mode_t)barcode->update_mode;
}

bool lv_barcode_is_render_valid(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return false);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->render_valid;
}

const char * lv_barcode_get_data(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return NULL);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->data;
}

int32_t lv_barcode_get_size(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->size;
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
    barcode->size = 0;
    barcode->scale = 0;
    barcode->direction = LV_DIR_HOR;
    barcode->encoding = LV_BARCODE_ENCODING_CODE128_GS1;
    barcode->update_mode = LV_BARCODE_UPDATE_MODE_IMMEDIATE;
    barcode->needs_update = false;
    /*No bitmap has been generated yet, so there is nothing valid to report*/
    barcode->render_valid = false;

    /*The canvas holds a strip one pixel thick and the tiling repeats it across whatever
     *thickness the application gives the Widget*/
    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_TILE);

    /*Set default size. Code 128 needs 11 modules per character plus 35 for the guards and
     *the checksum, so a smaller default would not hold a payload worth encoding.*/
    lv_barcode_set_size(obj, LV_DPI_DEF * 2);
}

static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_ASSERT(obj != NULL);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    lv_free(barcode->data);
    barcode->data = NULL;

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

    /*Call the ancestor's event handler*/
    lv_result_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    if(code == LV_EVENT_DRAW_MAIN_BEGIN) {
        lv_obj_t * obj = lv_event_get_current_target(e);
        lv_barcode_t * barcode = (lv_barcode_t *)obj;
        /*A state that already failed is not retried here: it can only start working again
         *when a property changes, and that sets `render_valid` again.*/
        if(barcode->needs_update && barcode->render_valid) {
            /*Only deferred mode leaves the bitmap out of date; immediate mode regenerates in the setter*/
            LV_ASSERT(barcode->update_mode == LV_BARCODE_UPDATE_MODE_DEFERRED);

            /*Deferred mode expects an explicit lv_barcode_render() once the properties are
             *set. Generating here still produces the right bitmap, but it charges the work
             *to this refresh and there is no caller left to return the result to.*/
            LV_LOG_WARN("regenerating the barcode during the redraw because "
                        "lv_barcode_render() was not called after the property changes; this adds the "
                        "work to the refresh and its result cannot be reported");

            if(barcode_generate(obj) != LV_RESULT_OK) {
                /*Nothing here can return the failure to the application, so report it*/
                LV_LOG_ERROR("the barcode could not be regenerated during the redraw "
                             "(scale %d, %s); the bitmap is left blank",
                             (int)barcode->scale, barcode->direction == LV_DIR_VER ? "vertical" : "horizontal");
            }
        }
    }
}

/**
 * Give the canvas the geometry the current size and direction ask for: a strip one pixel
 * thick and `size` long. The thickness comes from the image tiling, not from the buffer,
 * so nothing here depends on the Widget's own size.
 */
static bool barcode_resize(lv_obj_t * obj)
{
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    const bool hor = (barcode->direction == LV_DIR_HOR);
    const int32_t w = hor ? barcode->size : 1;
    const int32_t h = hor ? 1 : barcode->size;

    lv_draw_buf_t * old_buf = lv_canvas_get_draw_buf(obj);
    lv_draw_buf_t * new_buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_I1, LV_STRIDE_AUTO);
    if(new_buf == NULL) {
        LV_LOG_ERROR("malloc failed for canvas buffer");
        return false;
    }

    lv_canvas_set_draw_buf(obj, new_buf);
    LV_LOG_INFO("set canvas buffer: %p, size = %" LV_PRId32 " x %" LV_PRId32, (void *)new_buf, w, h);

    /*Clear canvas buffer*/
    lv_draw_buf_clear(new_buf, NULL);

    if(old_buf != NULL) lv_draw_buf_destroy(old_buf);
    return true;
}

static void barcode_clear(lv_obj_t * obj)
{
    LV_ASSERT(obj != NULL);

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;

    lv_draw_buf_clear(draw_buf, NULL);
    lv_image_cache_drop(draw_buf);
    lv_obj_invalidate(obj);
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

static void barcode_mark_dirty(lv_obj_t * obj)
{
    LV_ASSERT(obj != NULL);
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Nothing to regenerate until there is data*/
    if(barcode->data == NULL) return;

    barcode->needs_update = true;

    /*The property change may well make the bars fit again, so allow a new attempt*/
    barcode->render_valid = true;

    /*Deferred mode collapses several changes into one regeneration on the next redraw*/
    if(barcode->update_mode == LV_BARCODE_UPDATE_MODE_IMMEDIATE) barcode_generate(obj);

    lv_obj_invalidate(obj);
}

static lv_result_t barcode_generate(lv_obj_t * obj)
{
    LV_ASSERT(obj != NULL);
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    /*Start invalid and settle both flags only on the single success path, so no early
     *return can forget to record the outcome. `needs_update` deliberately survives a
     *failure: the bitmap still does not match the properties, and reporting it as up to
     *date would be a lie. A cleared `render_valid` is what keeps the draw hook from
     *retrying a known-bad state on every frame; a property change sets it again to allow a
     *new attempt. Failures are never logged here - the result is returned, and it is up to
     *the caller to report it if nothing else will.*/
    barcode->render_valid = false;

    if(barcode->data == NULL) return LV_RESULT_INVALID;

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return LV_RESULT_INVALID;

    lv_draw_buf_clear(draw_buf, NULL);
    /*Set the palette directly on the draw buffer to avoid an extra invalidation here;
     *the caller (or the draw pass) takes care of refreshing the object*/
    lv_draw_buf_set_palette(draw_buf, 0, lv_color_to_32(barcode->light_color, LV_OPA_COVER));
    lv_draw_buf_set_palette(draw_buf, 1, lv_color_to_32(barcode->dark_color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);

    int32_t bar_count = 0;
    uint8_t * pattern = barcode_encode_data(barcode->data, barcode->encoding, &bar_count);
    if(pattern == NULL) return LV_RESULT_INVALID;

    /*The buffer is what the bars have to fit, which is the size the last resize managed*/
    const bool hor = (barcode->direction == LV_DIR_HOR);
    const int32_t avail = hor ? (int32_t)draw_buf->header.w : (int32_t)draw_buf->header.h;

    /*Canvas pixels per bar. A fitted scale of zero means not even a one pixel wide bar
     *fits, which would leave the bitmap blank, so report that instead of succeeding.*/
    const int32_t scale = barcode->scale ? barcode->scale : avail / bar_count;
    if(scale <= 0 || bar_count > avail / scale) {
        LV_LOG_WARN("%" LV_PRId32 " bars of %" LV_PRId32 " px do not fit %" LV_PRId32 " px",
                    bar_count, scale, avail);
        lv_free(pattern);
        return LV_RESULT_INVALID;
    }

    /*Centre the bars in the leftover space, as the QR code does*/
    const int32_t margin = (avail - bar_count * scale) / 2;

    /*Temporarily disable invalidation to improve the efficiency of lv_canvas_set_px*/
    lv_display_enable_invalidation(lv_obj_get_display(obj), false);

    const lv_color_t color = lv_color_hex(1);   /*Palette index 1, the dark color*/

    for(int32_t bar = 0; bar < bar_count; bar++) {
        /*A light bar is already there: the buffer was cleared to palette index 0*/
        if(pattern[bar] == 0) continue;

        for(int32_t i = 0; i < scale; i++) {
            const int32_t px = margin + bar * scale + i;
            if(hor) lv_canvas_set_px(obj, px, 0, color, LV_OPA_COVER);
            else lv_canvas_set_px(obj, 0, px, color, LV_OPA_COVER);
        }
    }

    lv_display_enable_invalidation(lv_obj_get_display(obj), true);

    lv_free(pattern);

    /*Only now does the bitmap match the properties*/
    barcode->needs_update = false;
    barcode->render_valid = true;
    return LV_RESULT_OK;
}

#endif /*LV_USE_BARCODE*/
