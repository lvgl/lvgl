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
static bool qrcode_data_is_string(const uint8_t * data, uint32_t data_len);
static uint32_t qrcode_payload_len(const lv_qrcode_t * qrcode);
static lv_result_t qrcode_render(lv_obj_t * obj);
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

    if(qrcode->light_color == color) return;
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

    if(qrcode->light_color == color) return;
    qrcode->light_color = color;

    /*Apply the color right away so it takes effect even if the QR code has
     *already been rendered (e.g. the color is set after the data)*/
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_canvas_set_palette(obj, 0, lv_color_to_32(color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);
}

lv_result_t lv_qrcode_set_data(lv_obj_t * obj, const char * data)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    LV_CHECK_ARG(data != NULL, return LV_RESULT_INVALID, "data must not be NULL");

    /*One byte of the limit is reserved for the NUL terminator stored below, so that
     *the stored length stays within the same qrcodegen_BUFFER_LEN_MAX bound that
     *lv_qrcode_set_data_binary() enforces*/
    const size_t len = lv_strlen(data);
    LV_CHECK_ARG(len <= qrcodegen_BUFFER_LEN_MAX - 1, return LV_RESULT_INVALID,
                 "data length %u exceeds the maximum %u",
                 (unsigned)len, (unsigned)(qrcodegen_BUFFER_LEN_MAX - 1));

    /*Store the string together with its NUL terminator. That extra byte both makes
     *the stored copy usable as a C string and lets it be told apart from binary data
     *(a string ends with a NUL and has no other NUL in it).*/
    return lv_qrcode_set_data_binary(obj, data, (uint32_t)len + 1);
}

lv_result_t lv_qrcode_set_data_binary(lv_obj_t * obj, const void * data, uint32_t data_len)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    LV_CHECK_ARG(data != NULL, return LV_RESULT_INVALID, "data must not be NULL");
    LV_CHECK_ARG(data_len <= qrcodegen_BUFFER_LEN_MAX, return LV_RESULT_INVALID,
                 "data_len %u exceeds the maximum %u",
                 (unsigned)data_len, (unsigned)qrcodegen_BUFFER_LEN_MAX);

    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Store a copy of the data so the bitmap can be regenerated later
     *(e.g. when the size or quiet zone changes after the data was set)*/
    if(!qrcode_store_data(qrcode, data, data_len)) return LV_RESULT_INVALID;

    qrcode->needs_update = true;
    if(qrcode->auto_update) return lv_qrcode_update(obj);

    /*Manual mode: regenerate on lv_qrcode_update()*/
    lv_obj_invalidate(obj);
    return LV_RESULT_OK;
}

lv_result_t lv_qrcode_update(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*The bitmap is already up to date: keep two consecutive calls no more expensive
     *than one. The outcome of the last generation is remembered, so skipping the work
     *never turns a failure into a claim of success.*/
    if(!qrcode->needs_update) return qrcode->render_failed ? LV_RESULT_INVALID : LV_RESULT_OK;

    lv_result_t res = qrcode_render(obj);   /*clears needs_update, updates render_failed*/
    lv_obj_invalidate(obj);
    return res;
}

void lv_qrcode_set_quiet_zone(lv_obj_t * obj, bool enable)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    if(qrcode->quite_zone == enable) return;
    qrcode->quiet_zone = enable;
    qrcode_mark_dirty(obj);
}

void lv_qrcode_set_auto_update(lv_obj_t * obj, bool enable)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    qrcode->auto_update = enable;

    /*If auto update is turned back on while the bitmap is out of date, regenerate
     *right away and report the result of that update*/
    if(enable && qrcode->needs_update) lv_qrcode_update(obj);
}

bool lv_qrcode_get_auto_update(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return false);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    return qrcode->auto_update;
}

const char * lv_qrcode_get_data(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return NULL);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Only a NUL terminated string with no embedded NUL is safe to return as a C
     *string. Binary data returns NULL; use lv_qrcode_get_data_binary() for it.*/
    if(!qrcode_data_is_string(qrcode->data, qrcode->data_len)) return NULL;

    return (const char *)qrcode->data;
}

uint32_t lv_qrcode_get_data_binary(lv_obj_t * obj, void * buf, uint32_t buf_size)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Report the payload (a string's stored NUL terminator is not part of it)*/
    uint32_t payload_len = qrcode_payload_len(qrcode);

    if(buf != NULL && buf_size > 0 && qrcode->data != NULL) {
        uint32_t copy_len = payload_len < buf_size ? payload_len : buf_size;
        lv_memcpy(buf, qrcode->data, copy_len);
    }

    /*Always return the full size so the caller can detect truncation (size > buf_size).
     *Passing buf == NULL / buf_size == 0 therefore also serves as a size query.*/
    return payload_len;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_qrcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    qrcode->data = NULL;
    qrcode->data_len = 0;
    qrcode->auto_update = true;
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

    lv_event_code_t code = lv_event_get_code(e);

    /*Call the ancestor's event handler*/
    lv_result_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    if(code == LV_EVENT_DRAW_MAIN_BEGIN) {
        lv_obj_t * obj = lv_event_get_current_target(e);
        lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
        if(qrcode->needs_update) {
            LV_ASSERT(!qrcode->auto_mode);
            LV_LOG_WARN("QR code properties changed without a manual lv_qrcode_update(); "
                        "regenerating on redraw. Call lv_qrcode_update() after setting properties.");
            /*Fill the buffer only; invalidating during the draw pass is not needed*/
            qrcode_render(obj);
        }
    }
}

static int32_t get_satisfied_size(int32_t min_version, int32_t size, int32_t * scale)
{
    if(min_version <= 0) return -1;

    int32_t offset = size;
    int32_t satisfied_version = min_version;
    if(scale) *scale = 0;

    for(int32_t version = min_version; version <= min_version + 2 && version <= qrcodegen_VERSION_MAX - 3; version++) {
        int32_t version_size = qrcodegen_version2size(version + 1);
        int32_t tmp_offset = size % version_size;
        int32_t tmp_scale = size / version_size;

        if(tmp_offset < offset) {
            offset = tmp_offset;
            satisfied_version = version;
            if(scale) *scale = tmp_scale;
        }
    }
    return satisfied_version;
}

static bool qrcode_store_data(lv_qrcode_t * qrcode, const void * data, uint32_t data_len)
{
    /*`data_len` bytes are copied verbatim. The caller decides what is stored:
     *the string setter includes the trailing NUL in `data_len`, the binary setter does not.
     *Both public setters keep `data_len` <= qrcodegen_BUFFER_LEN_MAX via LV_CHECK_ARG (the string
     *setter caps the string itself one byte lower to make room for the NUL), so it fits the
     *12-bit `data_len` field. Only skipped when arg checks are disabled.*/

    if(data == qrcode->data) {
        qrcode->data_len = data_len;
        return true;
    }

    uint8_t * new_data = lv_realloc(qrcode->data, data_len);
    if(new_data == NULL) return false;

    lv_memcpy(new_data, data, data_len);

    qrcode->data = new_data;
    qrcode->data_len = data_len;
    return true;
}

static bool qrcode_data_is_string(const uint8_t * data, uint32_t data_len)
{
    /*Heuristic: the stored data is a string if it ends with a NUL and contains no
     *other NUL byte (i.e. the terminator is the only one).*/
    if(data == NULL || data_len == 0) return false;
    if(data[data_len - 1] != '\0') return false;
    return lv_strlen((const char *)data) == (size_t)(data_len - 1);
}

static uint32_t qrcode_payload_len(const lv_qrcode_t * qrcode)
{
    /*The encoded/visible payload excludes a string's stored NUL terminator*/
    if(qrcode->data == NULL) return 0;
    if(qrcode_data_is_string(qrcode->data, qrcode->data_len)) return qrcode->data_len - 1;
    return qrcode->data_len;
}

static void qrcode_mark_dirty(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Nothing to regenerate until there is data*/
    if(qrcode->data == NULL) return;

    qrcode->needs_update = true;
    if(qrcode->auto_update) {
        lv_qrcode_update(obj);
    }
    else {
        lv_obj_invalidate(obj);
    }
}

static lv_result_t qrcode_render(lv_obj_t * obj)
{
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    /*Clear the flag up front (not only on success): if the data can't be encoded
     *at this size/quiet zone the failure is permanent until a property changes, so
     *leaving it set would make the draw-hook re-render and warn on every frame.
     *Assume failure and clear `render_failed` only on the single success path, so
     *that no early return can forget to record it.*/
    qrcode->needs_update = false;
    qrcode->render_failed = true;

    if(qrcode->data == NULL) return LV_RESULT_INVALID;

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) {
        LV_LOG_ERROR("canvas draw buffer is NULL");
        return LV_RESULT_INVALID;
    }

    const void * data = qrcode->data;
    /*Encode the payload only: a string's stored NUL terminator is not part of it*/
    const uint32_t data_len = qrcode_payload_len(qrcode);

    lv_draw_buf_clear(draw_buf, NULL);
    /*Set the palette directly on the draw buffer to avoid an extra invalidation here;
     *the caller (or the draw pass) takes care of refreshing the object*/
    lv_draw_buf_set_palette(draw_buf, 0, lv_color_to_32(qrcode->light_color, LV_OPA_COVER));
    lv_draw_buf_set_palette(draw_buf, 1, lv_color_to_32(qrcode->dark_color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);

    if(data_len > qrcodegen_BUFFER_LEN_MAX) return LV_RESULT_INVALID;

    int32_t qr_version = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, data_len);
    int32_t quiet_zone_scale = 0;
    if(qrcode->quiet_zone) qr_version = get_satisfied_size(qr_version, draw_buf->header.w, &quiet_zone_scale);
    if(qr_version <= 0) return LV_RESULT_INVALID;

    const int32_t qr_size = qrcodegen_version2size(qr_version);
    if(qr_size <= 0) return LV_RESULT_INVALID;

    /*Integer number of canvas pixels per QR module. Validate both branches the same
     *way: a scale of zero means the canvas is too small to hold even a 1:1 copy of
     *the code, and drawing it would leave the bitmap blank. Reporting success for
     *that would be indistinguishable from a QR code that was actually rendered.*/
    const int32_t scale = qrcode->quiet_zone ? quiet_zone_scale : draw_buf->header.w / qr_size;
    if(scale <= 0) {
        LV_LOG_ERROR("QR code size %d does not fit the %d px canvas%s",
                     (int)qr_size, (int)draw_buf->header.w,
                     qrcode->quiet_zone ? " with a quiet zone" : "");
        return LV_RESULT_INVALID;
    }

    uint8_t * qr0 = lv_malloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_MALLOC(qr0);
    uint8_t * data_tmp = lv_malloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_MALLOC(data_tmp);

    if(qr0 == NULL || data_tmp == NULL) {
        LV_LOG_ERROR("malloc failed for the QR code encoder buffers");
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

    qrcode->render_failed = false;
    return LV_RESULT_OK;
}

#endif /*LV_USE_QRCODE*/
