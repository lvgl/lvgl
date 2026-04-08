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
#define ORIENTATION_TAG 0x112 /* Exif tag for orientation */
#define APP1_MARKER JPEG_APP0 + 1  /* APP1 Marker code https://www.media.mit.edu/pia/Research/deepview/exif.html */
#define MARKER_DATA_LIMIT 0xFFFF /* APP1 Marker limit */

/**********************
 *      TYPEDEFS
 **********************/
/**
* according to the Exif specification(http://www.cipa.jp/std/documents/e/DC-008-Translation-2019-E.pdf)
* Relationship between image data and orientation on a display screen according to an orientation tag
*/
typedef enum {
    /* Orientation = 0 is created when the image data in the Exif is not rotated */
    IMAGE_CLOCKWISE_NONE    = 0,
    /* Orientation = 1 is created when Oth row of the coded image data stored in the Exif image file
     * and the visual top of the display screen, and Oth column and visual left, will each be matched for display
     */
    IMAGE_CLOCKWISE_0       = 1,
    /* Orientation = 2 is equivalent to an arrangement that is reversed Orientation = 1 horizontally */
    IMAGE_FLIP_HOR          = 2,
    /* Orientation = 3 is equivalent to an arrangement that is turned Orientation = 6 90 degrees clockwise */
    IMAGE_CLOCKWISE_180     = 3,
    /* Orientation = 4 is equivalent to an arrangement that is reversed Orientation = 3 horizontally */
    IMAGE_FLIP_VER          = 4,
    /* Orientation = 5 is equivalent to an arrangement that is reversed Orientation = 6 horizontally */
    IMAGE_TRANSPOSE         = 5,
    /* Orientation = 6 is equivalent to an arrangement that is turned Orientation = 1 90 degrees clockwise */
    IMAGE_CLOCKWISE_90      = 6,
    /* Orientation = 7 is equivalent to an arrangement that is reversed Orientation = 8 horizontally */
    IMAGE_TRANSVERSE        = 7,
    /* Orientation = 8 is equivalent to an arrangement that is turned Orientation = 3 90 degrees clockwise */
    IMAGE_CLOCKWISE_270     = 8,
} image_orientation_t;

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
static void convert_size_with_orientation(image_orientation_t image_orientation, uint32_t * width, uint32_t * height);
static lv_draw_buf_t * decode_jpeg_file(const char * filename);
static bool get_jpeg_head_info(const char * filename, uint32_t * width, uint32_t * height);
static bool get_jpeg_size(uint8_t * data, uint32_t data_size, uint32_t * width, uint32_t * height);
static image_orientation_t get_jpeg_direction(uint8_t * data, uint32_t data_size);
static inline void process_buffer_orientation(image_orientation_t op, uint8_t * cur_pos, lv_image_header_t * header,
                                              uint32_t line_index, uint8_t * buffer, uint32_t buf_stride);
static image_orientation_t jpeg_markers_reader(struct jpeg_decompress_struct * cinfo);
static void jpeg_cmyk_to_bgrx(uint8_t * cmyk_data, uint32_t pixel_count);
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

        if(!get_jpeg_head_info(src, &width, &height)) {
            return LV_RESULT_INVALID;
        }

        /*Save the data in the header*/
        header->cf = LV_COLOR_FORMAT_RGB888;
        header->w = width;
        header->h = height;

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

static void convert_size_with_orientation(image_orientation_t image_orientation, uint32_t * width, uint32_t * height)
{
    if(image_orientation == IMAGE_CLOCKWISE_NONE || image_orientation == IMAGE_CLOCKWISE_0
       || image_orientation == IMAGE_CLOCKWISE_180 || image_orientation == IMAGE_FLIP_VER ||
       image_orientation == IMAGE_FLIP_HOR) {
        return;
    }

    uint32_t tmp = *width;
    *width = *height;
    *height = tmp;
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

    lv_draw_buf_t * decoded = NULL;

    /* In this example we want to open the input file before doing anything else,
     * so that the setjmp() error recovery below can assume the file is open.
     * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
     * requires it in order to read binary files.
     */

    uint32_t data_size;
    uint8_t * data = lv_fs_load_with_alloc(filename, &data_size);
    if(data == NULL) {
        LV_LOG_WARN("can't load file %s", filename);
        return NULL;
    }

    /* read jpeg exif orientation */
    image_orientation_t image_orientation = get_jpeg_direction(data, data_size);

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
    if(cinfo.jpeg_color_space == JCS_CMYK || cinfo.jpeg_color_space == JCS_YCCK) {
        cinfo.out_color_space = JCS_CMYK;
    }
    else {
        cinfo.out_color_space = JCS_EXT_BGR;
    }

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

    uint32_t width = cinfo.output_width;
    uint32_t height = cinfo.output_height;
    convert_size_with_orientation(image_orientation, &width, &height);

    lv_image_header_t image_header = { 0 };
    image_header.w = width;
    image_header.h = height;
    image_header.stride = row_stride;

    lv_color_format_t fm = LV_COLOR_FORMAT_RGB888;
    if(cinfo.out_color_space == JCS_CMYK) {
        fm = LV_COLOR_FORMAT_XRGB8888;
    }

    /* Allocate the decoded draw buffer */
    decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, width, height, fm, LV_STRIDE_AUTO);
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

            if(cinfo.out_color_space == JCS_CMYK) {
                jpeg_cmyk_to_bgrx(buffer[0], decoded->header.w);
            }

            /* Assume put_scanline_someplace wants a pointer and sample count. */
            process_buffer_orientation(image_orientation, decoded->data, &image_header, line_index, buffer[0],
                                       decoded->header.stride);
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

    return decoded;
}

static bool get_jpeg_head_info(const char * filename, uint32_t * width, uint32_t * height)
{
    uint8_t * data = NULL;
    uint32_t data_size;
    data = lv_fs_load_with_alloc(filename, &data_size);
    if(data == NULL) {
        return false;
    }

    if(!get_jpeg_size(data, data_size, width, height)) {
        LV_LOG_WARN("read jpeg size failed.");
    }

    lv_free(data);

    return JPEG_HEADER_OK;
}

static void jpeg_decompress_prepare(struct jpeg_decompress_struct * cinfo, uint8_t * data, uint32_t data_size)
{
    jpeg_create_decompress(cinfo);
    jpeg_mem_src(cinfo, data, data_size);
    jpeg_save_markers(cinfo, APP1_MARKER, MARKER_DATA_LIMIT);
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

    /* jpeg_create_decompress */
    jpeg_decompress_prepare(&cinfo, data, data_size);

    int ret = jpeg_read_header(&cinfo, TRUE);
    if(ret != JPEG_HEADER_OK) {
        LV_LOG_WARN("read jpeg header failed: %d", ret);
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    /* read file exif orientation */
    image_orientation_t op = jpeg_markers_reader(&cinfo);

    *width = cinfo.image_width;
    *height = cinfo.image_height;
    convert_size_with_orientation(op, width, height);

    jpeg_destroy_decompress(&cinfo);

    return true;
}

static image_orientation_t get_jpeg_direction(uint8_t * data, uint32_t data_size)
{
    image_orientation_t res = IMAGE_CLOCKWISE_NONE;
    struct jpeg_decompress_struct cinfo;
    error_mgr_t jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit;

    if(setjmp(jerr.jb)) {
        LV_LOG_WARN("read jpeg orientation failed");
        jpeg_destroy_decompress(&cinfo);
        return res;
    }
    /* jpeg_create_decompress */
    jpeg_decompress_prepare(&cinfo, data, data_size);
    /* read file exif orientation */
    res = jpeg_markers_reader(&cinfo);

    jpeg_destroy_decompress(&cinfo);

    LV_LOG_INFO("read jpeg orientation data : %d", res);

    return res;
}

static inline void process_buffer_orientation(image_orientation_t op, uint8_t * cur_pos, lv_image_header_t * header,
                                              uint32_t line_index, uint8_t * buffer, uint32_t buf_stride)
{
    uint32_t dst_index = 0;
    switch(op) {
        case IMAGE_CLOCKWISE_NONE:
        case IMAGE_CLOCKWISE_0:
            lv_memcpy(cur_pos + line_index * buf_stride, buffer, header->stride);
            break;
        case IMAGE_CLOCKWISE_90:
            for(uint32_t x = 0; x < header->h; x++) {
                dst_index = x * buf_stride + (header->w - line_index - 1)  * JPEG_PIXEL_SIZE;
                lv_memcpy(cur_pos + dst_index, buffer + x * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
            }
            break;
        case IMAGE_CLOCKWISE_180:
            for(uint32_t x = 0; x < header->w; x++) {
                dst_index = (header->h - line_index - 1) * buf_stride + x * JPEG_PIXEL_SIZE;
                lv_memcpy(cur_pos + dst_index, buffer + (header->w - x - 1) * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
            }
            break;
        case IMAGE_CLOCKWISE_270:
            for(uint32_t x = 0; x < header->h; x++) {
                dst_index = (header->h - x - 1) * buf_stride + line_index * JPEG_PIXEL_SIZE;
                lv_memcpy(cur_pos + dst_index, buffer + x * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
            }
            break;
        case IMAGE_FLIP_VER:
            dst_index = (header->h - line_index - 1) * buf_stride;
            lv_memcpy(cur_pos + dst_index, buffer, header->stride);
            break;
        case IMAGE_FLIP_HOR:
            for(uint32_t x = 0; x < header->w; x++) {
                dst_index = line_index * buf_stride + (header->w - x - 1) * JPEG_PIXEL_SIZE;
                lv_memcpy(cur_pos + dst_index, buffer + x * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
            }
            break;
        case IMAGE_TRANSPOSE:
            for(uint32_t x = 0; x < header->h; x++) {
                dst_index = x * buf_stride + line_index * JPEG_PIXEL_SIZE;
                lv_memcpy(cur_pos + dst_index, buffer + x * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
            }
            break;
        case IMAGE_TRANSVERSE:
            for(uint32_t x = 0; x < header->h; x++) {
                dst_index = (header->h - x - 1) * buf_stride + (header->w - line_index - 1) * JPEG_PIXEL_SIZE;
                lv_memcpy(cur_pos + dst_index, buffer + x * JPEG_PIXEL_SIZE, JPEG_PIXEL_SIZE);
            }
            break;
        default:
            lv_memcpy(cur_pos + line_index * buf_stride, buffer, header->stride);
            break;
    }
}

static image_orientation_t jpeg_markers_reader(struct jpeg_decompress_struct * cinfo)
{
    image_orientation_t res = IMAGE_CLOCKWISE_NONE;

    if(cinfo == NULL || cinfo->marker == NULL) {
        return res;
    }

    cinfo->marker->read_markers(cinfo);

    jpeg_saved_marker_ptr marker = cinfo->marker_list;
    while(marker != NULL) {
        if(marker->marker == APP1_MARKER) {
            JOCTET FAR * app1_data = marker->data;
            if(TRANS_32_VALUE(true, app1_data) == JPEG_EXIF) {
                uint16_t endian_tag = TRANS_16_VALUE(true, app1_data + 4 + 2);
                if(!(endian_tag == JPEG_LITTLE_ENDIAN_TAG || endian_tag == JPEG_BIG_ENDIAN_TAG)) {
                    return res;
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
                        return res;
                    }
                    ifd = app1_data + entry_offset;
                    unsigned short num_entries = TRANS_16_VALUE(is_big_endian, ifd - 2);
                    if(entry_offset + num_entries * 12 >= marker->data_length) {
                        return res;
                    }
                    for(int i = 0; i < num_entries; i++) {
                        unsigned short tag = TRANS_16_VALUE(is_big_endian, ifd);
                        if(tag == ORIENTATION_TAG) {
                            /* ifd entry: 12bytes = 2bytes(tag number) + 2bytes(kind of data) + 4bytes(number of components) + 4bytes(data)
                            * orientation kind(0x03) of data is unsigned short */
                            uint32_t dirc = TRANS_16_VALUE(is_big_endian, ifd + 2 + 2 + 4);
                            res = (dirc >= IMAGE_CLOCKWISE_0 && dirc <= IMAGE_CLOCKWISE_270) ? (image_orientation_t)dirc : IMAGE_CLOCKWISE_NONE;
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

    return res;
}

static void jpeg_cmyk_to_bgrx(uint8_t * cmyk_data, uint32_t pixel_count)
{
    uint8_t * ptr = cmyk_data;
    for(uint32_t i = 0; i < pixel_count; i++) {
        uint8_t c = 255 - ptr[0];
        uint8_t m = 255 - ptr[1];
        uint8_t y = 255 - ptr[2];
        uint8_t k = 255 - ptr[3];

        uint16_t inv_k = 255 - k;
        ptr[0] = (uint8_t)(((inv_k * (255 - y)) + 128) / 255);
        ptr[1] = (uint8_t)(((inv_k * (255 - m)) + 128) / 255);
        ptr[2] = (uint8_t)(((inv_k * (255 - c)) + 128) / 255);
        ptr[3] = 255;

        ptr += 4;
    }
}

static void error_exit(j_common_ptr cinfo)
{
    error_mgr_t * myerr = (error_mgr_t *)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->jb, 1);
}

#endif /*LV_USE_LIBJPEG_TURBO*/
