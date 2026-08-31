/**
 * @file lv_qrcode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_class_private.h"
#include "../../lvgl_public.h"
#include "lv_qrcode_private.h"

#if LV_USE_QRCODE

#include "../../misc/cache/lv_cache.h"
#include "../../libs/qrcode/qrcodegen.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_qrcode_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_qrcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_qrcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_qrcode_event(const lv_obj_class_t * class_p, lv_event_t * e);
static int32_t get_satisfied_size(int32_t min_version, int32_t size, int32_t * scale);
static bool qrcode_store_data(lv_qrcode_t * qrcode, const void * data, uint32_t data_len);
static lv_result_t qrcode_encode(lv_obj_t * obj);
static void qrcode_mark_dirty(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_qrcode_class = {
    .constructor_cb = lv_qrcode_constructor,
    .destructor_cb = lv_qrcode_destructor,
    .event_cb = lv_qrcode_event,
    .instance_size = sizeof(lv_qrcode_t),
    .base_class = &lv_canvas_class,
    .name = "lv_qrcode",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_qrcode_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_qrcode_set_size(lv_obj_t * obj, int32_t size)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_draw_buf_t * old_buf = lv_canvas_get_draw_buf(obj);
    lv_draw_buf_t * new_buf = lv_draw_buf_create(size, size, LV_COLOR_FORMAT_I1, LV_STRIDE_AUTO);
    if(new_buf == NULL) {
        LV_LOG_ERROR("malloc failed for canvas buffer");
        return;
    }

    lv_canvas_set_draw_buf(obj, new_buf);
    LV_LOG_INFO("set canvas buffer: %p, size = %d", (void *)new_buf, (int)size);

    /*Clear canvas buffer*/
    lv_draw_buf_clear(new_buf, NULL);

    if(old_buf != NULL) lv_draw_buf_destroy(old_buf);

    /*The new buffer is empty; re-render the QR code into it*/
    qrcode_mark_dirty(obj);
}

void lv_qrcode_set_dark_color(lv_obj_t * obj, lv_color_t color)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Nothing to do if the color is unchanged: skip the palette write and cache drop*/
    if(lv_color_eq(qrcode->dark_color, color)) return;
    qrcode->dark_color = color;

    /*Apply the color right away so it takes effect even if the QR code has
     *already been rendered (e.g. the color is set after the data)*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 1, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

void lv_qrcode_set_light_color(lv_obj_t * obj, lv_color_t color)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Nothing to do if the color is unchanged: skip the palette write and cache drop*/
    if(lv_color_eq(qrcode->light_color, color)) return;
    qrcode->light_color = color;

    /*Apply the color right away so it takes effect even if the QR code has
     *already been rendered (e.g. the color is set after the data)*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 0, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

lv_result_t lv_qrcode_update(lv_obj_t * obj, const void * data, uint32_t data_len)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    LV_CHECK_ARG_MSG(data != NULL, return LV_RESULT_INVALID, "data must not be NULL");
    LV_CHECK_ARG_FORMAT_MSG(data_len <= qrcodegen_BUFFER_LEN_MAX, return LV_RESULT_INVALID,
                            "data_len %u exceeds the maximum %u",
                            (unsigned)data_len, (unsigned)qrcodegen_BUFFER_LEN_MAX);

    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Keep a copy of the payload so a later size or quiet zone change can re-encode it*/
    if(!qrcode_store_data(qrcode, data, data_len)) return LV_RESULT_INVALID;

    /*A new payload leaves the bitmap out of date, and makes any earlier failure moot*/
    qrcode->needs_update = true;
    qrcode->render_failed = false;

    /*Setting the data always encodes right away, in either update mode*/
    return lv_qrcode_render(obj);
}

void lv_qrcode_set_data(lv_obj_t * obj, const char * data)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG_MSG(data != NULL, return, "data must not be NULL");

    lv_qrcode_update(obj, data, lv_strlen(data));
}

lv_result_t lv_qrcode_render(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);

    lv_result_t res = qrcode_encode(obj);
    lv_obj_invalidate(obj);
    return res;
}

void lv_qrcode_set_quiet_zone(lv_obj_t * obj, bool enable)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Nothing to do if the quiet zone is unchanged: skip the re-encode*/
    if(qrcode->quiet_zone == enable) return;
    qrcode->quiet_zone = enable;
    qrcode_mark_dirty(obj);
}

void lv_qrcode_set_update_mode(lv_obj_t * obj, lv_qrcode_update_mode_t mode)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Going back to immediate mode has to apply whatever was deferred, but this setter
     *returns void, so an encode failure would be dropped. Warn and encode anyway: the
     *order that can report the failure is lv_qrcode_update() first, then switch mode.*/
    if(mode == LV_QRCODE_UPDATE_MODE_IMMEDIATE && qrcode->needs_update) {
        qrcode_encode(obj);
        if(qrcode->render_failed) {
            LV_LOG_ERROR("re-encoding on the switch to immediate update mode failed; "
                         "call lv_qrcode_render() before switching the mode to get the result");
        }
        else {
            LV_LOG_WARN("switching to immediate update mode while the bitmap was out of date; "
                        "call lv_qrcode_render() before switching the mode to get the result");
        }
        lv_obj_invalidate(obj);
    }

    qrcode->update_mode = mode;
}

lv_qrcode_update_mode_t lv_qrcode_get_update_mode(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_QRCODE_UPDATE_MODE_IMMEDIATE);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    return (lv_qrcode_update_mode_t)qrcode->update_mode;
}

bool lv_qrcode_get_render_failed(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return true);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    return qrcode->render_failed;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_qrcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_ASSERT(obj != NULL);

    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    qrcode->data = NULL;
    qrcode->data_len = 0;
    qrcode->update_mode = LV_QRCODE_UPDATE_MODE_IMMEDIATE;
    qrcode->needs_update = false;
    /*No bitmap has been generated yet, so there is nothing valid to report*/
    qrcode->render_failed = true;

    /*Set default size*/
    lv_qrcode_set_size(obj, LV_DPI_DEF);

    /*Set default color*/
    lv_qrcode_set_dark_color(obj, lv_color_black());
    lv_qrcode_set_light_color(obj, lv_color_white());
}

static void lv_qrcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_ASSERT(obj != NULL);

    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    if(qrcode->data) {
        lv_free(qrcode->data);
        qrcode->data = NULL;
    }

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_image_cache_drop(draw_buf);

    /*@fixme destroy buffer in cache free_cb.*/
    lv_draw_buf_destroy(draw_buf);
}

static void lv_qrcode_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    LV_ASSERT(e != NULL);

    lv_event_code_t code = lv_event_get_code(e);

    /*Call the ancestor's event handler*/
    lv_result_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    if(code == LV_EVENT_DRAW_MAIN_BEGIN) {
        lv_obj_t * obj = lv_event_get_current_target(e);
        lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
        /*A state that already failed to encode is not retried here: it can only start
         *working again when a property changes, and that clears `render_failed`.*/
        if(qrcode->needs_update && !qrcode->render_failed) {
            /*Only deferred mode leaves the bitmap out of date; immediate mode re-encodes in the setter*/
            LV_ASSERT(qrcode->update_mode == LV_QRCODE_UPDATE_MODE_DEFERRED);

            /*Deferred mode expects an explicit lv_qrcode_render() once the properties are
             *set. Encoding here still produces the right bitmap, but it charges the encode
             *to this refresh and there is no caller left to return the result to.*/
            LV_LOG_WARN("re-encoding the QR code during the redraw because "
                        "lv_qrcode_render() was not called after the property changes; this adds the "
                        "encode to the refresh and its result cannot be reported");

            /*Fill the buffer only; invalidating during the draw pass is not needed*/
            if(qrcode_encode(obj) != LV_RESULT_OK) {
                /*Nothing here can return the failure to the application, so report it*/
                LV_LOG_ERROR("the QR code could not be re-encoded during the redraw "
                             "(%u bytes, quiet zone %s); the bitmap is left blank",
                             (unsigned)qrcode->data_len, qrcode->quiet_zone ? "on" : "off");
            }
        }
    }
}

static int32_t get_satisfied_size(int32_t min_version, int32_t size, int32_t * scale)
{
    LV_ASSERT(scale != NULL);
    LV_ASSERT(min_version >= 0);

    int32_t offset = size;
    int32_t satisfied_version = min_version;
    *scale = 0;

    for(int32_t version = min_version; version <= min_version + 2 && version <= qrcodegen_VERSION_MAX - 3; version++) {
        int32_t version_size = qrcodegen_version2size(version + 1);
        int32_t tmp_offset = size % version_size;
        int32_t tmp_scale = size / version_size;

        if(tmp_offset < offset) {
            offset = tmp_offset;
            satisfied_version = version;
            *scale = tmp_scale;
        }
    }
    return satisfied_version;
}

static bool qrcode_store_data(lv_qrcode_t * qrcode, const void * data, uint32_t data_len)
{
    LV_ASSERT(qrcode != NULL);
    LV_ASSERT(data != NULL);

    uint8_t * new_data = lv_realloc(qrcode->data, data_len);
    LV_ASSERT_MALLOC(new_data);
    if(new_data == NULL) return false;

    lv_memcpy(new_data, data, data_len);

    qrcode->data = new_data;
    qrcode->data_len = data_len;
    return true;
}

static void qrcode_mark_dirty(lv_obj_t * obj)
{
    LV_ASSERT(obj != NULL);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Nothing to regenerate until there is data*/
    if(qrcode->data == NULL) return;

    qrcode->needs_update = true;

    /*The property change may well make the payload encodable again, so allow a new attempt*/
    qrcode->render_failed = false;

    /*Deferred mode collapses several changes into one re-encode on the next redraw*/
    if(qrcode->update_mode == LV_QRCODE_UPDATE_MODE_IMMEDIATE) qrcode_encode(obj);

    lv_obj_invalidate(obj);
}

static lv_result_t qrcode_encode(lv_obj_t * obj)
{
    LV_ASSERT(obj != NULL);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Assume failure and clear both flags only on the single success path, so no early
     *return can forget to record the outcome. `needs_update` deliberately survives a
     *failure: the bitmap still does not match the properties, and reporting it as up to
     *date would be a lie. `render_failed` is what keeps the draw hook from retrying a
     *known-bad state on every frame; a property change clears it to allow a new attempt.
     *Failures are never logged here - the result is returned, and it is up to the caller
     *to report it if nothing else will.*/
    qrcode->render_failed = true;

    if(qrcode->data == NULL) return LV_RESULT_INVALID;

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return LV_RESULT_INVALID;

    const void * data = qrcode->data;
    const uint32_t data_len = qrcode->data_len;

    lv_draw_buf_clear(draw_buf, NULL);
    /*Set the palette directly on the draw buffer to avoid an extra invalidation here;
     *the caller (or the draw pass) takes care of refreshing the object*/
    lv_draw_buf_set_palette(draw_buf, 0, lv_color_to_32(qrcode->light_color, LV_OPA_COVER));
    lv_draw_buf_set_palette(draw_buf, 1, lv_color_to_32(qrcode->dark_color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);

    if(data_len > qrcodegen_BUFFER_LEN_MAX) return LV_RESULT_INVALID;

    int32_t qr_version = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, data_len);
    if(qr_version <= 0) {
        return LV_RESULT_INVALID;
    }
    int32_t quiet_zone_scale = 0;
    if(qrcode->quiet_zone) qr_version = get_satisfied_size(qr_version, draw_buf->header.w, &quiet_zone_scale);
    LV_ASSERT(qr_version > 0);

    const int32_t qr_size = qrcodegen_version2size(qr_version);
    LV_ASSERT(qr_size > 0);

    /*Canvas pixels per QR module. Zero means the canvas cannot hold even a 1:1 copy of
     *the code, which would leave the bitmap blank, so report that instead of succeeding.*/
    const int32_t scale = qrcode->quiet_zone ? quiet_zone_scale : draw_buf->header.w / qr_size;
    if(scale <= 0) return LV_RESULT_INVALID;

    uint8_t * qr0 = lv_malloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_MALLOC(qr0);
    uint8_t * data_tmp = lv_malloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_MALLOC(data_tmp);

    if(qr0 == NULL || data_tmp == NULL) {
        lv_free(qr0);
        lv_free(data_tmp);
        return LV_RESULT_INVALID;
    }

    lv_memcpy(data_tmp, data, data_len);

    bool ok = qrcodegen_encodeBinary(data_tmp, data_len,
                                     qr0, qrcodegen_Ecc_MEDIUM,
                                     qr_version, qr_version,
                                     qrcodegen_Mask_AUTO, true);

    if(!ok) {
        lv_free(qr0);
        lv_free(data_tmp);
        return LV_RESULT_INVALID;
    }

    /* Temporarily disable invalidation to improve the efficiency of lv_canvas_set_px */
    lv_display_enable_invalidation(lv_obj_get_display(obj), false);

    int32_t obj_w = draw_buf->header.w;
    int scaled = qr_size * scale;
    int margin = (obj_w - scaled) / 2;
    uint8_t * buf_u8 = draw_buf->data + 8;    /*+8 skip the palette*/
    lv_color_t c = lv_color_hex(1);

    /* Copy the qr code canvas:
     * A simple `lv_canvas_set_px` would work but it's slow for so many pixels.
     * So buffer 1 byte (8 px) from the qr code and set it in the canvas image */
    uint32_t row_byte_cnt = draw_buf->header.stride;
    int y;
    for(y = margin; y < scaled + margin; y += scale) {
        uint8_t b = 0;
        uint8_t p = 0;
        bool aligned = false;
        int x;
        for(x = margin; x < scaled + margin; x++) {
            bool a = qrcodegen_getModule(qr0, (x - margin) / scale, (y - margin) / scale);

            if(aligned == false && (x & 0x7) == 0) aligned = true;

            if(aligned == false) {
                if(a) {
                    lv_canvas_set_px(obj, x, y, c, LV_OPA_COVER);
                }
            }
            else {
                if(!a) b |= (1 << (7 - p));
                p++;
                if(p == 8) {
                    uint32_t px = row_byte_cnt * y + (x >> 3);
                    buf_u8[px] = ~b;
                    b = 0;
                    p = 0;
                }
            }
        }

        /*Process the last byte of the row*/
        if(p) {
            /*Make the rest of the bits white*/
            b |= (1 << (8 - p)) - 1;

            uint32_t px = row_byte_cnt * y + (x >> 3);
            buf_u8[px] = ~b;
        }

        /*The Qr is probably scaled so simply to the repeated rows*/
        int s;
        const uint8_t * row_ori = buf_u8 + row_byte_cnt * y;
        for(s = 1; s < scale; s++) {
            lv_memcpy((uint8_t *)buf_u8 + row_byte_cnt * (y + s), row_ori, row_byte_cnt);
        }
    }

    lv_display_enable_invalidation(lv_obj_get_display(obj), true);

    lv_free(qr0);
    lv_free(data_tmp);

    /*Only now does the bitmap match the properties*/
    qrcode->needs_update = false;
    qrcode->render_failed = false;
    return LV_RESULT_OK;
}

#endif /*LV_USE_QRCODE*/
