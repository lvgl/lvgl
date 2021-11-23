/**
 * @file lv_qrcode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_qrcode.h"
#if LV_USE_QRCODE

#include "qrcodegen.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_qrcode_class

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
    .base_class = &lv_canvas_class
};

static lv_coord_t size_param;
static lv_color_t dark_color_param;
static lv_color_t light_color_param;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create an empty QR code (an `lv_canvas`) object.
 * @param parent point to an object where to create the QR code
 * @param size width and height of the QR code
 * @param dark_color dark color of the QR code
 * @param light_color light color of the QR code
 * @return pointer to the created QR code object
 */
lv_obj_t * lv_qrcode_create(lv_obj_t * parent, lv_coord_t size, lv_color_t dark_color, lv_color_t light_color)
{
    LV_LOG_INFO("begin");
    size_param = size;
    light_color_param = light_color;
    dark_color_param = dark_color;

    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;

}

/**
 * Set the data of a QR code object
 * @param qrcode pointer to aQ code object
 * @param data data to display
 * @param data_len length of data in bytes
 * @return LV_RES_OK: if no error; LV_RES_INV: on error
 */
lv_res_t lv_qrcode_update(lv_obj_t * qrcode, const void * data, uint32_t data_len)
{
    lv_color_t c;
    c.full = 1;
    lv_canvas_fill_bg(qrcode, c, LV_OPA_COVER);

    if(data_len > qrcodegen_BUFFER_LEN_MAX) return LV_RES_INV;

    lv_img_dsc_t * imgdsc = lv_canvas_get_img(qrcode);

    int32_t qr_version = qrcodegen_getMinFitVersion(qrcodegen_Ecc_MEDIUM, data_len);
    if(qr_version <= 0) return LV_RES_INV;
    int32_t qr_size = qrcodegen_version2size(qr_version);
    if(qr_size <= 0) return LV_RES_INV;
    int32_t scale = imgdsc->header.w / qr_size;
    if(scale <= 0) return LV_RES_INV;
    int32_t remain = imgdsc->header.w % qr_size;

    /* The qr version is incremented by four point */
    uint32_t version_extend = remain / (scale << 2);
    if(version_extend && qr_version < qrcodegen_VERSION_MAX) {
        qr_version = qr_version + version_extend > qrcodegen_VERSION_MAX ?
                     qrcodegen_VERSION_MAX : qr_version + version_extend;
    }

    uint8_t * qr0 = lv_mem_alloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_MALLOC(qr0);
    uint8_t * data_tmp = lv_mem_alloc(qrcodegen_BUFFER_LEN_FOR_VERSION(qr_version));
    LV_ASSERT_MALLOC(data_tmp);
    lv_memcpy(data_tmp, data, data_len);

    bool ok = qrcodegen_encodeBinary(data_tmp, data_len,
                                     qr0, qrcodegen_Ecc_MEDIUM,
                                     qr_version, qr_version,
                                     qrcodegen_Mask_AUTO, true);

    if(!ok) {
        lv_mem_free(qr0);
        lv_mem_free(data_tmp);
        return LV_RES_INV;
    }

    lv_coord_t obj_w = imgdsc->header.w;
    qr_size = qrcodegen_getSize(qr0);
    scale = obj_w / qr_size;
    int scaled = qr_size * scale;
    int margin = (obj_w - scaled) / 2;
    uint8_t * buf_u8 = (uint8_t *)imgdsc->data + 8;    /*+8 skip the palette*/

    /* Copy the qr code canvas:
     * A simple `lv_canvas_set_px` would work but it's slow for so many pixels.
     * So buffer 1 byte (8 px) from the qr code and set it in the canvas image */
    uint32_t row_byte_cnt = (imgdsc->header.w + 7) >> 3;
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
                c.full = a ? 0 : 1;
                lv_canvas_set_px_color(qrcode, x, y, c);
            }
            else {
                if(!a) b |= (1 << (7 - p));
                p++;
                if(p == 8) {
                    uint32_t px = row_byte_cnt * y + (x >> 3);
                    buf_u8[px] = b;
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
            buf_u8[px] = b;
        }

        /*The Qr is probably scaled so simply to the repeated rows*/
        int s;
        const uint8_t * row_ori = buf_u8 + row_byte_cnt * y;
        for(s = 1; s < scale; s++) {
            lv_memcpy((uint8_t *)buf_u8 + row_byte_cnt * (y + s), row_ori, row_byte_cnt);
        }
    }

    lv_mem_free(qr0);
    lv_mem_free(data_tmp);
    return LV_RES_OK;
}


void lv_qrcode_delete(lv_obj_t * qrcode)
{
    lv_obj_del(qrcode);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_qrcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    uint32_t buf_size = LV_CANVAS_BUF_SIZE_INDEXED_1BIT(size_param, size_param);
    uint8_t * buf = lv_mem_alloc(buf_size);
    LV_ASSERT_MALLOC(buf);
    if(buf == NULL) return;

    lv_canvas_set_buffer(obj, buf, size_param, size_param, LV_IMG_CF_INDEXED_1BIT);
    lv_canvas_set_palette(obj, 0, dark_color_param);
    lv_canvas_set_palette(obj, 1, light_color_param);
}

static void lv_qrcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_img_dsc_t * img = lv_canvas_get_img(obj);
    lv_img_cache_invalidate_src(img);
    lv_mem_free((void *)img->data);
    img->data = NULL;
}

#endif /*LV_USE_QRCODE*/
