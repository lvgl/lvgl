/**
 * @file lv_bin.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"


/*********************
 *      DEFINES
 *********************/
#define CF_BUILT_IN_FIRST LV_IMG_CF_TRUE_COLOR
#define CF_BUILT_IN_LAST LV_IMG_CF_ALPHA_8BIT

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_fs_file_t f;
    lv_color_t * palette;
    lv_opa_t *   opa;
} lv_img_decoder_built_in_data_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_img_decoder_built_in_line_true_color(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                        lv_coord_t len, uint8_t * buf);
static lv_res_t lv_img_decoder_built_in_line_alpha(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                   lv_coord_t len, uint8_t * buf);
static lv_res_t lv_img_decoder_built_in_line_indexed(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                     lv_coord_t len, uint8_t * buf);

static lv_res_t lv_img_decoder_built_in_accept(const lv_img_src_uri_t * src, uint8_t * caps);

static lv_res_t lv_img_decoder_built_in_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags);

static lv_res_t lv_img_decoder_built_in_read_line(lv_img_decoder_dsc_t * dsc,
                                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void lv_img_decoder_built_in_close(lv_img_decoder_dsc_t * dsc);



void lv_bin_init()
{
    /*Create a decoder for the built in color format*/
    lv_img_decoder_t * decoder = lv_img_decoder_create();
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) {
        LV_LOG_WARN("lv_img_decoder_init: out of memory");
        return;
    }

    lv_img_decoder_set_accept_cb(decoder, lv_img_decoder_built_in_accept);
    lv_img_decoder_set_open_cb(decoder, lv_img_decoder_built_in_open);
    lv_img_decoder_set_read_line_cb(decoder, lv_img_decoder_built_in_read_line);
    lv_img_decoder_set_close_cb(decoder, lv_img_decoder_built_in_close);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t init_dec_ctx(uint8_t * caps)
{
    if(caps != NULL) caps = LV_IMG_DEC_CACHED;
    return LV_RES_OK;
}

lv_res_t lv_img_decoder_built_in_accept(const lv_img_src_uri_t * src, uint8_t * caps)
{
    if(src->type == LV_IMG_SRC_VARIABLE) {
        lv_img_cf_t cf = ((lv_img_dsc_t *)src->uri)->header.cf;
        if(cf < CF_BUILT_IN_FIRST || cf > CF_BUILT_IN_LAST) return LV_RES_INV;
        if(init_dec_ctx(caps) != LV_RES_OK) return LV_RES_INV;
        return LV_RES_OK;
    }
    if(src->type == LV_IMG_SRC_FILE) {
        /*Support only "*.bin" files*/
        if(!src->ext || strcmp(src->ext, ".bin")) return LV_RES_INV;
        if (caps) *caps = LV_IMG_DEC_DEFAULT; /*Image is not cached for files*/

        /*Check file exists*/
        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, src->uri, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) return LV_RES_INV;
        lv_fs_close(&f);

        return LV_RES_OK;
    }

    LV_LOG_WARN("Image get info found unknown src type");
    return LV_RES_INV;
}

lv_res_t lv_img_decoder_built_in_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags)
{
    /* Only extract metadata ?*/
    if (flags == LV_IMG_DEC_ONLYMETA) {
        if (dsc->src->type == LV_IMG_SRC_VARIABLE) {
            lv_img_header_t header = ((lv_img_dsc_t *)dsc->src->uri)->header;

            dsc->header.w  = header.w;
            dsc->header.h  = header.h;
            dsc->header.cf = header.cf;

            return LV_RES_OK;
        }

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, dsc->src->uri, LV_FS_MODE_RD);
        if(res == LV_FS_RES_OK) {
            uint32_t rn;
            res = lv_fs_read(&f, &dsc->header, sizeof(lv_img_header_t), &rn);
            lv_fs_close(&f);
            if(res != LV_FS_RES_OK || rn != sizeof(lv_img_header_t)) {
                LV_LOG_WARN("Image get info get read file header");
                return LV_RES_INV;
            }
        }

        if(dsc->header.cf < CF_BUILT_IN_FIRST || dsc->header.cf > CF_BUILT_IN_LAST) return LV_RES_INV;
        return LV_RES_OK;
    }

    /*Open the file if it's a file*/
    if(dsc->src->type == LV_IMG_SRC_FILE) {

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, dsc->src->uri, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) {
            LV_LOG_WARN("Built-in image decoder can't open the file");
            return LV_RES_INV;
        }

        /*If the file was open successfully save the file descriptor*/
        LV_ZERO_ALLOC(dsc->dec_ctx);

        if(dsc->dec_ctx->user_data == NULL) {
            LV_ZALLOC(dsc->dec_ctx->user_data, sizeof(lv_img_decoder_built_in_data_t));
            if(dsc->dec_ctx->user_data == NULL) {
                LV_LOG_ERROR("img_decoder_built_in_open: out of memory");
                lv_fs_close(&f);
                return LV_RES_INV;
            }
        }

        lv_img_decoder_built_in_data_t * user_data = dsc->dec_ctx->user_data;
        lv_memcpy_small(&user_data->f, &f, sizeof(f));

        if(dsc->dec_ctx != NULL) {
            /*The image is not cached for files, it's read line by line*/
            dsc->dec_ctx->caps = LV_IMG_DEC_DEFAULT;
        }
    }
    else if(dsc->src->type == LV_IMG_SRC_VARIABLE) {
        /*The variables should have valid data*/
        if(((lv_img_dsc_t *)dsc->src->uri)->data == NULL) {
            return LV_RES_INV;
        }
        LV_ZERO_ALLOC(dsc->dec_ctx);
        if(init_dec_ctx(&dsc->dec_ctx->caps) != LV_RES_OK) return LV_RES_INV;
    }

    lv_img_cf_t cf = dsc->header.cf;
    /*Process true color formats*/
    if(cf == LV_IMG_CF_TRUE_COLOR || cf == LV_IMG_CF_TRUE_COLOR_ALPHA || cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        if(dsc->src->type == LV_IMG_SRC_VARIABLE) {
            /*In case of uncompressed formats the image stored in the ROM/RAM.
             *So simply give its pointer*/
            dsc->img_data = ((lv_img_dsc_t *)dsc->src->uri)->data;
            return LV_RES_OK;
        }
        else {
            /*If it's a file it need to be read line by line later*/
            return LV_RES_OK;
        }
    }
    /*Process indexed images. Build a palette*/
    else if(cf == LV_IMG_CF_INDEXED_1BIT || cf == LV_IMG_CF_INDEXED_2BIT || cf == LV_IMG_CF_INDEXED_4BIT ||
            cf == LV_IMG_CF_INDEXED_8BIT) {
        uint8_t px_size       = lv_img_cf_get_px_size(cf);
        uint32_t palette_size = 1 << px_size;

        /*Allocate the palette*/
        if(dsc->dec_ctx->user_data == NULL) {
            LV_ZALLOC(dsc->dec_ctx->user_data, sizeof(lv_img_decoder_built_in_data_t));
            if(dsc->dec_ctx->user_data == NULL) {
                LV_LOG_ERROR("img_decoder_built_in_open: out of memory");
                return LV_RES_INV;
            }
        }

        lv_img_decoder_built_in_data_t * user_data = dsc->dec_ctx->user_data;
        if (user_data->palette == NULL) {
            user_data->palette                         = lv_mem_alloc(palette_size * sizeof(lv_color_t));
            LV_ASSERT_MALLOC(user_data->palette);
            user_data->opa                             = lv_mem_alloc(palette_size * sizeof(lv_opa_t));
            LV_ASSERT_MALLOC(user_data->opa);
            if(user_data->palette == NULL || user_data->opa == NULL) {
                LV_LOG_ERROR("img_decoder_built_in_open: out of memory");
                lv_img_decoder_built_in_close(dsc);
                return LV_RES_INV;
            }

            if(dsc->src->type == LV_IMG_SRC_FILE) {
                /*Read the palette from file*/
                lv_fs_seek(&user_data->f, 4, LV_FS_SEEK_SET); /*Skip the header*/
                lv_color32_t cur_color;
                uint32_t i;
                for(i = 0; i < palette_size; i++) {
                    lv_fs_read(&user_data->f, &cur_color, sizeof(lv_color32_t), NULL);
                    user_data->palette[i] = lv_color_make(cur_color.ch.red, cur_color.ch.green, cur_color.ch.blue);
                    user_data->opa[i]     = cur_color.ch.alpha;
                }
            }
            else {
                /*The palette begins in the beginning of the image data. Just point to it.*/
                lv_color32_t * palette_p = (lv_color32_t *)((lv_img_dsc_t *)dsc->src)->data;

                uint32_t i;
                for(i = 0; i < palette_size; i++) {
                    user_data->palette[i] = lv_color_make(palette_p[i].ch.red, palette_p[i].ch.green, palette_p[i].ch.blue);
                    user_data->opa[i]     = palette_p[i].ch.alpha;
                }
            }
        }
    }
    /*Alpha indexed images.*/
    else if(cf == LV_IMG_CF_ALPHA_1BIT || cf == LV_IMG_CF_ALPHA_2BIT || cf == LV_IMG_CF_ALPHA_4BIT ||
            cf == LV_IMG_CF_ALPHA_8BIT) {
        return LV_RES_OK; /*Nothing to process*/
    }
    /*Unknown format. Can't decode it.*/
    else {
        /*Free the potentially allocated memories*/
        lv_img_decoder_built_in_close(dsc);

        LV_LOG_WARN("Image decoder open: unknown color format");
        return LV_RES_INV;
    }
    return LV_RES_OK;
}

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 * @param x start x coordinate
 * @param y start y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return LV_RES_OK: ok; LV_RES_INV: failed
 */
lv_res_t lv_img_decoder_built_in_read_line(lv_img_decoder_dsc_t * dsc, lv_coord_t x,
                                           lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    lv_res_t res = LV_RES_INV;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR || dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA ||
       dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        /*For TRUE_COLOR images read line required only for files.
         *For variables the image data was returned in `open`*/
        if(dsc->src->type == LV_IMG_SRC_FILE) {
            res = lv_img_decoder_built_in_line_true_color(dsc, x, y, len, buf);
        }
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT || dsc->header.cf == LV_IMG_CF_ALPHA_2BIT ||
            dsc->header.cf == LV_IMG_CF_ALPHA_4BIT || dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
        res = lv_img_decoder_built_in_line_alpha(dsc, x, y, len, buf);
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_1BIT || dsc->header.cf == LV_IMG_CF_INDEXED_2BIT ||
            dsc->header.cf == LV_IMG_CF_INDEXED_4BIT || dsc->header.cf == LV_IMG_CF_INDEXED_8BIT) {
        res = lv_img_decoder_built_in_line_indexed(dsc, x, y, len, buf);
    }
    else {
        LV_LOG_WARN("Built-in image decoder read not supports the color format");
        return LV_RES_INV;
    }

    return res;
}

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
void lv_img_decoder_built_in_close(lv_img_decoder_dsc_t * dsc)
{
    lv_img_decoder_built_in_data_t * user_data = dsc->dec_ctx->user_data;
    if(user_data) {
        if(dsc->src->type == LV_IMG_SRC_FILE) {
            lv_fs_close(&user_data->f);
        }
        if(user_data->palette) lv_mem_free(user_data->palette);
        if(user_data->opa) lv_mem_free(user_data->opa);

        lv_mem_free(user_data);
        dsc->dec_ctx->user_data = NULL;
    }
    if (!dsc->dec_ctx->self_allocated) {
        lv_mem_free(dsc->dec_ctx);
        dsc->dec_ctx = NULL;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t lv_img_decoder_built_in_line_true_color(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                        lv_coord_t len, uint8_t * buf)
{
    lv_img_decoder_built_in_data_t * user_data = dsc->dec_ctx->user_data;
    lv_fs_res_t res;
    uint8_t px_size = lv_img_cf_get_px_size(dsc->header.cf);

    uint32_t pos = ((y * dsc->header.w + x) * px_size) >> 3;
    pos += 4; /*Skip the header*/
    res = lv_fs_seek(&user_data->f, pos, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("Built-in image decoder seek failed");
        return LV_RES_INV;
    }
    uint32_t btr = len * (px_size >> 3);
    uint32_t br  = 0;
    res = lv_fs_read(&user_data->f, buf, btr, &br);
    if(res != LV_FS_RES_OK || btr != br) {
        LV_LOG_WARN("Built-in image decoder read failed");
        return LV_RES_INV;
    }

    return LV_RES_OK;
}

static lv_res_t lv_img_decoder_built_in_line_alpha(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                   lv_coord_t len, uint8_t * buf)
{
    const lv_opa_t alpha1_opa_table[2]  = {0, 255};          /*Opacity mapping with bpp = 1 (Just for compatibility)*/
    const lv_opa_t alpha2_opa_table[4]  = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/
    const lv_opa_t alpha4_opa_table[16] = {0,  17, 34,  51,  /*Opacity mapping with bpp = 4*/
                                           68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255
                                          };

    /*Simply fill the buffer with the color. Later only the alpha value will be modified.*/
    lv_color_t bg_color = dsc->color;
    lv_coord_t i;
    for(i = 0; i < len; i++) {
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full;
#elif LV_COLOR_DEPTH == 16
        /*Because of Alpha byte 16 bit color can start on odd address which can cause crash*/
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full & 0xFF;
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + 1] = (bg_color.full >> 8) & 0xFF;
#elif LV_COLOR_DEPTH == 32
        *((uint32_t *)&buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE]) = bg_color.full;
#else
#error "Invalid LV_COLOR_DEPTH. Check it in lv_conf.h"
#endif
    }

    const lv_opa_t * opa_table = NULL;
    uint8_t px_size            = lv_img_cf_get_px_size(dsc->header.cf);
    uint16_t mask              = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    lv_coord_t w = 0;
    uint32_t ofs = 0;
    int8_t pos   = 0;
    switch(dsc->header.cf) {
        case LV_IMG_CF_ALPHA_1BIT:
            w = (dsc->header.w + 7) >> 3; /*E.g. w = 20 -> w = 2 + 1*/
            ofs += w * y + (x >> 3); /*First pixel*/
            pos = 7 - (x & 0x7);
            opa_table = alpha1_opa_table;
            break;
        case LV_IMG_CF_ALPHA_2BIT:
            w = (dsc->header.w + 3) >> 2; /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            ofs += w * y + (x >> 2); /*First pixel*/
            pos = 6 - (x & 0x3) * 2;
            opa_table = alpha2_opa_table;
            break;
        case LV_IMG_CF_ALPHA_4BIT:
            w = (dsc->header.w + 1) >> 1; /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            ofs += w * y + (x >> 1); /*First pixel*/
            pos = 4 - (x & 0x1) * 4;
            opa_table = alpha4_opa_table;
            break;
        case LV_IMG_CF_ALPHA_8BIT:
            w = dsc->header.w; /*E.g. x = 7 -> w = 7 (bytes)*/
            ofs += w * y + x;  /*First pixel*/
            pos = 0;
            break;
    }

    lv_img_decoder_built_in_data_t * user_data = dsc->dec_ctx->user_data;
    uint8_t * fs_buf = lv_mem_buf_get(w);
    if(fs_buf == NULL) return LV_RES_INV;

    const uint8_t * data_tmp = NULL;
    if(dsc->src->type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = (const lv_img_dsc_t *)dsc->src->uri;

        data_tmp = img_dsc->data + ofs;
    }
    else {
        lv_fs_seek(&user_data->f, ofs + 4, LV_FS_SEEK_SET); /*+4 to skip the header*/
        lv_fs_read(&user_data->f, fs_buf, w, NULL);
        data_tmp = fs_buf;
    }

    for(i = 0; i < len; i++) {
        uint8_t val_act = (*data_tmp >> pos) & mask;

        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + LV_IMG_PX_SIZE_ALPHA_BYTE - 1] =
            dsc->header.cf == LV_IMG_CF_ALPHA_8BIT ? val_act : opa_table[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            data_tmp++;
        }
    }
    lv_mem_buf_release(fs_buf);
    return LV_RES_OK;
}

static lv_res_t lv_img_decoder_built_in_line_indexed(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                     lv_coord_t len, uint8_t * buf)
{
    uint8_t px_size = lv_img_cf_get_px_size(dsc->header.cf);
    uint16_t mask   = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    lv_coord_t w = 0;
    int8_t pos   = 0;
    uint32_t ofs = 0;
    switch(dsc->header.cf) {
        case LV_IMG_CF_INDEXED_1BIT:
            w = (dsc->header.w + 7) >> 3; /*E.g. w = 20 -> w = 2 + 1*/
            ofs += w * y + (x >> 3); /*First pixel*/
            ofs += 8;                /*Skip the palette*/
            pos = 7 - (x & 0x7);
            break;
        case LV_IMG_CF_INDEXED_2BIT:
            w = (dsc->header.w + 3) >> 2; /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            ofs += w * y + (x >> 2); /*First pixel*/
            ofs += 16;               /*Skip the palette*/
            pos = 6 - (x & 0x3) * 2;
            break;
        case LV_IMG_CF_INDEXED_4BIT:
            w = (dsc->header.w + 1) >> 1; /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            ofs += w * y + (x >> 1); /*First pixel*/
            ofs += 64;               /*Skip the palette*/
            pos = 4 - (x & 0x1) * 4;
            break;
        case LV_IMG_CF_INDEXED_8BIT:
            w = dsc->header.w; /*E.g. x = 7 -> w = 7 (bytes)*/
            ofs += w * y + x;  /*First pixel*/
            ofs += 1024;       /*Skip the palette*/
            pos = 0;
            break;
    }

    lv_img_decoder_built_in_data_t * user_data = dsc->dec_ctx->user_data;

    uint8_t * fs_buf = lv_mem_buf_get(w);
    if(fs_buf == NULL) return LV_RES_INV;
    const uint8_t * data_tmp = NULL;
    if(dsc->src->type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = (const lv_img_dsc_t *)dsc->src->uri;
        data_tmp                     = img_dsc->data + ofs;
    }
    else {
        lv_fs_seek(&user_data->f, ofs + 4, LV_FS_SEEK_SET); /*+4 to skip the header*/
        lv_fs_read(&user_data->f, fs_buf, w, NULL);
        data_tmp = fs_buf;
    }

    lv_coord_t i;
    for(i = 0; i < len; i++) {
        uint8_t val_act = (*data_tmp >> pos) & mask;

        lv_color_t color = user_data->palette[val_act];
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = color.full;
#elif LV_COLOR_DEPTH == 16
        /*Because of Alpha byte 16 bit color can start on odd address which can cause crash*/
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = color.full & 0xFF;
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + 1] = (color.full >> 8) & 0xFF;
#elif LV_COLOR_DEPTH == 32
        *((uint32_t *)&buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE]) = color.full;
#else
#error "Invalid LV_COLOR_DEPTH. Check it in lv_conf.h"
#endif
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + LV_IMG_PX_SIZE_ALPHA_BYTE - 1] = user_data->opa[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            data_tmp++;
        }
    }
    lv_mem_buf_release(fs_buf);
    return LV_RES_OK;
}

