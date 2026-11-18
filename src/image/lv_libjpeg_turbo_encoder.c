/**
 * @file lv_libjpeg_turbo_encoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lvgl_public.h"
#if LV_USE_LIBJPEG_TURBO_ENCODER

#include "lv_image_encoder_private.h"
#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    struct jpeg_error_mgr pub;
    jmp_buf jb;
} jpeg_error_mgr_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t jpeg_encoder_save(lv_image_encoder_t * encoder, const lv_draw_buf_t * draw_buf,
                                     const char * path, const lv_image_encoder_args_t * args);
static void jpeg_encoder_error_exit(j_common_ptr cinfo);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_libjpeg_turbo_encoder_init(void)
{
    lv_image_encoder_t * enc = lv_image_encoder_create();
    if(enc == NULL) return;
    lv_image_encoder_set_save_cb(enc, jpeg_encoder_save);
    lv_image_encoder_set_name(enc, "JPEG");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void jpeg_encoder_error_exit(j_common_ptr cinfo)
{
    jpeg_error_mgr_t * myerr = (jpeg_error_mgr_t *)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->jb, 1);
}

static lv_result_t jpeg_encoder_save(lv_image_encoder_t * encoder, const lv_draw_buf_t * draw_buf,
                                     const char * path, const lv_image_encoder_args_t * args)
{
    LV_UNUSED(encoder);
    LV_PROFILER_DECODER_BEGIN;

    const char * ext = lv_fs_get_ext(path);
    if(lv_strcmp(ext, "jpg") != 0 && lv_strcmp(ext, "jpeg") != 0) {
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    struct jpeg_compress_struct cinfo;
    jpeg_error_mgr_t jerr;
    unsigned char * outbuf = NULL;
    unsigned long outsize = 0;

    /*From here on RGB565 is encoded as the converted RGB888 copy*/
    lv_draw_buf_t * converted = NULL;
    if(draw_buf->header.cf == LV_COLOR_FORMAT_RGB565) {
        converted = lv_draw_buf_create(draw_buf->header.w, draw_buf->header.h, LV_COLOR_FORMAT_RGB888, LV_STRIDE_AUTO);
        if(converted == NULL || lv_draw_buf_convert_format(converted, draw_buf) != LV_RESULT_OK) {
            LV_LOG_ERROR("convert RGB565 to RGB888 failed");
            lv_draw_buf_destroy(converted);
            LV_PROFILER_DECODER_END;
            return LV_RESULT_INVALID;
        }
        draw_buf = converted;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = jpeg_encoder_error_exit;
    if(setjmp(jerr.jb)) {
        jpeg_destroy_compress(&cinfo);
        if(outbuf) free(outbuf); /*allocated by libjpeg via standard malloc*/
        lv_draw_buf_destroy(converted);
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    jpeg_create_compress(&cinfo);
    jpeg_mem_dest(&cinfo, &outbuf, &outsize);

    cinfo.image_width  = draw_buf->header.w;
    cinfo.image_height = draw_buf->header.h;

    switch(draw_buf->header.cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            cinfo.input_components = 4;
            cinfo.in_color_space = JCS_EXT_BGRA;
            break;
        case LV_COLOR_FORMAT_RGB888:
            cinfo.input_components = 3;
            cinfo.in_color_space = JCS_EXT_BGR;
            break;
        default:
            LV_LOG_WARN("unsupported color format %d", draw_buf->header.cf);
            jpeg_destroy_compress(&cinfo);
            if(outbuf) free(outbuf); /*allocated by libjpeg via standard malloc*/
            LV_PROFILER_DECODER_END;
            return LV_RESULT_INVALID;
    }

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, args && args->quality > 0 ? args->quality : 85, TRUE);

    LV_PROFILER_DECODER_BEGIN_TAG("jpeg_compress");
    jpeg_start_compress(&cinfo, TRUE);

    /*Optional EXIF orientation: a minimal APP1 segment with only the Orientation tag (0x0112).
     *args->rotation is the clockwise rotation of the content, the EXIF code is picked here
     *so the encoder args stay independent of the format.*/
    uint16_t exif_orientation = 0; /*0: no marker*/
    if(args) {
        switch(args->rotation) {
            case 0:
                break;
            case 90:
                exif_orientation = 6;
                break;
            case 180:
                exif_orientation = 3;
                break;
            case 270:
                exif_orientation = 8;
                break;
            default:
                LV_LOG_WARN("unsupported rotation %d, writing no orientation marker", (int)args->rotation);
                break;
        }
    }

    if(exif_orientation > 0) {
        const uint8_t exif[] = {
            'E', 'x', 'i', 'f', 0x00, 0x00,                     /*EXIF header*/
            0x49, 0x49, 0x2A, 0x00, 0x08, 0x00, 0x00, 0x00,     /*TIFF, little-endian, IFD0 at offset 8*/
            0x01, 0x00,                                         /*IFD0: 1 entry*/
            0x12, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,     /*tag 0x0112, SHORT, count 1*/
            (uint8_t)(exif_orientation & 0xFF), (uint8_t)(exif_orientation >> 8), 0x00, 0x00, /*value + padding*/
            0x00, 0x00, 0x00, 0x00,                             /*no next IFD*/
        };
        jpeg_write_marker(&cinfo, JPEG_APP0 + 1, exif, sizeof(exif));
    }

    while(cinfo.next_scanline < cinfo.image_height) {
        uint8_t * row = draw_buf->data + (size_t)cinfo.next_scanline * draw_buf->header.stride;
        jpeg_write_scanlines(&cinfo, &row, 1);
    }

    jpeg_finish_compress(&cinfo);
    LV_PROFILER_DECODER_END_TAG("jpeg_compress");
    jpeg_destroy_compress(&cinfo);
    lv_draw_buf_destroy(converted);

    /*Write to file via lv_fs*/
    lv_fs_file_t file;
    lv_fs_res_t fs_res = lv_fs_open(&file, path, LV_FS_MODE_WR | LV_FS_MODE_TRUNC);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_ERROR("create file %s failed, res %d", path, (int)fs_res);
        free(outbuf); /*allocated by libjpeg via standard malloc*/
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    uint32_t bw;
    fs_res = lv_fs_write(&file, outbuf, outsize, &bw);
    lv_fs_close(&file);
    free(outbuf); /*allocated by libjpeg via standard malloc*/

    if(fs_res != LV_FS_RES_OK || bw != (uint32_t)outsize) {
        LV_LOG_ERROR("write file %s failed, res %d, bw %d", path, (int)fs_res, (int)bw);
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    LV_PROFILER_DECODER_END;
    return LV_RESULT_OK;
}

#endif /*LV_USE_LIBJPEG_TURBO_ENCODER*/
