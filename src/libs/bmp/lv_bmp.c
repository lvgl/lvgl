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
static void set_caps(lv_fs_file_t * f, uint8_t * caps);
static lv_res_t decoder_accept(const lv_img_src_t * src, uint8_t * caps, void * user_data);
static lv_res_t decoder_open(lv_img_dec_dsc_t * dsc, const lv_img_dec_flags_t flags, void * user_data);


static lv_res_t decoder_read_line(lv_img_dec_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf, void * user_data);

static void decoder_close(lv_img_dec_dsc_t * dsc, void * user_data);

static lv_res_t check_colordepth(int bpp);

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
    lv_img_decoder_t * dec = lv_img_decoder_create(NULL);
    lv_img_decoder_set_accept_cb(dec, decoder_accept);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_read_line_cb(dec, decoder_read_line);
    lv_img_decoder_set_close_cb(dec, decoder_close);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void set_caps(lv_fs_file_t * f, uint8_t * caps)
{
#if LV_COLOR_DEPTH == 32
    lv_fs_seek(f, 28, LV_FS_SEEK_SET);
    uint16_t bpp = 0;
    lv_fs_read(f, &bpp, sizeof(bpp), NULL);
    *caps = bpp == 32 ? LV_IMG_DEC_TRANSPARENT : LV_IMG_DEC_DEFAULT;
#else
    LV_UNUSED(f);
    *caps = LV_IMG_DEC_DEFAULT;
#endif
}

/**
 * Get info about a PNG image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_accept(const lv_img_src_t * src, uint8_t * caps, void * user_data)
{
    LV_UNUSED(user_data);

    /*If it's a BMP file...*/
    if(src->type == LV_IMG_SRC_FILE) {
        /*Support only "*.bin" files*/
        if(!src->ext || strcmp(src->ext, "bmp")) return LV_RES_INV;

        /*Check file exists*/
        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, src->data, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) return LV_RES_INV;
        set_caps(&f, caps);
        lv_fs_close(&f);

        return LV_RES_OK;
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    return LV_RES_INV;
}


static lv_res_t check_colordepth(int bpp)
{
    if(LV_COLOR_DEPTH == 32 && bpp != 32 && bpp != 24) {
        LV_LOG_WARN("LV_COLOR_DEPTH == 32 but bpp is %d (should be 32 or 24)", bpp);
        return LV_RES_INV;
    }
    else if(LV_COLOR_DEPTH == 16 && bpp != 16) {
        LV_LOG_WARN("LV_COLOR_DEPTH == 16 but bpp is %d (should be 16)", bpp);
        return LV_RES_INV;
    }
    else if(LV_COLOR_DEPTH == 8 && bpp != 8) {
        LV_LOG_WARN("LV_COLOR_DEPTH == 8 but bpp is %d (should be 8)", bpp);
        return LV_RES_INV;
    }
    return LV_RES_OK;
}

/**
 * Open a BMP image and return the decided image
 * @param src can be file name or pointer to a C array
 * @param style style of the image object (unused now but certain formats might use it)
 * @return pointer to the decoded image or `LV_IMG_DECODER_OPEN_FAIL` if failed
 */
static lv_res_t decoder_open(lv_img_dec_dsc_t * dsc, const lv_img_dec_flags_t flags, void * user_data)
{
    LV_UNUSED(user_data);
    /* Only extract metadata ?*/
    if(flags == LV_IMG_DEC_ONLYMETA) {
        /*Save the data in the header*/
        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, dsc->input.src->data, LV_FS_MODE_RD);
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
        uint16_t bpp;
        memcpy(&bpp, headers + 28, 2);
#if LV_COLOR_DEPTH == 32
        dsc->header.cf = bpp == 32 ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
        dsc->caps = bpp == 32 ? LV_IMG_DEC_TRANSPARENT : LV_IMG_DEC_DEFAULT;
#else
        dsc->header.cf = LV_IMG_CF_TRUE_COLOR;
        dsc->caps = LV_IMG_DEC_DEFAULT;
#endif
        return check_colordepth(bpp);
    }

    /*If it's a BMP file...*/
    if(dsc->input.src->type == LV_IMG_SRC_FILE) {

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, dsc->input.src->data, LV_FS_MODE_RD);
        if(res == LV_RES_OK) return LV_RES_INV;

        uint8_t header[54];
        lv_fs_read(&f, header, 54, NULL);
        uint16_t bpp;
        memcpy(&bpp, header + 28, 2);

        if(0x42 != header[0] || 0x4d != header[1] || check_colordepth(bpp) == LV_RES_INV) {
            lv_fs_close(&f);
            return LV_RES_INV;
        }

        LV_ZERO_ALLOC(dsc->dec_ctx);
        dsc->dec_ctx->auto_allocated = 1;
        if(dsc->dec_ctx->user_data == NULL) {
            LV_ZALLOC(dsc->dec_ctx->user_data, sizeof(bmp_dsc_t));
            if(dsc->dec_ctx->user_data == NULL) {
                LV_LOG_ERROR("img_decoder_built_in_open: out of memory");
                lv_fs_close(&f);
                return LV_RES_INV;
            }

            bmp_dsc_t * b = (bmp_dsc_t *)dsc->dec_ctx->user_data;
            memcpy(&b->px_offset, header + 10, 4);
            memcpy(&b->px_width, header + 18, 4);
            memcpy(&b->px_height, header + 22, 4);
            memcpy(&b->bpp, header + 28, 2);
            b->row_size_bytes = ((b->bpp * b->px_width + 31) / 32) * 4;
            dsc->header.w = b->px_width;
            dsc->header.h = b->px_height;
#if LV_COLOR_DEPTH == 32
            dsc->header.cf = bpp == 32 ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
#else
            dsc->header.cf = LV_IMG_CF_TRUE_COLOR;
#endif
            b->f = f;
        }
        dsc->img_data = NULL;
        return LV_RES_OK;
    }
    return LV_RES_INV;    /*If not returned earlier then it failed*/
}


static lv_res_t decoder_read_line(lv_img_dec_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf, void * user_data)
{
    LV_UNUSED(user_data);
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
static void decoder_close(lv_img_dec_dsc_t * dsc, void * user_data)
{
    LV_UNUSED(user_data);
    if(!dsc || !dsc->dec_ctx) return;

    bmp_dsc_t * b = dsc->dec_ctx->user_data;
    lv_fs_close(&b->f);
    lv_free(dsc->dec_ctx->user_data);
    if(dsc->dec_ctx->auto_allocated) {
        lv_free(dsc->dec_ctx);
        dsc->dec_ctx = NULL;
    }

}

#endif /*LV_USE_BMP*/
