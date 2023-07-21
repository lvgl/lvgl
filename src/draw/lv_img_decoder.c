/**
 * @file lv_img_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_decoder.h"
#include "../misc/lv_assert.h"
#include "../draw/lv_draw_img.h"
#include "../misc/lv_ll.h"
#include "../misc/lv_gc.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_fs_file_t f;
    lv_color_t * palette;
    lv_opa_t * opa;
} lv_img_decoder_built_in_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decode_indexed_line(lv_color_format_t color_format, const lv_color32_t * palette, lv_coord_t y,
                                    lv_coord_t w_px, const uint8_t * in, lv_color32_t * out);

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
 * Initialize the image decoder module
 */
void _lv_img_decoder_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_img_decoder_ll), sizeof(lv_img_decoder_t));

    lv_img_decoder_t * decoder;

    /*Create a decoder for the built in color format*/
    decoder = lv_img_decoder_create();
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) {
        LV_LOG_WARN("out of memory");
        return;
    }

    lv_img_decoder_set_info_cb(decoder, lv_img_decoder_built_in_info);
    lv_img_decoder_set_open_cb(decoder, lv_img_decoder_built_in_open);
    lv_img_decoder_set_close_cb(decoder, lv_img_decoder_built_in_close);
}

/**
 * Get information about an image.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src the image source. E.g. file name or variable.
 * @param header the image info will be stored here
 * @return LV_RES_OK: success; LV_RES_INV: wasn't able to get info about the image
 */
lv_res_t lv_img_decoder_get_info(const void * src, lv_img_header_t * header)
{
    lv_memzero(header, sizeof(lv_img_header_t));

    if(src == NULL) return LV_RES_INV;

    lv_img_src_t src_type = lv_img_src_get_type(src);
    if(src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RES_INV;
    }

    lv_res_t res = LV_RES_INV;
    lv_img_decoder_t * d;
    _LV_LL_READ(&LV_GC_ROOT(_lv_img_decoder_ll), d) {
        if(d->info_cb) {
            res = d->info_cb(d, src, header);
            if(res == LV_RES_OK) break;
        }
    }

    return res;
}

lv_res_t lv_img_decoder_open(lv_img_decoder_dsc_t * dsc, const void * src, lv_color_t color, int32_t frame_id)
{
    lv_memzero(dsc, sizeof(lv_img_decoder_dsc_t));

    if(src == NULL) return LV_RES_INV;
    lv_img_src_t src_type = lv_img_src_get_type(src);
    if(src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RES_INV;
    }

    dsc->color    = color;
    dsc->src_type = src_type;
    dsc->frame_id = frame_id;

    if(dsc->src_type == LV_IMG_SRC_FILE) {
        size_t fnlen = lv_strlen(src);
        dsc->src = lv_malloc(fnlen + 1);
        LV_ASSERT_MALLOC(dsc->src);
        if(dsc->src == NULL) {
            LV_LOG_WARN("out of memory");
            return LV_RES_INV;
        }
        lv_strcpy((char *)dsc->src, src);
    }
    else {
        dsc->src = src;
    }

    lv_res_t res = LV_RES_INV;

    lv_img_decoder_t * decoder;
    _LV_LL_READ(&LV_GC_ROOT(_lv_img_decoder_ll), decoder) {
        /*Info and Open callbacks are required*/
        if(decoder->info_cb == NULL || decoder->open_cb == NULL) continue;

        res = decoder->info_cb(decoder, src, &dsc->header);
        if(res != LV_RES_OK) continue;

        dsc->decoder = decoder;
        res = decoder->open_cb(decoder, dsc);

        /*Opened successfully. It is a good decoder for this image source*/
        if(res == LV_RES_OK) return res;

        /*Prepare for the next loop*/
        lv_memzero(&dsc->header, sizeof(lv_img_header_t));

        dsc->error_msg = NULL;
        dsc->img_data  = NULL;
        dsc->user_data = NULL;
        dsc->time_to_open = 0;
    }

    if(dsc->src_type == LV_IMG_SRC_FILE)
        lv_free((void *)dsc->src);

    return res;
}

/**
 * Read a line from an opened image
 * @param dsc pointer to `lv_img_decoder_dsc_t` used in `lv_img_decoder_open`
 * @param x start X coordinate (from left)
 * @param y start Y coordinate (from top)
 * @param len number of pixels to read
 * @param buf store the data here
 * @return LV_RES_OK: success; LV_RES_INV: an error occurred
 */
lv_res_t lv_img_decoder_read_line(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    lv_res_t res = LV_RES_INV;
    if(dsc->decoder->read_line_cb) res = dsc->decoder->read_line_cb(dsc->decoder, dsc, x, y, len, buf);

    return res;
}

/**
 * Close a decoding session
 * @param dsc pointer to `lv_img_decoder_dsc_t` used in `lv_img_decoder_open`
 */
void lv_img_decoder_close(lv_img_decoder_dsc_t * dsc)
{
    if(dsc->decoder) {
        if(dsc->decoder->close_cb) dsc->decoder->close_cb(dsc->decoder, dsc);

        if(dsc->src_type == LV_IMG_SRC_FILE) {
            lv_free((void *)dsc->src);
            dsc->src = NULL;
        }
    }
}

/**
 * Create a new image decoder
 * @return pointer to the new image decoder
 */
lv_img_decoder_t * lv_img_decoder_create(void)
{
    lv_img_decoder_t * decoder;
    decoder = _lv_ll_ins_head(&LV_GC_ROOT(_lv_img_decoder_ll));
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) return NULL;

    lv_memzero(decoder, sizeof(lv_img_decoder_t));

    return decoder;
}

/**
 * Delete an image decoder
 * @param decoder pointer to an image decoder
 */
void lv_img_decoder_delete(lv_img_decoder_t * decoder)
{
    _lv_ll_remove(&LV_GC_ROOT(_lv_img_decoder_ll), decoder);
    lv_free(decoder);
}

/**
 * Set a callback to get information about the image
 * @param decoder pointer to an image decoder
 * @param info_cb a function to collect info about an image (fill an `lv_img_header_t` struct)
 */
void lv_img_decoder_set_info_cb(lv_img_decoder_t * decoder, lv_img_decoder_info_f_t info_cb)
{
    decoder->info_cb = info_cb;
}

/**
 * Set a callback to open an image
 * @param decoder pointer to an image decoder
 * @param open_cb a function to open an image
 */
void lv_img_decoder_set_open_cb(lv_img_decoder_t * decoder, lv_img_decoder_open_f_t open_cb)
{
    decoder->open_cb = open_cb;
}

/**
 * Set a callback to a decoded line of an image
 * @param decoder pointer to an image decoder
 * @param read_line_cb a function to read a line of an image
 */
void lv_img_decoder_set_read_line_cb(lv_img_decoder_t * decoder, lv_img_decoder_read_line_f_t read_line_cb)
{
    decoder->read_line_cb = read_line_cb;
}

/**
 * Set a callback to close a decoding session. E.g. close files and free other resources.
 * @param decoder pointer to an image decoder
 * @param close_cb a function to close a decoding session
 */
void lv_img_decoder_set_close_cb(lv_img_decoder_t * decoder, lv_img_decoder_close_f_t close_cb)
{
    decoder->close_cb = close_cb;
}

/**
 * Get info about a built-in image
 * @param decoder the decoder where this function belongs
 * @param src the image source: pointer to an `lv_img_dsc_t` variable, a file path or a symbol
 * @param header store the image data here
 * @return LV_RES_OK: the info is successfully stored in `header`; LV_RES_INV: unknown format or other error.
 */
lv_res_t lv_img_decoder_built_in_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_img_src_t src_type = lv_img_src_get_type(src);
    if(src_type == LV_IMG_SRC_VARIABLE) {
        header->w  = ((lv_img_dsc_t *)src)->header.w;
        header->h  = ((lv_img_dsc_t *)src)->header.h;
        header->cf = ((lv_img_dsc_t *)src)->header.cf;
    }
    else if(src_type == LV_IMG_SRC_FILE) {
        /*Support only "*.bin" files*/
        if(strcmp(lv_fs_get_ext(src), "bin")) return LV_RES_INV;

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, src, LV_FS_MODE_RD);
        if(res == LV_FS_RES_OK) {
            uint32_t rn;
            res = lv_fs_read(&f, header, sizeof(lv_img_header_t), &rn);
            lv_fs_close(&f);
            if(res != LV_FS_RES_OK || rn != sizeof(lv_img_header_t)) {
                LV_LOG_WARN("Image get info get read file header");
                return LV_RES_INV;
            }
        }
    }
    else if(src_type == LV_IMG_SRC_SYMBOL) {
        /*The size depend on the font but it is unknown here. It should be handled outside of the
         *function*/
        header->w = 1;
        header->h = 1;
        /*Symbols always have transparent parts. Important because of cover check in the draw
         *function. The actual value doesn't matter because lv_draw_label will draw it*/
        header->cf = LV_COLOR_FORMAT_A8;
    }
    else {
        LV_LOG_WARN("Image get info found unknown src type");
        return LV_RES_INV;
    }
    return LV_RES_OK;
}

/**
 * Open a built in image
 * @param decoder the decoder where this function belongs
 * @param dsc pointer to decoder descriptor. `src`, `color` are already initialized in it.
 * @return LV_RES_OK: the info is successfully stored in `header`; LV_RES_INV: unknown format or other error.
 */
lv_res_t lv_img_decoder_built_in_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    /*Open the file if it's a file*/
    if(dsc->src_type == LV_IMG_SRC_FILE) {
        /*Support only "*.bin" files*/
        if(strcmp(lv_fs_get_ext(dsc->src), "bin")) return LV_RES_INV;

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, dsc->src, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) {
            LV_LOG_WARN("Built-in image decoder can't open the file");
            return LV_RES_INV;
        }

        /*If the file was open successfully save the file descriptor*/
        if(dsc->user_data == NULL) {
            dsc->user_data = lv_malloc(sizeof(lv_img_decoder_built_in_data_t));
            LV_ASSERT_MALLOC(dsc->user_data);
            if(dsc->user_data == NULL) {
                LV_LOG_ERROR("out of memory");
                lv_fs_close(&f);
                return LV_RES_INV;
            }
            lv_memzero(dsc->user_data, sizeof(lv_img_decoder_built_in_data_t));
        }

        lv_img_decoder_built_in_data_t * user_data = dsc->user_data;
        lv_memcpy(&user_data->f, &f, sizeof(f));
    }
    else if(dsc->src_type == LV_IMG_SRC_VARIABLE) {
        /*The variables should have valid data*/
        if(((lv_img_dsc_t *)dsc->src)->data == NULL) {
            return LV_RES_INV;
        }
    }

    if(dsc->src_type == LV_IMG_SRC_VARIABLE) {
        lv_img_dsc_t * img_dsc = (lv_img_dsc_t *)dsc->src;
        lv_color_format_t cf = img_dsc->header.cf;
        if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
            switch(cf) {
                case LV_COLOR_FORMAT_I1:
                    dsc->palette_size = 2;
                    break;
                case LV_COLOR_FORMAT_I2:
                    dsc->palette_size = 4;
                    break;
                case LV_COLOR_FORMAT_I4:
                    dsc->palette_size = 16;
                    break;
                case LV_COLOR_FORMAT_I8:
                    dsc->palette_size = 256;
                    break;
                default:
                    LV_LOG_WARN("Unexpected color format");
                    return LV_RES_INV;
            }

            dsc->img_data = lv_malloc(sizeof(lv_color32_t) * img_dsc->header.w *  img_dsc->header.h);
            dsc->palette = (const lv_color32_t *)img_dsc->data;
            dsc->header.cf = LV_COLOR_FORMAT_ARGB8888;

            uint32_t y;
            for(y = 0; y < img_dsc->header.h; y++) {
                decode_indexed_line(cf, dsc->palette, y, img_dsc->header.w, img_dsc->data, (lv_color32_t *) dsc->img_data);
            }
        }
        else {
            /*In case of uncompressed formats the image stored in the ROM/RAM.
             *So simply give its pointer*/
            dsc->img_data = ((lv_img_dsc_t *)dsc->src)->data;
        }
        return LV_RES_OK;
    }
    else {
        /*TODO indexed formats needs to be read here*/
        /*If it's a file it need to be read line by line later*/
        return LV_RES_OK;
    }
}

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
void lv_img_decoder_built_in_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/
    lv_img_dsc_t * img_dsc = (lv_img_dsc_t *)dsc->src;
    lv_color_format_t cf = img_dsc->header.cf;
    if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        lv_free((void *)dsc->img_data);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t decode_indexed_line(lv_color_format_t color_format, const lv_color32_t * palette, lv_coord_t y,
                                    lv_coord_t w_px, const uint8_t * in, lv_color32_t * out)
{
    uint8_t px_size;
    uint16_t mask;

    out += w_px * y;

    lv_coord_t w_byte = 0;
    int8_t pos   = 0;
    switch(color_format) {
        case LV_COLOR_FORMAT_I1:
            px_size = 1;
            w_byte = (w_px + 7) >> 3; /*E.g. w = 20 -> w = 2 + 1*/
            in += w_byte * y;            /*First pixel*/
            in += 8;                /*Skip the palette*/
            pos = 7;
            break;
        case LV_COLOR_FORMAT_I2:
            px_size = 2;
            w_byte = (w_px + 3) >> 2; /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            in += w_byte * y; /*First pixel*/
            in += 16;               /*Skip the palette*/
            pos = 6;
            break;
        case LV_COLOR_FORMAT_I4:
            px_size = 4;
            w_byte = (w_px + 1) >> 1; /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            in += w_byte * y; /*First pixel*/
            in += 64;               /*Skip the palette*/
            pos = 4;
            break;
        case LV_COLOR_FORMAT_I8:
            px_size = 8;
            w_byte = w_px;
            in += w_byte * y;  /*First pixel*/
            in += 1024;       /*Skip the palette*/
            pos = 0;
            break;
        default:
            return LV_RES_INV;
    }

    mask   = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    lv_coord_t i;
    for(i = 0; i < w_px; i++) {
        uint8_t val_act = (*in >> pos) & mask;
        out[i] = palette[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            in++;
        }
    }
    return LV_RES_OK;
}
