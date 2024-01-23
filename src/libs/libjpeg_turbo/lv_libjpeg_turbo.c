/**
 * @file lv_libjpeg_turbo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_LIBJPEG_TURBO

#include "lv_libjpeg_turbo.h"
#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>

/*********************
 *      DEFINES
 *********************/
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
static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_draw_buf_t * decode_jpeg_file(const char * filename);
static bool get_jpeg_size(const char * filename, uint32_t * width, uint32_t * height);
static void error_exit(j_common_ptr cinfo);
static void jpeg_decoder_cache_free_cb(lv_image_cache_data_t * cached_data, void * user_data);
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
 * Register the JPEG decoder functions in LVGL
 */
void lv_libjpeg_turbo_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);
    lv_image_decoder_set_cache_free_cb(dec, (lv_cache_free_cb_t)jpeg_decoder_cache_free_cb);
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
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Unused*/
    lv_image_src_t src_type = lv_image_src_get_type(src);          /*Get the source type*/

    /*If it's a JPEG file...*/
    if(src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = src;

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) {
            LV_LOG_WARN("Can't open file: %s", fn);
            return LV_RESULT_INVALID;
        }

        uint32_t jpg_signature = 0;
        uint32_t rn;
        lv_fs_read(&f, &jpg_signature, sizeof(jpg_signature), &rn);
        lv_fs_close(&f);

        if(rn != sizeof(jpg_signature)) {
            LV_LOG_WARN("file: %s signature len = %" LV_PRIu32 " error", fn, rn);
            return LV_RESULT_INVALID;
        }

        bool is_jpeg_ext = (lv_strcmp(lv_fs_get_ext(fn), "jpg") == 0)
                           || (lv_strcmp(lv_fs_get_ext(fn), "jpeg") == 0);

        if(!IS_JPEG_SIGNATURE(jpg_signature)) {
            if(is_jpeg_ext) {
                LV_LOG_WARN("file: %s signature = 0X%" LV_PRIX32 " error", fn, jpg_signature);
            }
            return LV_RESULT_INVALID;
        }

        uint32_t width;
        uint32_t height;

        if(!get_jpeg_size(fn, &width, &height)) {
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

        if(dsc->args.no_cache) return LV_RES_OK;

#if LV_CACHE_DEF_SIZE > 0
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
#endif
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

    if(dsc->args.no_cache || LV_CACHE_DEF_SIZE == 0)
        lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
    else
        lv_cache_release(dsc->cache, dsc->cache_entry, NULL);
}

static uint8_t * alloc_file(const char * filename, uint32_t * size)
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
    int row_stride;     /* physical row width in output buffer */

    lv_draw_buf_t * decoded = NULL;

    /* In this example we want to open the input file before doing anything else,
     * so that the setjmp() error recovery below can assume the file is open.
     * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
     * requires it in order to read binary files.
     */

    uint32_t data_size;
    uint8_t * data = alloc_file(filename, &data_size);
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

    decoded = lv_draw_buf_create(cinfo.output_width, cinfo.output_height, LV_COLOR_FORMAT_RGB888, LV_STRIDE_AUTO);
    if(decoded != NULL) {
        uint8_t * cur_pos = decoded->data;
        size_t stride = cinfo.output_width * JPEG_PIXEL_SIZE;

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
            lv_memcpy(cur_pos, buffer[0], stride);
            cur_pos += decoded->header.stride;
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

static bool get_jpeg_size(const char * filename, uint32_t * width, uint32_t * height)
{
    struct jpeg_decompress_struct cinfo;
    error_mgr_t jerr;

    uint8_t * data = NULL;
    uint32_t data_size;
    data = alloc_file(filename, &data_size);
    if(data == NULL) {
        return false;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit;

    if(setjmp(jerr.jb)) {
        LV_LOG_WARN("read jpeg head failed");
        jpeg_destroy_decompress(&cinfo);
        lv_free(data);
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

    lv_free(data);

    return (ret == JPEG_HEADER_OK);
}

static void error_exit(j_common_ptr cinfo)
{
    error_mgr_t * myerr = (error_mgr_t *)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->jb, 1);
}

static void jpeg_decoder_cache_free_cb(lv_image_cache_data_t * cached_data, void * user_data)
{
    LV_UNUSED(user_data);

    if(cached_data->src_type == LV_IMAGE_SRC_FILE) lv_free((void *)cached_data->src);
    lv_draw_buf_destroy((lv_draw_buf_t *)cached_data->decoded);
}

#endif /*LV_USE_LIBJPEG_TURBO*/
