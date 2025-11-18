/**
 * @file lv_libjpeg_turbo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_image_decoder_private.h"
#include "../../../lvgl.h"
#if LV_USE_LIBJPEG_TURBO

#include "lv_libjpeg_turbo.h"
#include <stdio.h>
#include <jpeglib.h>
#include <jpegint.h>
#include <setjmp.h>
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "JPEG_TURBO"

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

#define JPEG_PIXEL_SIZE 3 /* RGB888 */
#define JPEG_SIGNATURE 0xFFD8FF
#define IS_JPEG_SIGNATURE(x) (((x) & 0x00FFFFFF) == JPEG_SIGNATURE)

/**********************
 *      TYPEDEFS
 **********************/
typedef struct error_mgr_s {
    struct jpeg_error_mgr pub;
    jmp_buf jb;
} error_mgr_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_draw_buf_t * decode_jpeg_file(const char * filename);
static uint8_t * read_file(const char * filename, uint32_t * size);
static bool get_jpeg_head_info(const char * filename, uint32_t * width, uint32_t * height, uint32_t * orientation);
static bool get_jpeg_size(uint8_t * data, uint32_t data_size, uint32_t * width, uint32_t * height);
static bool get_jpeg_direction(uint8_t * data, uint32_t data_size, uint32_t * orientation);
static void rotate_buffer(lv_draw_buf_t * decoded, uint8_t * buffer, uint32_t line_index, uint32_t angle,
                          uint32_t row_stride);
static void error_exit(j_common_ptr cinfo);
/**********************
 *  STATIC VARIABLES
 **********************/
const int JPEG_EXIF = 0x45786966; /* Exif data structure tag */
const int JPEG_BIG_ENDIAN_TAG = 0x4d4d;
const int JPEG_LITTLE_ENDIAN_TAG = 0x4949;

/**********************
 *      MACROS
 **********************/
#define TRANS_32_VALUE(big_endian, data) big_endian ? \
    ((*(data) << 24) | (*((data) + 1) << 16) | (*((data) + 2) << 8) | *((data) + 3)) : \
    (*(data) | (*((data) + 1) << 8) | (*((data) + 2) << 16) | (*((data) + 3) << 24))
#define TRANS_16_VALUE(big_endian, data) big_endian ? \
    ((*(data) << 8) | *((data) + 1)) : (*(data) | (*((data) + 1) << 8))

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register the JPEG decoder functions in LVGL
 */
void lv_libjpeg_turbo_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);

    dec->name = DECODER_NAME;
}

void lv_libjpeg_turbo_deinit(void)
{
    lv_image_decoder_t * dec = NULL;
    while((dec = lv_image_decoder_get_next(dec)) != NULL) {
        if(dec->info_cb == decoder_info) {
            lv_image_decoder_delete(dec);
            break;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get info about a JPEG image
 * @param dsc image descriptor containing the source and type of the image and other info.
 * @param header store the info here
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Unused*/
    lv_image_src_t src_type = dsc->src_type;          /*Get the source type*/

    /*If it's a JPEG file...*/
    if(src_type == LV_IMAGE_SRC_FILE) {
        const char * src = dsc->src;
        uint32_t jpg_signature = 0;
        uint32_t rn;
        lv_fs_read(&dsc->file, &jpg_signature, sizeof(jpg_signature), &rn);

        if(rn != sizeof(jpg_signature)) {
            LV_LOG_WARN("file: %s signature len = %" LV_PRIu32 " error", src, rn);
            return LV_RESULT_INVALID;
        }

        const char * ext = lv_fs_get_ext(src);
        bool is_jpeg_ext = (lv_strcmp(ext, "jpg") == 0)
                           || (lv_strcmp(ext, "jpeg") == 0);

        if(!IS_JPEG_SIGNATURE(jpg_signature)) {
            if(is_jpeg_ext) {
                LV_LOG_WARN("file: %s signature = 0X%" LV_PRIX32 " error", src, jpg_signature);
            }
            return LV_RESULT_INVALID;
        }

        uint32_t width;
        uint32_t height;
        uint32_t orientation = 0;

        if(!get_jpeg_head_info(src, &width, &height, &orientation)) {
            return LV_RESULT_INVALID;
        }

        /*Save the data in the header*/
        header->cf = LV_COLOR_FORMAT_RGB888;
        header->w = (orientation % 180) ? height : width;
        header->h = (orientation % 180) ? width : height;

        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;         /*If didn't succeeded earlier then it's an error*/
}

/**
 * Open a JPEG image and return the decided image
 * @param decoder pointer to the decoder
 * @param dsc     pointer to the decoder descriptor
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    /*If it's a JPEG file...*/
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;
        lv_draw_buf_t * decoded = decode_jpeg_file(fn);
        if(decoded == NULL) {
            LV_LOG_WARN("decode jpeg file failed");
            return LV_RESULT_INVALID;
        }

        dsc->decoded = decoded;

        if(dsc->args.no_cache) return LV_RESULT_OK;

        /*If the image cache is disabled, just return the decoded image*/
        if(!lv_image_cache_is_enabled()) return LV_RESULT_OK;

        /*Add the decoded image to the cache*/
        lv_image_cache_data_t search_key;
        search_key.src_type = dsc->src_type;
        search_key.src = dsc->src;
        search_key.slot.size = decoded->data_size;

        lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, decoded, NULL);

        if(entry == NULL) {
            lv_draw_buf_destroy(decoded);
            return LV_RESULT_INVALID;
        }
        dsc->cache_entry = entry;
        return LV_RESULT_OK;    /*If not returned earlier then it failed*/
    }

    return LV_RESULT_INVALID;    /*If not returned earlier then it failed*/
}

/**
 * Free the allocated resources
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    if(dsc->args.no_cache ||
       !lv_image_cache_is_enabled()) lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
}

static uint8_t * read_file(const char * filename, uint32_t * size)
{
    uint8_t * data = NULL;
    lv_fs_file_t f;
    uint32_t data_size;
    uint32_t rn;
    lv_fs_res_t res;

    *size = 0;

    res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("can't open %s", filename);
        return NULL;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_tell(&f, &data_size);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    /*Read file to buffer*/
    data = lv_malloc(data_size);
    if(data == NULL) {
        LV_LOG_WARN("malloc failed for data");
        goto failed;
    }

    res = lv_fs_read(&f, data, data_size, &rn);

    if(res == LV_FS_RES_OK && rn == data_size) {
        *size = rn;
    }
    else {
        LV_LOG_WARN("read file failed");
        lv_free(data);
        data = NULL;
    }

failed:
    lv_fs_close(&f);

    return data;
}

static lv_draw_buf_t * decode_jpeg_file(const char * filename)
{
    /* This struct contains the JPEG decompression parameters and pointers to
     * working space (which is allocated as needed by the JPEG library).
     */
    struct jpeg_decompress_struct cinfo;
    /* We use our private extension JPEG error handler.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    error_mgr_t jerr;

    /* More stuff */
    JSAMPARRAY buffer;  /* Output row buffer */

    uint32_t row_stride;     /* physical row width in output buffer */
    uint32_t image_angle = 0;   /* image rotate angle */

    lv_draw_buf_t * decoded = NULL;

    /* In this example we want to open the input file before doing anything else,
     * so that the setjmp() error recovery below can assume the file is open.
     * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
     * requires it in order to read binary files.
     */

    uint32_t data_size;
    uint8_t * data = read_file(filename, &data_size);
    if(data == NULL) {
        LV_LOG_WARN("can't load file %s", filename);
        return NULL;
    }

    /* allocate and initialize JPEG decompression object */

    /* We set up the normal JPEG error routines, then override error_exit. */
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit;
    /* Establish the setjmp return context for my_error_exit to use. */
    if(setjmp(jerr.jb)) {

        LV_LOG_WARN("decoding error");

        if(decoded) {
            lv_draw_buf_destroy(decoded);
        }

        /* If we get here, the JPEG code has signaled an error.
        * We need to clean up the JPEG object, close the input file, and return.
        */
        jpeg_destroy_decompress(&cinfo);
        lv_free(data);
        return NULL;
    }

    /* Get rotate angle from Exif data */
    if(!get_jpeg_direction(data, data_size, &image_angle)) {
        LV_LOG_WARN("read jpeg orientation failed.");
    }

    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&cinfo);

    /* specify data source (eg, a file or buffer) */

    jpeg_mem_src(&cinfo, data, data_size);

    /* read file parameters with jpeg_read_header() */

    jpeg_read_header(&cinfo, TRUE);

    /* We can ignore the return value from jpeg_read_header since
     *   (a) suspension is not possible with the stdio data source, and
     *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
     * See libjpeg.doc for more info.
     */

    /* set parameters for decompression */

    cinfo.out_color_space = JCS_EXT_BGR;

    /* In this example, we don't need to change any of the defaults set by
     * jpeg_read_header(), so we do nothing here.
     */

    /* Start decompressor */

    jpeg_start_decompress(&cinfo);

    /* We can ignore the return value since suspension is not possible
     * with the stdio data source.
     */

    /* We may need to do some setup of our own at this point before reading
     * the data.  After jpeg_start_decompress() we have the correct scaled
     * output image dimensions available, as well as the output colormap
     * if we asked for color quantization.
     * In this example, we need to make an output work buffer of the right size.
     */
    /* JSAMPLEs per row in output buffer */
    row_stride = cinfo.output_width * cinfo.output_components;
    /* Make a one-row-high sample array that will go away when done with image */
    buffer = (*cinfo.mem->alloc_sarray)
             ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    uint32_t buf_width = (image_angle % 180) ? cinfo.output_height : cinfo.output_width;
    uint32_t buf_height = (image_angle % 180) ? cinfo.output_width : cinfo.output_height;
    decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, buf_width, buf_height, LV_COLOR_FORMAT_RGB888,
                                    LV_STRIDE_AUTO);
    if(decoded != NULL) {
        uint32_t line_index = 0;
        /* while (scan lines remain to be read) */
        /* jpeg_read_scanlines(...); */

        /* Here we use the library's state variable cinfo.output_scanline as the
         * loop counter, so that we don't have to keep track ourselves.
         */
        while(cinfo.output_scanline < cinfo.output_height) {
            /* jpeg_read_scanlines expects an array of pointers to scanlines.
             * Here the array is only one element long, but you could ask for
             * more than one scanline at a time if that's more convenient.
             */
            jpeg_read_scanlines(&cinfo, buffer, 1);

            /* Assume put_scanline_someplace wants a pointer and sample count. */
            rotate_buffer(decoded, buffer[0], line_index, image_angle, row_stride);

            line_index++;
        }
    }

    /* Finish decompression */

    jpeg_finish_decompress(&cinfo);

    /* We can ignore the return value since suspension is not possible
     * with the stdio data source.
     */

    /* Release JPEG decompression object */

    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress(&cinfo);

    /* After finish_decompress, we can close the input file.
    * Here we postpone it until after no more JPEG errors are possible,
    * so as to simplify the setjmp error logic above.  (Actually, I don't
    * think that jpeg_destroy can do an error exit, but why assume anything...)
    */
    lv_free(data);

    /* At this point you may want to check to see whether any corrupt-data
    * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
    */

    /* And we're done! */
    return decoded;
}

static bool get_jpeg_head_info(const char * filename, uint32_t * width, uint32_t * height, uint32_t * orientation)
{
    uint8_t * data = NULL;
    uint32_t data_size;
    data = read_file(filename, &data_size);
    if(data == NULL) {
        return false;
    }

    if(!get_jpeg_size(data, data_size, width, height)) {
        LV_LOG_WARN("read jpeg size failed.");
    }

    if(!get_jpeg_direction(data, data_size, orientation)) {
        LV_LOG_WARN("read jpeg orientation failed.");
    }

    lv_free(data);

    return JPEG_HEADER_OK;
}

static bool get_jpeg_size(uint8_t * data, uint32_t data_size, uint32_t * width, uint32_t * height)
{
    struct jpeg_decompress_struct cinfo;
    error_mgr_t jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit;

    if(setjmp(jerr.jb)) {
        LV_LOG_WARN("read jpeg head failed");
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    jpeg_create_decompress(&cinfo);

    jpeg_mem_src(&cinfo, data, data_size);

    int ret = jpeg_read_header(&cinfo, TRUE);

    if(ret == JPEG_HEADER_OK) {
        *width = cinfo.image_width;
        *height = cinfo.image_height;
    }
    else {
        LV_LOG_WARN("read jpeg head failed: %d", ret);
    }

    jpeg_destroy_decompress(&cinfo);

    return JPEG_HEADER_OK;
}

static bool get_jpeg_direction(uint8_t * data, uint32_t data_size, uint32_t * orientation)
{
    struct jpeg_decompress_struct cinfo;
    error_mgr_t jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit;

    if(setjmp(jerr.jb)) {
        LV_LOG_WARN("read jpeg orientation failed");
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    jpeg_create_decompress(&cinfo);

    jpeg_mem_src(&cinfo, data, data_size);

    jpeg_save_markers(&cinfo, JPEG_APP0 + 1, 0xFFFF);

    cinfo.marker->read_markers(&cinfo);

    jpeg_saved_marker_ptr marker = cinfo.marker_list;
    while(marker != NULL) {
        if(marker->marker == JPEG_APP0 + 1) {
            JOCTET FAR * app1_data = marker->data;
            if(TRANS_32_VALUE(true, app1_data) == JPEG_EXIF) {
                uint16_t endian_tag = TRANS_16_VALUE(true, app1_data + 4 + 2);
                if(!(endian_tag == JPEG_LITTLE_ENDIAN_TAG || endian_tag == JPEG_BIG_ENDIAN_TAG)) {
                    jpeg_destroy_decompress(&cinfo);
                    return false;
                }
                bool is_big_endian = endian_tag == JPEG_BIG_ENDIAN_TAG;
                /* first ifd offset addr : 4bytes(Exif) + 2bytes(0x00) + 2bytes(align) + 2bytes(tag mark) */
                unsigned int offset = TRANS_32_VALUE(is_big_endian, app1_data + 8 + 2);
                /* ifd base : 4bytes(Exif) + 2bytes(0x00) */
                unsigned char * ifd = 0;
                do {
                    /* ifd start: 4bytes(Exif) + 2bytes(0x00) + offset value(2bytes(align) + 2bytes(tag mark) + 4bytes(offset size)) */
                    unsigned int entry_offset = 4 + 2 + offset + 2;
                    if(entry_offset >= marker->data_length) {
                        jpeg_destroy_decompress(&cinfo);
                        return false;
                    }
                    ifd = app1_data + entry_offset;
                    unsigned short num_entries = TRANS_16_VALUE(is_big_endian, ifd - 2);
                    if(entry_offset + num_entries * 12 >= marker->data_length) {
                        jpeg_destroy_decompress(&cinfo);
                        return false;
                    }
                    for(int i = 0; i < num_entries; i++) {
                        unsigned short tag = TRANS_16_VALUE(is_big_endian, ifd);
                        if(tag == 0x0112) {
                            /* ifd entry: 12bytes = 2bytes(tag number) + 2bytes(kind of data) + 4bytes(number of components) + 4bytes(data)
                            * orientation kind(0x03) of data is unsigned short */
                            int dirc = TRANS_16_VALUE(is_big_endian, ifd + 2 + 2 + 4);
                            switch(dirc) {
                                case 1:
                                    *orientation = 0;
                                    break;
                                case 3:
                                    *orientation = 180;
                                    break;
                                case 6:
                                    *orientation = 90;
                                    break;
                                case 8:
                                    *orientation = 270;
                                    break;
                                default:
                                    *orientation = 0;
                            }
                        }
                        ifd += 12;
                    }
                    offset = TRANS_32_VALUE(is_big_endian, ifd);
                } while(offset != 0);
            }
            break;
        }
        marker = marker->next;
    }

    jpeg_destroy_decompress(&cinfo);

    return JPEG_HEADER_OK;
}

static void rotate_buffer(lv_draw_buf_t * decoded, uint8_t * buffer, uint32_t line_index, uint32_t angle,
                          uint32_t row_stride)
{
    if(angle == 90) {
        for(uint32_t x = 0; x < decoded->header.h; x++) {
            uint32_t dst_index = x * decoded->header.stride + (decoded->header.w - line_index - 1)  * JPEG_PIXEL_SIZE;
            lv_memcpy(decoded->data + dst_index, buffer + x * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
        }
    }
    else if(angle == 180) {
        for(uint32_t x = 0; x < decoded->header.w; x++) {
            uint32_t dst_index = (decoded->header.h - line_index - 1) * decoded->header.stride + x * JPEG_PIXEL_SIZE;
            lv_memcpy(decoded->data + dst_index, buffer + (decoded->header.w - x - 1) * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
        }
    }
    else if(angle == 270) {
        for(uint32_t x = 0; x < decoded->header.h; x++) {
            uint32_t dst_index = (decoded->header.h - x - 1) * decoded->header.stride + line_index * JPEG_PIXEL_SIZE;
            lv_memcpy(decoded->data + dst_index, buffer + x * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
        }
    }
    else {
        lv_memcpy(decoded->data + line_index * decoded->header.stride, buffer, row_stride);
    }
}

static void error_exit(j_common_ptr cinfo)
{
    error_mgr_t * myerr = (error_mgr_t *)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->jb, 1);
}

#endif /*LV_USE_LIBJPEG_TURBO*/
