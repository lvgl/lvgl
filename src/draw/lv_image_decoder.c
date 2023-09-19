/**
 * @file lv_image_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_decoder.h"
#include "../misc/lv_assert.h"
#include "../draw/lv_draw_image.h"
#include "../misc/lv_ll.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define img_decoder_ll_p &(LV_GLOBAL_DEFAULT()->img_decoder_ll)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_fs_file_t f;
    lv_color32_t * palette;
    uint8_t * img_data;
    lv_opa_t * opa;
} lv_image_decoder_built_in_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decode_indexed_line(lv_color_format_t color_format, const lv_color32_t * palette, lv_coord_t x,
                                       lv_coord_t y,
                                       lv_coord_t w_px, const uint8_t * in, lv_color32_t * out);

static uint32_t img_width_to_stride(lv_image_header_t * header);
static lv_fs_res_t fs_read_file_at(lv_fs_file_t * f, uint32_t pos, uint8_t * buff, uint32_t btr, uint32_t * br);

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
void _lv_image_decoder_init(void)
{
    _lv_ll_init(img_decoder_ll_p, sizeof(lv_image_decoder_t));

    lv_image_decoder_t * decoder;

    /*Create a decoder for the built in color format*/
    decoder = lv_image_decoder_create();
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) {
        LV_LOG_WARN("out of memory");
        return;
    }

    lv_image_decoder_set_info_cb(decoder, lv_image_decoder_built_in_info);
    lv_image_decoder_set_open_cb(decoder, lv_image_decoder_built_in_open);
    lv_image_decoder_set_get_area_cb(decoder, lv_image_decoder_built_in_get_area);
    lv_image_decoder_set_close_cb(decoder, lv_image_decoder_built_in_close);
}

/**
 * Get information about an image.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src the image source. E.g. file name or variable.
 * @param header the image info will be stored here
 * @return LV_RESULT_OK: success; LV_RESULT_INVALID: wasn't able to get info about the image
 */
lv_result_t lv_image_decoder_get_info(const void * src, lv_image_header_t * header)
{
    lv_memzero(header, sizeof(lv_image_header_t));

    if(src == NULL) return LV_RESULT_INVALID;

    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RESULT_INVALID;
    }

    lv_result_t res = LV_RESULT_INVALID;
    lv_image_decoder_t * d;
    _LV_LL_READ(img_decoder_ll_p, d) {
        if(d->info_cb) {
            res = d->info_cb(d, src, header);
            if(res == LV_RESULT_OK) {
                if(header->stride == 0) header->stride = img_width_to_stride(header);
                break;
            }
        }
    }

    return res;
}

lv_result_t lv_image_decoder_open(lv_image_decoder_dsc_t * dsc, const void * src, lv_color_t color, int32_t frame_id)
{
    lv_memzero(dsc, sizeof(lv_image_decoder_dsc_t));

    if(src == NULL) return LV_RESULT_INVALID;
    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RESULT_INVALID;
    }

    dsc->color    = color;
    dsc->src_type = src_type;
    dsc->frame_id = frame_id;

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        size_t fnlen = lv_strlen(src);
        dsc->src = lv_malloc(fnlen + 1);
        LV_ASSERT_MALLOC(dsc->src);
        if(dsc->src == NULL) {
            LV_LOG_WARN("out of memory");
            return LV_RESULT_INVALID;
        }
        lv_strcpy((char *)dsc->src, src);
    }
    else {
        dsc->src = src;
    }

    lv_result_t res = LV_RESULT_INVALID;

    lv_image_decoder_t * decoder;
    _LV_LL_READ(img_decoder_ll_p, decoder) {
        /*Info and Open callbacks are required*/
        if(decoder->info_cb == NULL || decoder->open_cb == NULL) continue;

        res = decoder->info_cb(decoder, src, &dsc->header);
        if(res != LV_RESULT_OK) continue;

        if(dsc->header.stride == 0) dsc->header.stride = img_width_to_stride(&dsc->header);

        dsc->decoder = decoder;
        res = decoder->open_cb(decoder, dsc);

        /*Opened successfully. It is a good decoder for this image source*/
        if(res == LV_RESULT_OK) return res;

        /*Prepare for the next loop*/
        lv_memzero(&dsc->header, sizeof(lv_image_header_t));

        dsc->error_msg = NULL;
        dsc->img_data  = NULL;
        dsc->user_data = NULL;
        dsc->time_to_open = 0;
    }

    if(dsc->src_type == LV_IMAGE_SRC_FILE)
        lv_free((void *)dsc->src);

    return res;
}

/**
 * Read a line from an opened image
 * @param dsc pointer to `lv_image_decoder_dsc_t` used in `lv_image_decoder_open`
 * @param x start X coordinate (from left)
 * @param y start Y coordinate (from top)
 * @param len number of pixels to read
 * @param buf store the data here
 * @return LV_RESULT_OK: success; LV_RESULT_INVALID: an error occurred
 */
lv_result_t lv_image_decoder_get_area(lv_image_decoder_dsc_t * dsc, const lv_area_t * full_area,
                                      lv_area_t * decoded_area)
{
    lv_result_t res = LV_RESULT_INVALID;
    if(dsc->decoder->get_area_cb) res = dsc->decoder->get_area_cb(dsc->decoder, dsc, full_area, decoded_area);

    return res;
}

/**
 * Close a decoding session
 * @param dsc pointer to `lv_image_decoder_dsc_t` used in `lv_image_decoder_open`
 */
void lv_image_decoder_close(lv_image_decoder_dsc_t * dsc)
{
    if(dsc->decoder) {
        if(dsc->decoder->close_cb) dsc->decoder->close_cb(dsc->decoder, dsc);

        if(dsc->src_type == LV_IMAGE_SRC_FILE) {
            lv_free((void *)dsc->src);
            dsc->src = NULL;
        }
    }
}

/**
 * Create a new image decoder
 * @return pointer to the new image decoder
 */
lv_image_decoder_t * lv_image_decoder_create(void)
{
    lv_image_decoder_t * decoder;
    decoder = _lv_ll_ins_head(img_decoder_ll_p);
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) return NULL;

    lv_memzero(decoder, sizeof(lv_image_decoder_t));

    return decoder;
}

/**
 * Delete an image decoder
 * @param decoder pointer to an image decoder
 */
void lv_image_decoder_delete(lv_image_decoder_t * decoder)
{
    _lv_ll_remove(img_decoder_ll_p, decoder);
    lv_free(decoder);
}

/**
 * Set a callback to get information about the image
 * @param decoder pointer to an image decoder
 * @param info_cb a function to collect info about an image (fill an `lv_image_header_t` struct)
 */
void lv_image_decoder_set_info_cb(lv_image_decoder_t * decoder, lv_image_decoder_info_f_t info_cb)
{
    decoder->info_cb = info_cb;
}

/**
 * Set a callback to open an image
 * @param decoder pointer to an image decoder
 * @param open_cb a function to open an image
 */
void lv_image_decoder_set_open_cb(lv_image_decoder_t * decoder, lv_image_decoder_open_f_t open_cb)
{
    decoder->open_cb = open_cb;
}

/**
 * Set a callback to a decoded line of an image
 * @param decoder pointer to an image decoder
 * @param read_line_cb a function to read a line of an image
 */
void lv_image_decoder_set_get_area_cb(lv_image_decoder_t * decoder, lv_image_decoder_get_area_cb_t read_line_cb)
{
    decoder->get_area_cb = read_line_cb;
}

/**
 * Set a callback to close a decoding session. E.g. close files and free other resources.
 * @param decoder pointer to an image decoder
 * @param close_cb a function to close a decoding session
 */
void lv_image_decoder_set_close_cb(lv_image_decoder_t * decoder, lv_image_decoder_close_f_t close_cb)
{
    decoder->close_cb = close_cb;
}

/**
 * Get info about a built-in image
 * @param decoder the decoder where this function belongs
 * @param src the image source: pointer to an `lv_image_dsc_t` variable, a file path or a symbol
 * @param header store the image data here
 * @return LV_RESULT_OK: the info is successfully stored in `header`; LV_RESULT_INVALID: unknown format or other error.
 */
lv_result_t lv_image_decoder_built_in_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        header->w  = ((lv_image_dsc_t *)src)->header.w;
        header->h  = ((lv_image_dsc_t *)src)->header.h;
        header->cf = ((lv_image_dsc_t *)src)->header.cf;
        header->stride = ((lv_image_dsc_t *)src)->header.stride;
    }
    else if(src_type == LV_IMAGE_SRC_FILE) {
        /*Support only "*.bin" files*/
        if(strcmp(lv_fs_get_ext(src), "bin")) return LV_RESULT_INVALID;

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, src, LV_FS_MODE_RD);
        if(res == LV_FS_RES_OK) {
            uint32_t rn;
            res = lv_fs_read(&f, header, sizeof(lv_image_header_t), &rn);
            lv_fs_close(&f);
            if(res != LV_FS_RES_OK || rn != sizeof(lv_image_header_t)) {
                LV_LOG_WARN("Image get info get read file header");
                return LV_RESULT_INVALID;
            }
        }
    }
    else if(src_type == LV_IMAGE_SRC_SYMBOL) {
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
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

static lv_image_decoder_built_in_data_t * get_decoder_data(lv_image_decoder_dsc_t * dsc)
{
    lv_image_decoder_built_in_data_t * data = dsc->user_data;
    if(data == NULL) {
        data = lv_malloc(sizeof(lv_image_decoder_built_in_data_t));
        LV_ASSERT_MALLOC(data);
        if(data == NULL) {
            LV_LOG_ERROR("out of memory");
            return NULL;
        }

        lv_memzero(data, sizeof(lv_image_decoder_built_in_data_t));
        dsc->user_data = data;
    }

    return data;
}

/**
 * Open a built in image
 * @param decoder the decoder where this function belongs
 * @param dsc pointer to decoder descriptor. `src`, `color` are already initialized in it.
 * @return LV_RESULT_OK: the info is successfully stored in `header`; LV_RESULT_INVALID: unknown format or other error.
 */
lv_result_t lv_image_decoder_built_in_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    /*Open the file if it's a file*/
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        /*Support only "*.bin" files*/
        if(strcmp(lv_fs_get_ext(dsc->src), "bin")) return LV_RESULT_INVALID;

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, dsc->src, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) {
            LV_LOG_WARN("Built-in image decoder can't open the file");
            return LV_RESULT_INVALID;
        }

        /*If the file was open successfully save the file descriptor*/
        lv_image_decoder_built_in_data_t * decoder_data = get_decoder_data(dsc);
        if(decoder_data == NULL) {
            return LV_RESULT_INVALID;
        }

        lv_memcpy(&decoder_data->f, &f, sizeof(f));
        dsc->user_data = decoder_data;

        /*read palette for indexed image*/
        if(LV_COLOR_FORMAT_IS_INDEXED(dsc->header.cf)) {
            uint32_t size = LV_COLOR_INDEXED_PALETTE_SIZE(dsc->header.cf);
            lv_color32_t * palette = lv_malloc(sizeof(lv_color32_t) * size);
            LV_ASSERT_MALLOC(palette);
            if(palette == NULL) {
                LV_LOG_ERROR("out of memory");
                lv_fs_close(&f);
                return LV_RESULT_INVALID;
            }

            uint32_t rn;
            res = fs_read_file_at(&f, sizeof(lv_image_header_t), (uint8_t *)palette, sizeof(lv_color32_t) * size, &rn);
            if(res != LV_FS_RES_OK || rn != sizeof(lv_color32_t) * size) {
                LV_LOG_WARN("Built-in image decoder can't read the palette");
                lv_free(palette);
                lv_fs_close(&f);
                return LV_RESULT_INVALID;
            }

            dsc->palette = palette;
            dsc->palette_size = size;

            decoder_data->palette = palette; /*Free decoder data on close*/

            /*It needs to be read by get_area_cb later*/
            return LV_RESULT_OK;
        }

        if(dsc->header.cf == LV_COLOR_FORMAT_A8) {
            /*For A8, we read directly to RAM since it takes much less memory than ARGB*/
            uint32_t len = (uint32_t)dsc->header.w * dsc->header.h * 1;
            uint8_t * data = lv_malloc(len);
            LV_ASSERT_MALLOC(data);
            if(data == NULL) {
                LV_LOG_ERROR("out of memory");
                lv_fs_close(&f);
                return LV_RESULT_INVALID;
            }

            uint32_t rn;
            res = fs_read_file_at(&f, sizeof(lv_image_header_t), data, len, &rn);
            if(res != LV_FS_RES_OK || rn != len) {
                LV_LOG_WARN("Built-in image decoder can't read the palette");
                lv_free(data);
                lv_fs_close(&f);
                return LV_RESULT_INVALID;
            }

            decoder_data->img_data = data;
            dsc->img_data = data;
            return LV_RESULT_OK;
        }

        /*It needs to be read by get_area_cb later*/
        return LV_RESULT_OK;
    }

    if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        /*The variables should have valid data*/
        lv_image_dsc_t * img_dsc = (lv_image_dsc_t *)dsc->src;
        if(img_dsc->data == NULL) {
            return LV_RESULT_INVALID;
        }

        lv_color_format_t cf = img_dsc->header.cf;
        if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
            /*Need decoder data to store converted image*/
            lv_image_decoder_built_in_data_t * decoder_data = get_decoder_data(dsc);
            if(decoder_data == NULL) {
                return LV_RESULT_INVALID;
            }

            uint8_t * img_data = lv_malloc(sizeof(lv_color32_t) * img_dsc->header.w *  img_dsc->header.h);
            LV_ASSERT_NULL(img_data);
            if(img_data == NULL) {
                return LV_RESULT_INVALID;
            }
            decoder_data->img_data = img_data; /*Put to decoder data for later free*/

            /*Assemble the decoded image dsc*/
            uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);
            dsc->palette_size = palette_size;
            dsc->palette = (const lv_color32_t *)img_dsc->data;
            dsc->img_data = img_data; /*Return decoded image data.*/
            dsc->header.cf = LV_COLOR_FORMAT_ARGB8888;
            dsc->header.stride = dsc->header.w * 4;

            uint32_t y;
            lv_color32_t * out = (lv_color32_t *) dsc->img_data;
            const uint8_t * in = img_dsc->data;
            in += palette_size * 4;
            for(y = 0; y < img_dsc->header.h; y++) {
                decode_indexed_line(cf, dsc->palette, 0, y, img_dsc->header.w, in, out);
            }
        }
        else {
            /*In case of uncompressed formats the image stored in the ROM/RAM.
             *So simply give its pointer*/
            dsc->img_data = ((lv_image_dsc_t *)dsc->src)->data;
        }

        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;
}

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
void lv_image_decoder_built_in_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/
    lv_image_decoder_built_in_data_t * decoder_data = dsc->user_data;
    if(decoder_data) {
        if(dsc->src_type == LV_IMAGE_SRC_FILE) {
            lv_fs_close(&decoder_data->f);
        }

        lv_free(decoder_data->img_data);
        lv_free(decoder_data->palette);
        lv_free(decoder_data);
    }
}

lv_result_t lv_image_decoder_built_in_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                               const lv_area_t * full_area, lv_area_t * decoded_area)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_color_format_t cf = dsc->header.cf;
    /*Check if cf is supported*/

    bool supported = LV_COLOR_FORMAT_IS_INDEXED(cf)
                     || cf == LV_COLOR_FORMAT_ARGB8888 || cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_RGB888
                     || cf == LV_COLOR_FORMAT_RGB565 || cf == LV_COLOR_FORMAT_RGB565A8;
    if(!supported) {
        LV_LOG_WARN("CF: %d is not supported", cf);
        return LV_RESULT_INVALID;
    }

    lv_result_t res = LV_RESULT_INVALID;
    lv_image_decoder_built_in_data_t * decoder_data = dsc->user_data;
    lv_fs_file_t * f = &decoder_data->f;
    uint32_t bpp = lv_color_format_get_bpp(cf);
    lv_coord_t w_px = lv_area_get_width(full_area);
    uint8_t * img_data = NULL;
    uint32_t offset = sizeof(lv_image_header_t); /*All image starts with image header*/

    /*We only support read line by line for now*/
    if(decoded_area->y1 == LV_COORD_MIN) {
        /*Indexed image is converted to ARGB888*/
        uint32_t len = LV_COLOR_FORMAT_IS_INDEXED(cf) ? sizeof(lv_color32_t) * 8 : bpp;
        len = (len * w_px) / 8;
        img_data = lv_malloc(len);
        LV_ASSERT_NULL(img_data);
        if(img_data == NULL)
            return LV_RESULT_INVALID;

        *decoded_area = *full_area;
        decoded_area->y2 = decoded_area->y1;
        decoder_data->img_data = img_data; /*Free on decoder close*/
    }
    else {
        decoded_area->y1++;
        decoded_area->y2++;
        img_data = decoder_data->img_data;
    }

    if(decoded_area->y1 > full_area->y2) {
        return LV_RESULT_INVALID;
    }

    if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        lv_coord_t x_fraction = decoded_area->x1 % (8 / bpp);
        uint32_t len = (w_px * bpp + 7) / 8 + 1; /*10px for 1bpp may across 3bytes*/
        uint8_t * buf = lv_malloc(len);
        LV_ASSERT_NULL(buf);
        if(buf == NULL)
            return LV_RESULT_INVALID;

        offset += dsc->palette_size * 4; /*Skip palette*/
        offset += decoded_area->y1 * ((dsc->header.w * bpp + 7) / 8); /*Move to y1*/
        offset += decoded_area->x1 * bpp / 8; /*Move to x1*/
        res = fs_read_file_at(f, offset, buf, len, NULL);
        if(res != LV_FS_RES_OK) {
            lv_free(buf);
            return LV_RESULT_INVALID;
        }

        decode_indexed_line(cf, dsc->palette, x_fraction, 0, w_px, buf, (lv_color32_t *)img_data);
        lv_free(buf);

        dsc->img_data = img_data; /*Return decoded image*/
        return LV_RESULT_OK;
    }

    if(cf == LV_COLOR_FORMAT_ARGB8888 || cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_RGB888
       || cf == LV_COLOR_FORMAT_RGB565) {
        uint32_t len = (w_px * bpp) / 8;
        offset += decoded_area->y1 * dsc->header.w * bpp / 8; /*Move to y1*/
        offset += decoded_area->x1 * bpp / 8; /*Move to x1*/
        res = fs_read_file_at(f, offset, img_data, len, NULL);
        if(res != LV_FS_RES_OK) {
            return LV_RESULT_INVALID;
        }

        dsc->img_data = img_data; /*Return decoded image*/
        return LV_RESULT_OK;
    }


    if(cf == LV_COLOR_FORMAT_RGB565A8) {
        bpp = 16; /* RGB565 + A8 mask*/
        uint32_t len = (w_px * bpp) / 8; /*map comes firstly*/
        offset += decoded_area->y1 * dsc->header.w * bpp / 8; /*Move to y1*/
        offset += decoded_area->x1 * bpp / 8; /*Move to x1*/
        res = fs_read_file_at(f, offset, img_data, len, NULL);
        if(res != LV_FS_RES_OK) {
            return LV_RESULT_INVALID;
        }

        /*Now the A8 mask*/
        offset = sizeof(lv_image_header_t);
        offset += dsc->header.h * dsc->header.w * bpp / 8; /*Move to A8 map*/
        offset += decoded_area->y1 * dsc->header.w * 1; /*Move to y1*/
        offset += decoded_area->x1 * 1; /*Move to x1*/
        res = fs_read_file_at(f, offset, img_data + len, w_px * 1, NULL);
        if(res != LV_FS_RES_OK) {
            return LV_RESULT_INVALID;
        }

        dsc->img_data = img_data; /*Return decoded image*/
        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t decode_indexed_line(lv_color_format_t color_format, const lv_color32_t * palette, lv_coord_t x,
                                       lv_coord_t y,
                                       lv_coord_t w_px, const uint8_t * in, lv_color32_t * out)
{
    uint8_t px_size;
    uint16_t mask;

    out += w_px * y;

    lv_coord_t w_byte = 0;
    int8_t shift   = 0;
    switch(color_format) {
        case LV_COLOR_FORMAT_I1:
            px_size = 1;
            w_byte = (w_px + 7) >> 3;   /*E.g. w = 20 -> w = 2 + 1*/
            in += w_byte * y;           /*First pixel*/
            in += x / 8;                /*8pixel per byte*/
            shift = 7 - (x & 0x7);
            break;
        case LV_COLOR_FORMAT_I2:
            px_size = 2;
            w_byte = (w_px + 3) >> 2;   /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            in += w_byte * y;           /*First pixel*/
            in += x / 4;                /*4pixel per byte*/
            shift = 6 - 2 * (x & 0x3);
            break;
        case LV_COLOR_FORMAT_I4:
            px_size = 4;
            w_byte = (w_px + 1) >> 1;   /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            in += w_byte * y;           /*First pixel*/
            in += x / 2;                /*2pixel per byte*/
            shift = 4 - 4 * (x & 0x1);
            break;
        case LV_COLOR_FORMAT_I8:
            px_size = 8;
            w_byte = w_px;
            in += w_byte * y;  /*First pixel*/
            in += x;
            shift = 0;
            break;
        default:
            return LV_RESULT_INVALID;
    }

    mask   = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    lv_coord_t i;
    for(i = 0; i < w_px; i++) {
        uint8_t val_act = (*in >> shift) & mask;
        out[i] = palette[val_act];

        shift -= px_size;
        if(shift < 0) {
            shift = 8 - px_size;
            in++;
        }
    }
    return LV_RESULT_OK;
}

static uint32_t img_width_to_stride(lv_image_header_t * header)
{
    if(header->cf == LV_COLOR_FORMAT_RGB565A8) {
        return header->w * 2;
    }
    else {
        return header->w * lv_color_format_get_size(header->cf);
    }
}

static lv_fs_res_t fs_read_file_at(lv_fs_file_t * f, uint32_t pos, uint8_t * buff, uint32_t btr, uint32_t * br)
{
    lv_fs_res_t res;
    if(br) *br = 0;

    res = lv_fs_seek(f, pos, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        return res;
    }

    res |= lv_fs_read(f, buff, btr, br);
    if(res != LV_FS_RES_OK) {
        return res;
    }

    return LV_FS_RES_OK;
}
