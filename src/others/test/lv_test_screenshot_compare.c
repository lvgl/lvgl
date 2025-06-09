/**
 * @file lv_test_assert.c
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
#include "../../lv_conf_internal.h"
#if LV_USE_TEST && defined(LV_USE_TEST_SCREENSHOT_COMPARE) && LV_USE_TEST_SCREENSHOT_COMPARE

#if LV_USE_LODEPNG == 0
    #error "lodepng is required for screenshot compare. Enable it in lv_conf.h (LV_USE_LODEPNG 1)"
#endif

#include "../../lvgl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
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
static bool screenshot_compare(const char * fn_ref, uint8_t tolerance);
static unsigned  read_png_file(lv_draw_buf_t ** refr_draw_buf, unsigned * width, unsigned * height,
                               const char * file_name);
static unsigned  write_png_file(void * raw_img, uint32_t width, uint32_t height, char * file_name);
static void buf_to_xrgb8888(const lv_draw_buf_t * draw_buf, uint8_t * buf_out);
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

bool lv_test_screenshot_compare(const char * fn_ref)
{
    bool pass;

    lv_obj_t * scr = lv_screen_active();
    lv_obj_invalidate(scr);

    pass = screenshot_compare(fn_ref, REF_IMG_TOLERANCE);
    if(!pass) return false;

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Compare the content of the frame buffer with a reference image
 * @param fn_ref    reference image path
 * @return          true: test passed; false: test failed
 */
static bool screenshot_compare(const char * fn_ref, uint8_t tolerance)
{
    char fn_ref_full[256];
    lv_snprintf(fn_ref_full, sizeof(fn_ref_full), "%s%s", REF_IMGS_PATH, fn_ref);

    create_folders_if_needed(fn_ref_full);

    lv_refr_now(NULL);

    lv_draw_buf_t * draw_buf = lv_display_get_buf_active(NULL);
    uint8_t * screen_buf_xrgb8888 = lv_malloc(draw_buf->header.w * draw_buf->header.h * 4);
    buf_to_xrgb8888(draw_buf, screen_buf_xrgb8888);

    lv_draw_buf_t * ref_draw_buf;
    unsigned ref_img_width = 0;
    unsigned  ref_img_height = 0;
    unsigned  res = read_png_file(&ref_draw_buf, &ref_img_width, &ref_img_height, fn_ref_full);
    if(res) {
        LV_LOG_ERROR("%s%s", fn_ref_full, " was not found, creating is now from the rendered screen");
        write_png_file(screen_buf_xrgb8888, draw_buf->header.w, draw_buf->header.h, fn_ref_full);
        lv_free(screen_buf_xrgb8888);
        return true;
    }

    unsigned x, y;
    bool err = false;
    for(y = 0; y < ref_img_height; y++) {
        uint8_t * screen_buf_tmp = screen_buf_xrgb8888 + draw_buf->header.w * 4 * y;
        uint8_t * ref_row = (uint8_t *)ref_draw_buf->data + y * ref_draw_buf->header.stride;
        for(x = 0; x < ref_img_width; x++) {
            uint8_t * ptr_ref = &(ref_row[x * 4]);
            uint8_t * ptr_act = &screen_buf_tmp[x * 4];

            if(LV_ABS((int32_t) ptr_act[0] - (int32_t) ptr_ref[0]) > tolerance ||
               LV_ABS((int32_t) ptr_act[1] - (int32_t) ptr_ref[1]) > tolerance ||
               LV_ABS((int32_t) ptr_act[2] - (int32_t) ptr_ref[2]) > tolerance) {
                uint32_t act_px = (ptr_act[2] << 16) + (ptr_act[1] << 8) + (ptr_act[0] << 0);
                uint32_t ref_px = 0;
                memcpy(&ref_px, ptr_ref, 3);
                LV_LOG("\nScreenshot compare error\n"
                       "  - File: %s\n"
                       "  - At x:%d, y:%d.\n"
                       "  - Expected: %X\n"
                       "  - Actual:   %X\n"
                       "  - Tolerance: %d\n",
                       fn_ref_full,  x, y, ref_px, act_px, tolerance);
                err = true;
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

        write_png_file(screen_buf_xrgb8888, draw_buf->header.w, draw_buf->header.h, fn_err_full);
    }

    fflush(stdout);
    lv_free(screen_buf_xrgb8888);
    lv_draw_buf_destroy(ref_draw_buf);
    return !err;

}

static unsigned  read_png_file(lv_draw_buf_t ** refr_draw_buf, unsigned * width, unsigned * height,
                               const char * file_name)
{
    unsigned  error = lodepng_decode32_file((void *)refr_draw_buf, width, height, file_name);
    if(error) LV_LOG_WARN("error %u: %s\n", error, lodepng_error_text(error));
    return error;
}

static unsigned  write_png_file(void * raw_img, uint32_t width, uint32_t height, char * file_name)
{
    unsigned  error = lodepng_encode32_file(file_name, raw_img, width, height);
    if(error) LV_LOG_WARN("error %u: %s\n", error, lodepng_error_text(error));
    return error;
}

static void buf_to_xrgb8888(const lv_draw_buf_t * draw_buf, uint8_t * buf_out)
{
    uint32_t stride = draw_buf->header.stride;
    lv_color_format_t cf_in = draw_buf->header.cf;
    const uint8_t * buf_in = draw_buf->data;

    if(cf_in == LV_COLOR_FORMAT_RGB565 || cf_in == LV_COLOR_FORMAT_RGB565_SWAPPED) {
        if(cf_in == LV_COLOR_FORMAT_RGB565_SWAPPED) {
            lv_draw_sw_rgb565_swap(draw_buf->data, draw_buf->header.w * draw_buf->header.h);
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
                buf_out[x * 4 + 3] = buf_in[x * 4 + 3];
                buf_out[x * 4 + 2] = buf_in[x * 4 + 0];
                buf_out[x * 4 + 1] = buf_in[x * 4 + 1];
                buf_out[x * 4 + 0] = buf_in[x * 4 + 2];
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
    char * ptr;
    char * path_copy = lv_strdup(path);
    if(path_copy == NULL) {
        LV_LOG_ERROR("Error duplicating path");
        exit(EXIT_FAILURE);
    }

    char * token = strtok_r(path_copy, "/", &ptr);
    char current_path[1024] = {'\0'}; /* Adjust the size as needed */

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
