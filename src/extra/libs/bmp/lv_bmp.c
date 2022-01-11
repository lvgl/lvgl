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
static lv_res_t decoder_accept(const lv_img_src_uri_t * src, uint8_t * caps);
static lv_res_t decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags);


static lv_res_t decoder_read_line(lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void decoder_close(lv_img_decoder_dsc_t * dsc);

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
    lv_img_decoder_set_accept_cb(dec, decoder_accept);
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
static lv_res_t decoder_accept(const lv_img_src_uri_t * src, uint8_t * caps)
{
    /*If it's a BMP file...*/
    if(src->type == LV_IMG_SRC_FILE) {
        /*Support only "*.bin" files*/
        if(!src->ext || strcmp(src->ext, ".bmp")) return LV_RES_INV;
        if(caps) *caps = LV_IMG_DEC_DEFAULT;  /*Image is not cached for files*/

        /*Check file exists*/
        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, src->uri, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) return LV_RES_INV;
        lv_fs_close(&f);

        return LV_RES_OK;
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    return LV_RES_INV;
}


/**
 * Open a BMP image and return the decided image
 * @param src can be file name or pointer to a C array
 * @param style style of the image object (unused now but certain formats might use it)
 * @return pointer to the decoded image or  `LV_IMG_DECODER_OPEN_FAIL` if failed
 */
static lv_res_t decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags)
{
    /* Only extract metadata ?*/
    if(flags == LV_IMG_DEC_ONLYMETA) {
        /*Save the data in the header*/
        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, dsc->src->uri, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) return LV_RES_INV;
        uint8_t headers[54];

        lv_fs_read(&f, headers, 54, NULL);
        uint32_t w;
        uint32_t h;
        memcpy(&w, headers + 18, 4);
        memcpy(&h, headers + 22, 4);
        dsc->header.w = w;
        dsc->header.h = h;
        dsc->header.always_zero = 0;
        lv_fs_close(&f);
#if LV_COLOR_DEPTH == 32
        uint16_t bpp;
        memcpy(&bpp, headers + 28, 2);
        dsc->header.cf = bpp == 32 ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
#else
        dsc->header.cf = LV_IMG_CF_TRUE_COLOR;
#endif
        return LV_RES_OK;
    }

    /*If it's a BMP file...*/
    if(dsc->src_type == LV_IMG_SRC_FILE) {

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, dsc->src->uri, LV_FS_MODE_RD);
        if(res == LV_RES_OK) return LV_RES_INV;

        uint8_t header[54];
        lv_fs_read(&f, header, 54, NULL);

        if(0x42 != header[0] || 0x4d != header[1]) {
            return LV_RES_INV;
        }

        LV_ZERO_ALLOC(dsc->dec_ctx);
        if(dsc->dec_ctx->user_data == NULL) {
            LV_ZALLOC(dsc->dec_ctx->user_data, sizeof(bmp_dsc_t));
            if(dsc->dec_ctx->user_data == NULL) {
                LV_LOG_ERROR("img_decoder_built_in_open: out of memory");
                lv_fs_close(&f);
                return LV_RES_INV;
            }

            bmp_dsc_t * b = (bmp_dsc_t *)dsc->dec_ctx->user_data;
            memcpy(&b.px_offset, header + 10, 4);
            memcpy(&b.px_width, header + 18, 4);
            memcpy(&b.px_height, header + 22, 4);
            memcpy(&b.bpp, header + 28, 2);
            b.row_size_bytes = ((b.bpp * b.px_width + 31) / 32) * 4;
        }
        dsc->img_data = NULL;
        return LV_RES_OK;
    }
    return LV_RES_INV;    /*If not returned earlier then it failed*/
}


static lv_res_t decoder_read_line(lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    bmp_dsc_t * b = dsc->dec_ctx->user_data;
    y = (b->px_height - 1) - y; /*BMP images are stored upside down*/
    uint32_t p = b->px_offset + b->row_size_bytes * y;
    p += x * (b->bpp / 8);
    lv_fs_seek(&b->f, p, LV_FS_SEEK_SET);
    lv_fs_read(&b->f, buf, len * (b->bpp / 8), NULL);

#if LV_COLOR_DEPTH == 32
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
static void decoder_close(lv_img_decoder_dsc_t * dsc)
{
    bmp_dsc_t * b = dsc->dec_ctx->user_data;
    lv_fs_close(&b->f);
    lv_mem_free(dsc->dec_ctx->user_data);
    if(!dsc->dec_ctx->auto_allocated) {
        lv_mem_free(dsc->dec_ctx);
        dsc->dec_ctx = NULL;
    }

}

#endif /*LV_USE_BMP*/
