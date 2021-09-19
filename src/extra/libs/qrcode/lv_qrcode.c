/**
 * @file lv_qrcode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_qrcode.h"
#include "qrcodegen.h"

/*********************
 *      DEFINES
 *********************/
#define QR_SIZE     140

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

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
   uint32_t buf_size = LV_CANVAS_BUF_SIZE_INDEXED_1BIT(size, size);
   uint8_t * buf = lv_mem_alloc(buf_size);
   LV_ASSERT_MALLOC(buf);
   if(buf == NULL) return NULL;

   lv_obj_t * canvas = lv_canvas_create(parent);
   if(canvas == NULL) return NULL;

   lv_canvas_set_buffer(canvas, buf, size, size, LV_IMG_CF_INDEXED_1BIT);
   lv_canvas_set_palette(canvas, 0, dark_color);
   lv_canvas_set_palette(canvas, 1, light_color);

   return canvas;

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

    uint8_t qr0[qrcodegen_BUFFER_LEN_MAX];
    uint8_t data_tmp[qrcodegen_BUFFER_LEN_MAX];
    memcpy(data_tmp, data, data_len);

    bool ok = qrcodegen_encodeBinary(data_tmp, data_len,
            qr0, qrcodegen_Ecc_MEDIUM,
            qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
            qrcodegen_Mask_AUTO, true);

    if (!ok) return LV_RES_INV;


    lv_img_dsc_t * imgdsc = lv_canvas_get_img(qrcode);
    lv_coord_t obj_w = imgdsc->header.w;
    int qr_size = qrcodegen_getSize(qr0);
    int scale = obj_w / qr_size;
    int scaled = qr_size * scale;
    int margin = (obj_w - scaled) / 2;
    uint8_t * buf_u8 = (uint8_t *)imgdsc->data + 8;    /*+8 skip the palette*/

    /* Copy the qr code canvas:
     * A simple `lv_canvas_set_px` would work but it's slow for so many pixels.
     * So buffer 1 byte (8 px) from the qr code and set it in the canvas image */
    uint32_t row_byte_cnt = (imgdsc->header.w + 7) >> 3;
    int y;
    for (y = margin; y < scaled + margin; y+=scale) {
        uint8_t b = 0;
        uint8_t p = 0;
        bool aligned = false;
        int x;
        for (x = margin; x < scaled + margin; x++) {
            bool a = qrcodegen_getModule(qr0, (x - margin) / scale, (y - margin) / scale);

            if(aligned == false && (x & 0x7) == 0) aligned = true;

            if(aligned == false) {
                c.full = a ? 0 : 1;
                lv_canvas_set_px(qrcode, x, y, c);
            } else {
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
          memcpy((uint8_t*)buf_u8 + row_byte_cnt * (y + s), row_ori, row_byte_cnt);
      }
    }

    return LV_RES_OK;
}

/**
 * Delete a QR code object
 * @param qrcode pointer to a QR code obejct
 */
void lv_qrcode_delete(lv_obj_t * qrcode)
{
    lv_img_dsc_t * img = lv_canvas_get_img(qrcode);
    lv_img_cache_invalidate_src(img);
    lv_mem_free(img->data);
    lv_obj_del(qrcode);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

