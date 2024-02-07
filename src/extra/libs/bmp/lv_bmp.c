/**
 * @file lv_bmp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_BMP

#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_fs_file_t f;
    unsigned int px_offset;
    int px_width;
    int px_height;
    unsigned int bpp;
    int row_size_bytes;
} bmp_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header);
static lv_res_t decoder_open(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);

static lv_res_t decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void decoder_close(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_bmp_init(void)
{
    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_read_line_cb(dec, decoder_read_line);
    lv_img_decoder_set_close_cb(dec, decoder_close);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get info about a PNG image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header)
{
    LV_UNUSED(decoder);

    lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/

    /*If it's a BMP file...*/
    if(src_type == LV_IMG_SRC_FILE) {
        const char * fn = src;
        if(strcmp(lv_fs_get_ext(fn), "bmp") == 0) {              /*Check the extension*/
            /*Save the data in the header*/
            lv_fs_file_t f;
            lv_fs_res_t res = lv_fs_open(&f, src, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) return LV_RES_INV;
            uint8_t headers[54];

            lv_fs_read(&f, headers, 54, NULL);
            uint32_t w;
            uint32_t h;
            memcpy(&w, headers + 18, 4);
            memcpy(&h, headers + 22, 4);
            header->w = w;
            header->h = h;
            header->always_zero = 0;
            lv_fs_close(&f);
#if LV_COLOR_DEPTH == 32
            uint16_t bpp;
            memcpy(&bpp, headers + 28, 2);
            header->cf = bpp == 32 ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
#else
            header->cf = LV_IMG_CF_TRUE_COLOR;
#endif
            return LV_RES_OK;
        }
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(src_type == LV_IMG_SRC_VARIABLE) {
        return LV_RES_INV;
    }

    return LV_RES_INV;         /*If didn't succeeded earlier then it's an error*/
}

/**
 * Open a PNG image and return the decided image
 * @param src can be file name or pointer to a C array
 * @param style style of the image object (unused now but certain formats might use it)
 * @return pointer to the decoded image or `LV_IMG_DECODER_OPEN_FAIL` if failed
 */
static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);

    /*If it's a PNG file...*/
    if(dsc->src_type == LV_IMG_SRC_FILE) {
        const char * fn = dsc->src;

        if(strcmp(lv_fs_get_ext(fn), "bmp") != 0) {
            return LV_RES_INV;       /*Check the extension*/
        }

        bmp_dsc_t b;
        memset(&b, 0x00, sizeof(b));

        lv_fs_res_t res = lv_fs_open(&b.f, dsc->src, LV_FS_MODE_RD);
        if(res == LV_RES_OK) return LV_RES_INV;

        uint8_t header[54];
        lv_fs_read(&b.f, header, 54, NULL);

        if(0x42 != header[0] || 0x4d != header[1]) {
            lv_fs_close(&b.f);
            return LV_RES_INV;
        }

        memcpy(&b.px_offset, header + 10, 4);
        memcpy(&b.px_width, header + 18, 4);
        memcpy(&b.px_height, header + 22, 4);
        memcpy(&b.bpp, header + 28, 2);
        b.row_size_bytes = ((b.bpp * b.px_width + 31) / 32) * 4;

        bool color_depth_error = false;
        if(LV_COLOR_DEPTH == 32 && (b.bpp != 32 && b.bpp != 24)) {
            LV_LOG_WARN("LV_COLOR_DEPTH == 32 but bpp is %d (should be 32 or 24)", b.bpp);
            color_depth_error = true;
        }
        else if(LV_COLOR_DEPTH == 16 && b.bpp != 16) {
            LV_LOG_WARN("LV_COLOR_DEPTH == 16 but bpp is %d (should be 16)", b.bpp);
            color_depth_error = true;
        }
        else if(LV_COLOR_DEPTH == 8 && b.bpp != 8) {
            LV_LOG_WARN("LV_COLOR_DEPTH == 8 but bpp is %d (should be 8)", b.bpp);
            color_depth_error = true;
        }

        if(color_depth_error) {
            dsc->error_msg = "Color depth mismatch";
            lv_fs_close(&b.f);
            return LV_RES_INV;
        }

        dsc->user_data = lv_mem_alloc(sizeof(bmp_dsc_t));
        LV_ASSERT_MALLOC(dsc->user_data);
        if(dsc->user_data == NULL) return LV_RES_INV;
        memcpy(dsc->user_data, &b, sizeof(b));

        dsc->img_data = NULL;
        return LV_RES_OK;
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(dsc->src_type == LV_IMG_SRC_VARIABLE) {
        return LV_RES_INV;
    }

    return LV_RES_INV;    /*If not returned earlier then it failed*/
}

static lv_res_t decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    LV_UNUSED(decoder);

    bmp_dsc_t * b = dsc->user_data;
    y = (b->px_height - 1) - y; /*BMP images are stored upside down*/
    uint32_t p = b->px_offset + b->row_size_bytes * y;
    p += x * (b->bpp / 8);
    lv_fs_seek(&b->f, p, LV_FS_SEEK_SET);
    lv_fs_read(&b->f, buf, len * (b->bpp / 8), NULL);

#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 1
    for(unsigned int i = 0; i < len * (b->bpp / 8); i += 2) {
        buf[i] = buf[i] ^ buf[i + 1];
        buf[i + 1] = buf[i] ^ buf[i + 1];
        buf[i] = buf[i] ^ buf[i + 1];
    }

#elif LV_COLOR_DEPTH == 32
    if(b->bpp == 32) {
        lv_coord_t i;
        for(i = 0; i < len; i++) {
            uint8_t b0 = buf[i * 4];
            uint8_t b1 = buf[i * 4 + 1];
            uint8_t b2 = buf[i * 4 + 2];
            uint8_t b3 = buf[i * 4 + 3];
            lv_color32_t * c = (lv_color32_t *)&buf[i * 4];
            c->ch.red = b2;
            c->ch.green = b1;
            c->ch.blue = b0;
            c->ch.alpha = b3;
        }
    }
    if(b->bpp == 24) {
        lv_coord_t i;

        for(i = len - 1; i >= 0; i--) {
            uint8_t * t = &buf[i * 3];
            lv_color32_t * c = (lv_color32_t *)&buf[i * 4];
            c->ch.red = t[2];
            c->ch.green = t[1];
            c->ch.blue = t[0];
            c->ch.alpha = 0xff;
        }
    }
#endif

    return LV_RES_OK;
}

/**
 * Free the allocated resources
 */
static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    bmp_dsc_t * b = dsc->user_data;
    lv_fs_close(&b->f);
    lv_mem_free(dsc->user_data);

}

#endif /*LV_USE_BMP*/
