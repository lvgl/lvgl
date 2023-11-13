/**
 * @file lv_rle_decoder.c
 * Decode RLE compressed original lvgl binary file. File name suffix is always
 * set to '.rle' and the file header is added with another 64bit including
 * 32bit magic number and 32bit rle information.
 *
 * Decoder support both variable and file as image source.
 *
 * The original binary data is directly decompressed to ram, and decoded by
 * lvgl built-in decoder.
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include "lv_rle_decoder.h"

#if LV_USE_RLE

/*********************
 *      DEFINES
 *********************/

#define RLE_HEADER_MAGIC        0x5aa521e0
#define RLE_DECODER_OPTIMIZE_FS 1 /* load file to RAM to decode. */
#define RLE_DECODER_PERF        0 /* output file load decoder performance */

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_image_decoder_dsc_t decoder_dsc;
    lv_image_dsc_t img_dsc;
} lv_rle_decoder_data_t;

typedef struct {
    uint32_t magic;         /* 0x5aa521e0 */
    uint32_t blksize: 4;    /* block size, the encoded data unit bytes. */
    uint32_t len_orig: 24;  /* Original data length. */
    uint32_t reserved: 4;
} lv_rle_header_t;

typedef struct {
    lv_rle_header_t rleheader;
    lv_image_header_t header;
} lv_rle_file_header_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_info(struct _lv_image_decoder_t * decoder,
                             const void * src, lv_image_header_t * header);
static lv_res_t decoder_open(lv_image_decoder_t * dec,
                             lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * dec, lv_image_decoder_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_rle_decoder_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if RLE_DECODER_OPTIMIZE_FS == 0
static int rle_decompress_from_file(lv_fs_file_t * f, uint8_t * output,
                                    uint32_t len, uint32_t blk_size)
{
    uint8_t ctrl_byte;
    uint32_t wr_len = 0;
    lv_fs_res_t res;

    while(1) {
        uint32_t rd;
        res = lv_fs_read(f, &ctrl_byte, 1, &rd);
        if(res != LV_FS_RES_OK) {
            return 0;
        }

        if(rd == 0)
            break;

        if(ctrl_byte & 0x80) {
            /* copy directly from input to output */
            uint32_t bytes = blk_size * (ctrl_byte & 0x7f);

            wr_len += bytes;
            if(wr_len > len)
                return 0;

            res = lv_fs_read(f, output, bytes, &rd);
            if(rd != bytes || res != LV_FS_RES_OK) {
                return 0;
            }
            output += bytes;
        }
        else {
            wr_len += blk_size * ctrl_byte;
            if(wr_len > len)
                return 0;

            uint8_t input[blk_size];
            res = lv_fs_read(f, input, blk_size, &rd);
            if(rd != blk_size || res != LV_FS_RES_OK) {
                return 0;
            }

            if(blk_size == 1) {
                /* optimize the most common case. */
                memset(output, input[0], ctrl_byte);
                output += ctrl_byte;
            }
            else {
                for(int i = 0; i < ctrl_byte; i++) {
                    memcpy(output, input, blk_size);
                    output += blk_size;
                }
            }

        }
    }

    return wr_len;
}
#endif

static int rle_decompress_from_mem(const uint8_t * input,
                                   uint32_t input_buff_len, uint8_t * output,
                                   uint32_t output_buff_len, uint8_t blk_size)
{
    uint32_t ctrl_byte;
    uint32_t rd_len = 0;
    uint32_t wr_len = 0;

    while(rd_len < input_buff_len) {
        ctrl_byte = input[0];
        rd_len++;
        input++;
        if(rd_len > input_buff_len)
            return 0;

        if(ctrl_byte & 0x80) {
            /* copy directly from input to output */
            uint32_t bytes = blk_size * (ctrl_byte & 0x7f);
            rd_len += bytes;
            if(rd_len > input_buff_len)
                return 0;

            wr_len += bytes;
            if(wr_len > output_buff_len)
                return 0;

            memcpy(output, input, bytes);
            output += bytes;
            input += bytes;
        }
        else {
            rd_len += blk_size;
            if(rd_len > input_buff_len)
                return 0;

            wr_len += blk_size * ctrl_byte;
            if(wr_len > output_buff_len)
                return 0;

            if(blk_size == 1) {
                /* optimize the most common case. */
                memset(output, input[0], ctrl_byte);
                output += ctrl_byte;
            }
            else {
                for(uint32_t i = 0; i < ctrl_byte; i++) {
                    memcpy(output, input, blk_size);
                    output += blk_size;
                }
            }
            input += blk_size;
        }
    }

    return wr_len;
}

static lv_res_t decoder_info(struct _lv_image_decoder_t * decoder,
                             const void * src, lv_image_header_t * header)
{
    (void) decoder; /*Unused*/
    lv_image_src_t src_type = lv_image_src_get_type(src);   /*Get the source type*/

    if(src_type == LV_IMAGE_SRC_FILE) {
        lv_fs_res_t res;
        uint32_t rn;
        lv_fs_file_t f;
        lv_rle_file_header_t file_header = { 0 };
        /*Support only "*.rle" files*/
        if(strcmp(lv_fs_get_ext(src), "rle") != 0)
            return LV_RES_INV;

        res = lv_fs_open(&f, src, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK)
            return LV_RES_INV;

        res = lv_fs_read(&f, &file_header, sizeof(lv_rle_file_header_t), &rn);
        lv_fs_close(&f);

        if((res != LV_FS_RES_OK) || (rn != sizeof(lv_rle_file_header_t))) {
            LV_LOG_WARN("Image get info get read file header");
            return LV_RES_INV;
        }

        if(file_header.rleheader.magic != RLE_HEADER_MAGIC) {
            return LV_RES_INV;
        }

        *header = file_header.header;

        return LV_RES_OK;
    }
    else if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        const lv_rle_header_t * rleheader = (const lv_rle_header_t *)img_dsc->data;
        if(rleheader->magic != RLE_HEADER_MAGIC) {
            return LV_RES_INV;
        }

        header->always_zero = 0;
        header->cf = img_dsc->header.cf;
        header->w = img_dsc->header.w;
        header->h = img_dsc->header.h;

        return LV_RES_OK;
    }

    return LV_RES_INV;
}

static inline lv_res_t decode_from_file(lv_image_decoder_t * decoder,
                                        lv_image_decoder_dsc_t * dsc,
                                        lv_rle_file_header_t * fileheader,
                                        void ** img_data)
{
    LV_UNUSED(decoder);
    lv_fs_res_t res;
    lv_fs_file_t f;
    uint32_t rd_cnt;
    uint32_t px_size;
    void * img_buf;
    uint32_t buf_len;
    uint32_t decoded_len;
    lv_image_header_t * header = &fileheader->header;
    lv_rle_header_t * rleheader = &fileheader->rleheader;
    const char * fn = dsc->src;

    res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("RLE image decoder can't open the file");
        return LV_RES_INV;
    }

    res = lv_fs_read(&f, fileheader, sizeof(lv_rle_file_header_t), &rd_cnt);
    if(rleheader->magic != RLE_HEADER_MAGIC) {
        return LV_RES_INV;
    }

    if((res != LV_FS_RES_OK) || (rd_cnt != sizeof(lv_rle_file_header_t))) {
        LV_LOG_WARN("RLE image decoder read header failed.");
        goto error_file;
    }

    px_size = lv_color_format_get_size(header->cf);
    if(px_size == 0) {
        goto error_file;
    }

    if(px_size != rleheader->blksize) {
        LV_LOG_WARN("Invalid rle file, blksize mismatch, expect: %" LV_PRIu32", got %d", px_size, rleheader->blksize);
        goto error_file;
    }

    buf_len = rleheader->len_orig;
    if(buf_len == 0) {
        LV_LOG_WARN("Invalid rle file, len_orig %d", rleheader->len_orig);
        goto error_file;
    }

    img_buf = lv_draw_buf_malloc(buf_len, header->cf);
    if(img_buf == NULL) {
        LV_LOG_ERROR("alloc img buf failed: %" LV_PRId32, buf_len);
        goto error_file;
    }

#if RLE_DECODER_OPTIMIZE_FS
    uint32_t size = 0;
    if(lv_fs_seek(&f, 0, LV_FS_SEEK_END) != 0) {
        goto error_with_img_buf;
    }

    lv_fs_tell(&f, &size);
    size -= sizeof(lv_rle_file_header_t);

    if(lv_fs_seek(&f, sizeof(lv_rle_file_header_t), LV_FS_SEEK_SET) != 0) {
        goto error_with_img_buf;
    }

    void * file_buffer = lv_malloc(size);
    if(file_buffer == NULL) {
        goto error_with_img_buf;
    }

#if RLE_DECODER_PERF
    int start, read_cost, decompress_cost;

    start = lv_tick_get();
#endif

    res = lv_fs_read(&f, file_buffer, size, &rd_cnt);
    if(res != LV_RES_INV || rd_cnt != size) {
        lv_free(file_buffer);
        goto error_with_img_buf;
    }

    lv_fs_close(&f);

#if RLE_DECODER_PERF
    read_cost = lv_tick_elaps(start);
    start = lv_tick_get();
#endif

    decoded_len = rle_decompress_from_mem(file_buffer, size, img_buf, buf_len,
                                          rleheader->blksize);

#if RLE_DECODER_PERF
    decompress_cost = lv_tick_elaps(start);
    LV_LOG_USER("file size: %" LV_PRIu32 ", img size: %" LV_PRIu32 ", readtime: %d, decode: %d\n",
                size, buf_len, read_cost, decompress_cost);
#endif

    lv_free(file_buffer);

    if(decoded_len != buf_len) {
        LV_LOG_WARN("rle decode failed, decoded len: %" LV_PRIu32
                    ", expected %" LV_PRIu32 ".",
                    decoded_len, buf_len);
        lv_free(img_buf);
        return LV_RES_INV;
    }
#else
    /**
     * Read and decompress data.
     */
    decoded_len = rle_decompress_from_file(&f, img_buf, buf_len,
                                           rleheader->blksize);

    lv_fs_close(&f);
    if(decoded_len != buf_len) {
        LV_LOG_WARN("rle decode failed, decoded len: %d, expect %d.",
                    decoded_len, buf_len);
        lv_free(img_buf);
        return LV_RES_INV;
    }
#endif

    *img_data = img_buf;
    return LV_RES_OK;

#if RLE_DECODER_OPTIMIZE_FS
error_with_img_buf:
    lv_free(img_buf);
#endif

error_file:
    lv_fs_close(&f);
    return LV_RES_INV;
}

static inline lv_res_t decode_from_variable(lv_image_decoder_t * decoder,
                                            lv_image_decoder_dsc_t * dsc,
                                            lv_rle_file_header_t * fileheader,
                                            void ** img_data)
{
    LV_UNUSED(decoder);
    uint32_t px_size;
    void * img_buf;
    uint32_t buf_len;
    uint32_t decoded_len;
    const lv_image_dsc_t * img_dsc = dsc->src;
    const lv_rle_header_t * rleheader = (const lv_rle_header_t *)img_dsc->data;
    const lv_image_header_t * header = &img_dsc->header;

    if(rleheader->magic != RLE_HEADER_MAGIC) {
        return LV_RES_INV;
    }

    px_size = lv_color_format_get_size(header->cf);
    if(px_size == 0) {
        return LV_RES_INV;
    }

    if(px_size != rleheader->blksize) {
        LV_LOG_WARN("Invalid rle file, blksize mismatch, expect: %" LV_PRIu32
                    ", got %d",
                    px_size, rleheader->blksize);
        return LV_RES_INV;
    }

    buf_len = rleheader->len_orig;
    if(buf_len == 0) {
        LV_LOG_WARN("Invalid rle file, len_orig %d", rleheader->len_orig);
        return LV_RES_INV;
    }

    img_buf = lv_malloc(buf_len);
    if(img_buf == NULL) {
        return LV_RES_INV;
    }

    const uint8_t * compressed_data = img_dsc->data + sizeof(lv_rle_header_t);
    uint32_t compressed_data_len = img_dsc->data_size - sizeof(lv_rle_header_t);

    decoded_len = rle_decompress_from_mem(compressed_data, compressed_data_len,
                                          img_buf, buf_len, rleheader->blksize);

    if(decoded_len != buf_len) {
        LV_LOG_WARN("rle decode failed, decoded len: %" LV_PRIu32
                    ", expected %" LV_PRIu32 ".",
                    decoded_len, buf_len);
        lv_free(img_buf);
        return LV_RES_INV;
    }

    fileheader->header = *header;
    fileheader->rleheader = *rleheader;
    *img_data = img_buf;
    return LV_RES_OK;
}

static lv_res_t decoder_open(lv_image_decoder_t * decoder,
                             lv_image_decoder_dsc_t * dsc)
{
    lv_fs_res_t res;
    lv_rle_file_header_t fileheader;
    lv_rle_decoder_data_t * data;
    void * img_data;
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        res = decode_from_file(decoder, dsc, &fileheader, &img_data);
        if(res != LV_RES_OK)
            return res;
    }

    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        res = decode_from_variable(decoder, dsc, &fileheader, &img_data);

        if(res != LV_RES_OK)
            return res;
    }
    else {
        return LV_RES_INV;
    }

    /*Decoder private data.*/
    data = lv_malloc(sizeof(*data));
    if(data == NULL) {
        return LV_RES_INV;
    }

    lv_memcpy(&data->decoder_dsc, dsc, sizeof(*dsc));

    data->img_dsc.header = fileheader.header;
    data->img_dsc.data = img_data;
    data->img_dsc.data_size = fileheader.rleheader.len_orig;

    data->decoder_dsc.img_data = NULL;
    data->decoder_dsc.src_type = LV_IMAGE_SRC_VARIABLE;
    data->decoder_dsc.src = &data->img_dsc;

    res = lv_image_decoder_built_in_open(decoder, &data->decoder_dsc);
    if(res != LV_RES_OK) {
        lv_free(img_data);
        lv_free(data);
        return LV_RES_INV;
    }

    dsc->img_data = data->decoder_dsc.img_data;
    dsc->header = data->decoder_dsc.header;
    dsc->user_data = data;
    return LV_RES_OK;
}

static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    if(dsc->user_data) {
        lv_rle_decoder_data_t * decoder_data = dsc->user_data;
        lv_image_decoder_built_in_close(decoder, &decoder_data->decoder_dsc);
        if(decoder_data->img_dsc.data)
            lv_free((void *)decoder_data->img_dsc.data);
        lv_free(dsc->user_data);
        dsc->user_data = NULL;
    }

    if(dsc->img_data) {
        dsc->img_data = NULL;
    }
}

#endif /*LV_USE_RLE*/
