/**
 * @file lv_qrcode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_class_private.h"
#include "lv_qrcode_private.h"

#if LV_USE_QRCODE

#include "qrcodegen.h"

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

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_qrcode_class = {
    .constructor_cb = lv_qrcode_constructor,
    .destructor_cb = lv_qrcode_destructor,
    .instance_size = sizeof(lv_qrcode_t),
    .base_class = &lv_canvas_class,
    .name = "qrcode",
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
    LV_ASSERT_OBJ(obj, MY_CLASS);

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
}

void lv_qrcode_set_dark_color(lv_obj_t * obj, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    qrcode->dark_color = color;
}

void lv_qrcode_set_light_color(lv_obj_t * obj, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;
    qrcode->light_color = color;
}

lv_result_t lv_qrcode_update(lv_obj_t * obj, const void * data, uint32_t data_len)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_qrcode_t * qrcode = (lv_qrcode_t *)obj;

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) {
        LV_LOG_ERROR("canvas draw buffer is NULL");
        return LV_RESULT_INVALID;
    }

    lv_draw_buf_clear(draw_buf, NULL);
    lv_canvas_set_palette(obj, 0, lv_color_to_32(qrcode->light_color, LV_OPA_COVER));
    lv_canvas_set_palette(obj, 1, lv_color_to_32(qrcode->dark_color, LV_OPA_COVER));
    lv_image_cache_drop(draw_buf);

    lv_obj_invalidate(obj);

    if(data_len > qrcodegen_BUFFER_LEN_MAX) return LV_RESULT_INVALID;

    int32_t qr_version = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, data_len);
    if(qr_version <= 0) return LV_RESULT_INVALID;
    int32_t qr_size = qrcodegen_version2size(qr_version);
    if(qr_size <= 0) return LV_RESULT_INVALID;
    int32_t scale = draw_buf->header.w / qr_size;
    if(scale <= 0) return LV_RESULT_INVALID;

    /* Pick the largest QR code that still maintains scale. */
    for(int32_t i = qr_version + 1; i < qrcodegen_VERSION_MAX; i++) {
        if(qrcodegen_version2size(i) * scale > draw_buf->header.w)
            break;

        qr_version = i;
    }
    qr_size = qrcodegen_version2size(qr_version);

    uint8_t * qr0 = lv_malloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_MALLOC(qr0);
    uint8_t * data_tmp = lv_malloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_MALLOC(data_tmp);
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
    qr_size = qrcodegen_getSize(qr0);
    scale = obj_w / qr_size;
    int scaled = qr_size * scale;
    int margin = (obj_w - scaled) / 2;
    uint8_t * buf_u8 = (uint8_t *)draw_buf->data + 8;    /*+8 skip the palette*/
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

    /* invalidate the canvas to refresh it */
    lv_display_enable_invalidation(lv_obj_get_display(obj), true);

    lv_free(qr0);
    lv_free(data_tmp);
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_qrcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    /*Set default size*/
    lv_qrcode_set_size(obj, LV_DPI_DEF);

    /*Set default color*/
    lv_qrcode_set_dark_color(obj, lv_color_black());
    lv_qrcode_set_light_color(obj, lv_color_white());
}

static void lv_qrcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_image_cache_drop(draw_buf);

    /*@fixme destroy buffer in cache free_cb.*/
    lv_draw_buf_destroy(draw_buf);
}

#endif /*LV_USE_QRCODE*/
