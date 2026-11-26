/**
 * @file lv_libpng_encoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lvgl_public.h"
#if LV_USE_LIBPNG_ENCODER

#include "lv_image_encoder_private.h"
#include <png.h>

/*********************
 *      DEFINES
 *********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t png_encoder_save(lv_image_encoder_t * encoder, const lv_draw_buf_t * draw_buf,
                                    const char * path, const lv_image_encoder_args_t * args);
static void png_encoder_write_cb(png_structp png, png_bytep data, size_t len);
static void png_encoder_flush_cb(png_structp png);
static void png_encoder_error_cb(png_structp png, png_const_charp msg);
static void png_encoder_warning_cb(png_structp png, png_const_charp msg);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_libpng_encoder_init(void)
{
    lv_image_encoder_t * enc = lv_image_encoder_create();
    if(enc == NULL) return;

    lv_image_encoder_set_save_cb(enc, png_encoder_save);
    lv_image_encoder_set_name(enc, "PNG");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void png_encoder_write_cb(png_structp png, png_bytep data, size_t len)
{
    lv_fs_file_t * file = png_get_io_ptr(png);

    uint32_t bw = 0;
    lv_fs_res_t res = lv_fs_write(file, data, (uint32_t)len, &bw);
    if(res != LV_FS_RES_OK || bw != (uint32_t)len) {
        LV_LOG_ERROR("write failed, res %d, bw %d", (int)res, (int)bw);
        png_error(png, "write failed");
    }
}

static void png_encoder_flush_cb(png_structp png)
{
    LV_UNUSED(png);
}

static void png_encoder_error_cb(png_structp png, png_const_charp msg)
{
    LV_UNUSED(msg);
    LV_LOG_ERROR("libpng: %s", msg);
    png_longjmp(png, 1);
}

static void png_encoder_warning_cb(png_structp png, png_const_charp msg)
{
    LV_UNUSED(png);
    LV_UNUSED(msg);
    LV_LOG_WARN("libpng: %s", msg);
}

static lv_result_t png_encoder_save(lv_image_encoder_t * encoder, const lv_draw_buf_t * draw_buf,
                                    const char * path, const lv_image_encoder_args_t * args)
{
    LV_UNUSED(encoder);

    const char * ext = lv_fs_get_ext(path);
    if(lv_strcmp(ext, "png") != 0) return LV_RESULT_INVALID;

    lv_color_format_t cf = draw_buf->header.cf;
    int color_type;
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
            color_type = PNG_COLOR_TYPE_RGB_ALPHA;
            break;
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_RGB565:
            color_type = PNG_COLOR_TYPE_RGB;
            break;
        default:
            LV_LOG_WARN("unsupported color format %d", cf);
            return LV_RESULT_INVALID;
    }

    LV_PROFILER_DECODER_BEGIN;

    int level = args ? LV_CLAMP(0, args->compression_level, 9) : 6;
    lv_result_t ret = LV_RESULT_INVALID;

    /*From here on RGB565 is encoded as the converted RGB888 copy*/
    lv_draw_buf_t * converted = NULL;
    if(cf == LV_COLOR_FORMAT_RGB565) {
        converted = lv_draw_buf_create(draw_buf->header.w, draw_buf->header.h, LV_COLOR_FORMAT_RGB888, LV_STRIDE_AUTO);
        if(converted == NULL || lv_draw_buf_convert_format(converted, draw_buf) != LV_RESULT_OK) {
            LV_LOG_ERROR("convert RGB565 to RGB888 failed");
            lv_draw_buf_destroy(converted);
            LV_PROFILER_DECODER_END;
            return LV_RESULT_INVALID;
        }
        draw_buf = converted;
        cf = LV_COLOR_FORMAT_RGB888;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
                                              png_encoder_error_cb, png_encoder_warning_cb);
    png_infop info = png ? png_create_info_struct(png) : NULL;
    if(png == NULL || info == NULL) {
        LV_LOG_ERROR("create png write struct failed");
        png_destroy_write_struct(&png, &info);
        lv_draw_buf_destroy(converted);
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    lv_fs_file_t file;
    lv_fs_res_t fs_res = lv_fs_open(&file, path, LV_FS_MODE_WR | LV_FS_MODE_TRUNC);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_ERROR("create file %s failed, res %d", path, (int)fs_res);
        png_destroy_write_struct(&png, &info);
        lv_draw_buf_destroy(converted);
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    if(setjmp(png_jmpbuf(png)) == 0) {
        png_set_write_fn(png, &file, png_encoder_write_cb, png_encoder_flush_cb);
        png_set_compression_level(png, level);
        png_set_IHDR(png, info, draw_buf->header.w, draw_buf->header.h, 8, color_type,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        png_write_info(png, info);

        /*LVGL's 24/32 bit formats are stored as B, G, R[, A/X] in memory*/
        png_set_bgr(png);
        /*Drop the unused X byte instead of writing it as alpha*/
        if(cf == LV_COLOR_FORMAT_XRGB8888) png_set_filler(png, 0, PNG_FILLER_AFTER);

        LV_PROFILER_DECODER_BEGIN_TAG("png_write_rows");
        for(int32_t y = 0; y < draw_buf->header.h; y++) {
            png_write_row(png, draw_buf->data + (size_t)y * draw_buf->header.stride);
        }
        LV_PROFILER_DECODER_END_TAG("png_write_rows");

        png_write_end(png, NULL);
        ret = LV_RESULT_OK;
    }

    png_destroy_write_struct(&png, &info);
    lv_draw_buf_destroy(converted);

    fs_res = lv_fs_close(&file);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_ERROR("close file %s failed, res %d", path, (int)fs_res);
        ret = LV_RESULT_INVALID;
    }

    LV_PROFILER_DECODER_END;
    return ret;
}

#endif /*LV_USE_LIBPNG_ENCODER*/
