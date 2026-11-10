/**
 * @file lv_test_screenshot_compare.c
 *
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"

#if LV_USE_TEST && defined(LV_USE_TEST_SCREENSHOT_COMPARE) && LV_USE_TEST_SCREENSHOT_COMPARE

#if LV_USE_LODEPNG == 0
    #error "lodepng is required for screenshot compare. Enable it in lv_conf.h (LV_USE_LODEPNG 1)"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include LV_STDARG_INCLUDE
#include <errno.h>
#include "../../libs/lodepng/lodepng.h"

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(pathname, mode) _mkdir(pathname)
    #define strtok_r strtok_s
#else
    #include <sys/stat.h>
#endif

/*********************
 *      DEFINES
 *********************/

#ifndef REF_IMGS_PATH
    #define REF_IMGS_PATH ""
#endif

#ifndef REF_IMG_TOLERANCE
    #define REF_IMG_TOLERANCE 0
#endif

#define ERR_FILE_NOT_FOUND  -1
#define ERR_PNG             -2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static unsigned  read_png_file(lv_draw_buf_t ** refr_draw_buf, unsigned * width, unsigned * height,
                               const char * file_name);
static unsigned  write_png_file(void * raw_img, uint32_t width, uint32_t height, char * file_name);
static void write_diff_png_file(const uint8_t * act, uint32_t act_stride, const uint8_t * ref, uint32_t ref_stride,
                                uint32_t w, uint32_t h, const char * fn_ref_no_ext);
static void buf_to_rgba8888(const lv_draw_buf_t * draw_buf, uint8_t * buf_out);
static void create_folders_if_needed(const char * path) ;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_test_screenshot_result_t lv_test_screenshot_compare(const char * fn_ref)
{
    LV_CHECK_ARG(fn_ref != NULL, return LV_TEST_SCREENSHOT_RESULT_FAILED);

    lv_obj_t * scr = lv_screen_active();
    lv_obj_invalidate(scr);
    lv_refr_now(NULL);

    lv_test_screenshot_result_t res;
    res = lv_test_screenshot_compare_core(fn_ref);
    return res;
}

lv_test_screenshot_result_t lv_test_screenshot_compare_core(const char * fn_ref)
{
    return lv_test_screenshot_compare_core_with_tolerance(fn_ref, REF_IMG_TOLERANCE);
}

lv_test_screenshot_result_t lv_test_screenshot_compare_core_with_tolerance(const char * fn_ref, uint8_t tolerance)
{
    LV_CHECK_ARG(fn_ref != NULL, return LV_TEST_SCREENSHOT_RESULT_FAILED);

    char fn_ref_full[256];
    lv_snprintf(fn_ref_full, sizeof(fn_ref_full), "%s%s", REF_IMGS_PATH, fn_ref);

#if LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
    create_folders_if_needed(fn_ref_full);
#endif

    lv_draw_buf_t * draw_buf = lv_display_get_buf_active(lv_display_get_default());

    uint8_t * screen_buf_rgba8888 = lv_malloc(draw_buf->header.w * draw_buf->header.h * 4);
    if(!screen_buf_rgba8888) {
        LV_LOG_ERROR("Not enough memory to compare display with screenshot");
        return LV_TEST_SCREENSHOT_RESULT_FAILED;
    }

    buf_to_rgba8888(draw_buf, screen_buf_rgba8888);

    lv_draw_buf_t * ref_draw_buf = NULL;
    unsigned ref_img_width = 0;
    unsigned  ref_img_height = 0;
    unsigned  res = read_png_file(&ref_draw_buf, &ref_img_width, &ref_img_height, fn_ref_full);
    if(res) {
        lv_test_screenshot_result_t comp_res;
#if LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
        LV_LOG_WARN("%s%s", fn_ref_full, " was not found, creating it now from the rendered screen");

        if(write_png_file(screen_buf_rgba8888, draw_buf->header.w, draw_buf->header.h, fn_ref_full)) {
            LV_LOG_ERROR("Couldn't write the %s reference image", fn_ref_full);
            comp_res = LV_TEST_SCREENSHOT_RESULT_FAILED;
        }
        else {
            comp_res = LV_TEST_SCREENSHOT_RESULT_PASSED;
        }
#else
        comp_res = LV_TEST_SCREENSHOT_RESULT_NO_REFERENCE_IMAGE;
#endif
        lv_free(screen_buf_rgba8888);
        if(ref_draw_buf) lv_draw_buf_destroy(ref_draw_buf);
        return comp_res;
    }

    if(ref_img_width != draw_buf->header.w || ref_img_height != draw_buf->header.h) {
        LV_LOG_WARN("The dimensions of the rendered and the %s reference image don't match", fn_ref);
        lv_free(screen_buf_rgba8888);
        if(ref_draw_buf) lv_draw_buf_destroy(ref_draw_buf);
        return LV_TEST_SCREENSHOT_RESULT_FAILED;
    }


    unsigned x, y;
    bool err = false;
    for(y = 0; y < ref_img_height; y++) {
        uint8_t * screen_buf_tmp = screen_buf_rgba8888 + draw_buf->header.w * 4 * y;
        uint8_t * ref_row = (uint8_t *)ref_draw_buf->data + y * ref_draw_buf->header.stride;
        for(x = 0; x < ref_img_width; x++) {
            uint8_t * ptr_ref = &(ref_row[x * 4]);
            uint8_t * ptr_act = &screen_buf_tmp[x * 4];

            /* Compared in premultiplied alpha: a pixel counts as much as it is visible */
            bool differs = LV_ABS((int32_t) ptr_act[3] - (int32_t) ptr_ref[3]) > tolerance;
            for(uint32_t c = 0; c < 3 && !differs; c++) {
                int32_t act_premul = LV_UDIV255((uint32_t)ptr_act[c] * ptr_act[3]);
                int32_t ref_premul = LV_UDIV255((uint32_t)ptr_ref[c] * ptr_ref[3]);
                differs = LV_ABS(act_premul - ref_premul) > tolerance;
            }
            if(differs) {
                err = true;
#if LV_USE_LOG
                LV_LOG("\nScreenshot compare error\n"
                       "  - File: %s\n"
                       "  - At x:%d, y:%d.\n"
                       "  - Expected: %02X%02X%02X/%02X\n"
                       "  - Actual:   %02X%02X%02X/%02X\n"
                       "  - Tolerance: %d\n",
                       fn_ref_full,  x, y,
                       ptr_ref[0], ptr_ref[1], ptr_ref[2], ptr_ref[3],
                       ptr_act[0], ptr_act[1], ptr_act[2], ptr_act[3], tolerance);
#endif
                break;
            }
        }
        if(err) break;
    }

    if(err) {
        char fn_ref_no_ext[128];
        lv_strlcpy(fn_ref_no_ext, fn_ref, sizeof(fn_ref_no_ext));
        fn_ref_no_ext[strlen(fn_ref_no_ext) - 4] = '\0';

        char fn_err_full[256];
        lv_snprintf(fn_err_full, sizeof(fn_err_full), "%s%s_err.png", REF_IMGS_PATH, fn_ref_no_ext);

        write_png_file(screen_buf_rgba8888, draw_buf->header.w, draw_buf->header.h, fn_err_full);

        write_diff_png_file(screen_buf_rgba8888, draw_buf->header.w * 4, ref_draw_buf->data, ref_draw_buf->header.stride,
                            draw_buf->header.w, draw_buf->header.h, fn_ref_no_ext);
    }

    fflush(stdout);
    lv_free(screen_buf_rgba8888);
    if(ref_draw_buf) lv_draw_buf_destroy(ref_draw_buf);
    return err ? LV_TEST_SCREENSHOT_RESULT_FAILED : LV_TEST_SCREENSHOT_RESULT_PASSED;

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static unsigned  read_png_file(lv_draw_buf_t ** refr_draw_buf, unsigned * width, unsigned * height,
                               const char * file_name)
{
    LV_ASSERT(refr_draw_buf != NULL);
    LV_ASSERT(width != NULL);
    LV_ASSERT(height != NULL);
    LV_ASSERT(file_name != NULL);
    unsigned  error = lodepng_decode32_file((void *)refr_draw_buf, width, height, file_name);
    if(error) LV_LOG_WARN("error %u: %s\n", error, lodepng_error_text(error));
    return error;
}

static unsigned  write_png_file(void * raw_img, uint32_t width, uint32_t height, char * file_name)
{
    LV_ASSERT(raw_img != NULL);
    LV_ASSERT(file_name != NULL);
    unsigned  error = lodepng_encode32_file(file_name, raw_img, width, height);
    if(error) LV_LOG_WARN("error %u: %s\n", error, lodepng_error_text(error));
    return error;
}

/**
 * Write <name>_diff.png next to the reference: the absolute difference of the rendered and the
 * reference image, channel by channel, as an opaque image. A difference in alpha is added to all
 * three color channels, so where only alpha differs the diff is grey.
 */
static void write_diff_png_file(const uint8_t * act, uint32_t act_stride, const uint8_t * ref, uint32_t ref_stride,
                                uint32_t w, uint32_t h, const char * fn_ref_no_ext)
{
    uint8_t * diff = lv_malloc(w * h * 4);
    if(!diff) {
        LV_LOG_WARN("Not enough memory for the diff image of %s", fn_ref_no_ext);
        return;
    }

    for(uint32_t y = 0; y < h; y++) {
        const uint8_t * act_row = act + y * act_stride;
        const uint8_t * ref_row = ref + y * ref_stride;
        uint8_t * out = diff + y * w * 4;
        for(uint32_t x = 0; x < w * 4; x += 4) {
            int32_t da = LV_ABS((int32_t)act_row[x + 3] - (int32_t)ref_row[x + 3]);
            for(uint32_t c = 0; c < 3; c++) {
                int32_t d = LV_ABS((int32_t)act_row[x + c] - (int32_t)ref_row[x + c]) + da;
                out[x + c] = (uint8_t)LV_MIN(d, 255);
            }
            out[x + 3] = 0xff;
        }
    }

    char fn_diff_full[256];
    lv_snprintf(fn_diff_full, sizeof(fn_diff_full), "%s%s_diff.png", REF_IMGS_PATH, fn_ref_no_ext);
    write_png_file(diff, w, h, fn_diff_full);
    lv_free(diff);
}

static void buf_to_rgba8888(const lv_draw_buf_t * draw_buf, uint8_t * buf_out)
{
    LV_ASSERT(draw_buf != NULL);
    LV_ASSERT(buf_out != NULL);
    uint32_t stride = draw_buf->header.stride;
    lv_color_format_t cf_in = draw_buf->header.cf;
    const uint8_t * buf_in = draw_buf->data;

    if(cf_in == LV_COLOR_FORMAT_RGB565 || cf_in == LV_COLOR_FORMAT_RGB565_SWAPPED) {
        if(cf_in == LV_COLOR_FORMAT_RGB565_SWAPPED) {
            lv_draw_rgb565_swap(draw_buf->data, draw_buf->header.w * draw_buf->header.h);
        }
        uint32_t y;
        for(y = 0; y < draw_buf->header.h; y++) {

            uint32_t x;
            for(x = 0; x < draw_buf->header.w; x++) {
                const lv_color16_t * c16 = (const lv_color16_t *)&buf_in[x * 2];

                buf_out[x * 4 + 3] = 0xff;
                buf_out[x * 4 + 2] = (c16->blue * 2106) >> 8;  /*To make it rounded*/
                buf_out[x * 4 + 1] = (c16->green * 1037) >> 8;
                buf_out[x * 4 + 0] = (c16->red * 2106) >> 8;
            }

            buf_in += stride;
            buf_out += draw_buf->header.w * 4;
        }
    }
    else if(cf_in == LV_COLOR_FORMAT_ARGB8888 || cf_in == LV_COLOR_FORMAT_XRGB8888 ||
            cf_in == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED) {
        uint32_t y;
        for(y = 0; y < draw_buf->header.h; y++) {
            uint32_t x;
            for(x = 0; x < draw_buf->header.w; x++) {
                uint8_t a = cf_in == LV_COLOR_FORMAT_XRGB8888 ? 0xff : buf_in[x * 4 + 3];
                uint8_t r = buf_in[x * 4 + 2];
                uint8_t g = buf_in[x * 4 + 1];
                uint8_t b = buf_in[x * 4 + 0];
                /* PNG holds straight alpha */
                if(cf_in == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED && a != 0 && a != 0xff) {
                    r = (uint8_t)LV_MIN(255, (r * 255 + a / 2) / a);
                    g = (uint8_t)LV_MIN(255, (g * 255 + a / 2) / a);
                    b = (uint8_t)LV_MIN(255, (b * 255 + a / 2) / a);
                }
                buf_out[x * 4 + 3] = a;
                buf_out[x * 4 + 2] = b;
                buf_out[x * 4 + 1] = g;
                buf_out[x * 4 + 0] = r;
            }

            buf_in += stride;
            buf_out += draw_buf->header.w * 4;
        }
    }
    else if(cf_in == LV_COLOR_FORMAT_RGB888) {
        uint32_t y;
        for(y = 0; y < draw_buf->header.h; y++) {
            uint32_t x;
            for(x = 0; x < draw_buf->header.w; x++) {
                buf_out[x * 4 + 3] = 0xff;
                buf_out[x * 4 + 2] = buf_in[x * 3 + 0];
                buf_out[x * 4 + 1] = buf_in[x * 3 + 1];
                buf_out[x * 4 + 0] = buf_in[x * 3 + 2];
            }

            buf_in += stride;
            buf_out += draw_buf->header.w * 4;
        }
    }
    else if(cf_in == LV_COLOR_FORMAT_L8) {
        uint32_t y;
        for(y = 0; y < draw_buf->header.h; y++) {
            uint32_t x;
            for(x = 0; x < draw_buf->header.w; x++) {
                buf_out[x * 4 + 3] = 0xff;
                buf_out[x * 4 + 2] = buf_in[x];
                buf_out[x * 4 + 1] = buf_in[x];
                buf_out[x * 4 + 0] = buf_in[x];
            }

            buf_in += stride;
            buf_out += draw_buf->header.w * 4;
        }
    }
    else if(cf_in == LV_COLOR_FORMAT_AL88) {
        uint32_t y;
        for(y = 0; y < draw_buf->header.h; y++) {
            uint32_t x;
            for(x = 0; x < draw_buf->header.w; x++) {
                buf_out[x * 4 + 3] = buf_in[x * 2 + 1];
                buf_out[x * 4 + 2] = buf_in[x * 2 + 0];
                buf_out[x * 4 + 1] = buf_in[x * 2 + 0];
                buf_out[x * 4 + 0] = buf_in[x * 2 + 0];
            }

            buf_in += stride;
            buf_out += draw_buf->header.w * 4;
        }
    }
    else if(cf_in == LV_COLOR_FORMAT_I1) {
        buf_in += 8;
        uint32_t y;
        for(y = 0; y < draw_buf->header.h; y++) {
            uint32_t x;
            for(x = 0; x < draw_buf->header.w; x++) {
                const uint8_t byte = buf_in[x / 8] ;
                const uint8_t bit_pos = x % 8;
                const uint8_t pixel = (byte >> (7 - bit_pos)) & 0x01;

                buf_out[x * 4 + 3] = 0xff;
                buf_out[x * 4 + 2] = pixel ? 0xff : 0x00;
                buf_out[x * 4 + 1] = pixel ? 0xff : 0x00;
                buf_out[x * 4 + 0] = pixel ? 0xff : 0x00;
            }

            buf_in += stride;
            buf_out += draw_buf->header.w * 4;
        }
    }
}

static void create_folders_if_needed(const char * path)
{
    LV_ASSERT(path != NULL);
    char * ptr;
    char * path_copy = lv_strdup(path);
    if(path_copy == NULL) {
        LV_LOG_ERROR("Error duplicating path");
        exit(EXIT_FAILURE);
    }

    char * token = strtok_r(path_copy, "/", &ptr);
    char current_path[1024] = {'\0'}; /* Adjust the size as needed */

    /*leading '/' means an absolute path, keep it */
    if(path[0] == '/') {
        lv_strcat(current_path, "/");
    }

    while(token && ptr && *ptr != '\0') {
        lv_strcat(current_path, token);
        lv_strcat(current_path, "/");

        int mkdir_retval = mkdir(current_path, 0777);
        if(mkdir_retval == 0) {
            LV_LOG_INFO("Created folder: %s\n", current_path);
        }
        else if(errno != EEXIST) {
            perror("Error creating folder");
            lv_free(path_copy);
            exit(EXIT_FAILURE);
        }

        token = strtok_r(NULL, "/", &ptr);
    }

    lv_free(path_copy);
}

#endif
