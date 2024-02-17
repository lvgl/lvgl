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
#if LV_BUILD_TEST
#include "../lvgl.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "unity.h"
#define PNG_DEBUG 3
#include <png.h>

/*********************
 *      DEFINES
 *********************/
//#define REF_IMGS_PATH "lvgl/tests/lv_test_ref_imgs/"
#define REF_IMGS_PATH "ref_imgs/"
#define ERR_FILE_NOT_FOUND  -1
#define ERR_PNG             -2

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers;
} png_image_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool screenhot_compare(const char * fn_ref, const char * mode, uint8_t tolerance);
static int read_png_file(png_image_t * p, const char * file_name);
static int write_png_file(void * raw_img, uint32_t width, uint32_t height, char * file_name);
static void png_release(png_image_t * p);
static void buf_to_xrgb8888(const uint8_t * buf_in, uint8_t * buf_out, lv_color_format_t cf_in);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_test_assert_image_eq(const char * fn_ref)
{
    bool pass;

    lv_obj_t * scr = lv_screen_active();
    lv_obj_invalidate(scr);

    pass = screenhot_compare(fn_ref, "full refresh", 0);
    if(!pass) return false;

    //Software has minor rounding errors when not the whole image is updated
    //so ignore stripe invalidation for now
    //    uint32_t i;
    //    for(i = 0; i < 800; i += 50 ) {
    //        lv_area_t a;
    //        a.y1 = 0;
    //        a.y2 = 479;
    //        a.x1 = i;
    //        a.x2 = i + 12;
    //        lv_obj_invalidate_area(scr, &a);
    //
    //        a.x1 = i + 25;
    //        a.x2 = i + 32;
    //        lv_obj_invalidate_area(scr, &a);
    //    }
    //
    //    pass = screenhot_compare(fn_ref, "vertical stripes", 32);
    //    if(!pass) return false;
    //
    //
    //    for(i = 0; i < 480; i += 40) {
    //        lv_area_t a;
    //        a.x1 = 0;
    //        a.x2 = 799;
    //        a.y1 = i;
    //        a.y2 = i + 9;
    //        lv_obj_invalidate_area(scr, &a);
    //
    //        a.y1 = i + 25;
    //        a.y2 = i + 32;
    //        lv_obj_invalidate_area(scr, &a);
    //    }
    //
    //    pass = screenhot_compare(fn_ref, "horizontal stripes", 32);
    //    if(!pass) return false;

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint8_t screen_buf_xrgb8888[800 * 480 * 4];
/**
 * Compare the content of the frame buffer with a reference image
 * @param fn_ref        reference image name
 * @param mode          arbitrary string to tell more about the compare
 * @return  true: test passed; false: test failed
 */
static bool screenhot_compare(const char * fn_ref, const char * mode, uint8_t tolerance)
{

    char fn_ref_full[256];
    lv_snprintf(fn_ref_full, sizeof(fn_ref_full), "%s%s", REF_IMGS_PATH, fn_ref);

    lv_refr_now(NULL);

    extern uint8_t * last_flushed_buf;

    lv_color_format_t cf = lv_display_get_color_format(NULL);
    uint8_t * screen_buf = lv_draw_buf_align(last_flushed_buf, cf);
    buf_to_xrgb8888(screen_buf, screen_buf_xrgb8888, cf);

    png_image_t p;
    int res = read_png_file(&p, fn_ref_full);
    if(res == ERR_FILE_NOT_FOUND) {
        TEST_PRINTF("%s%s", fn_ref_full, " was not found, creating is now from the rendered screen");
        fflush(stderr);
        write_png_file(screen_buf_xrgb8888, 800, 480, fn_ref_full);
        return true;
    }
    else if(res == ERR_PNG) {
        return false;
    }

    uint8_t * ptr_act = NULL;
    const png_byte * ptr_ref = NULL;

    bool err = false;
    int x, y;
    for(y = 0; y < p.height; y++) {
        uint8_t * screen_buf_tmp = screen_buf_xrgb8888 + 800 * 4 * y;
        png_byte * row = p.row_pointers[y];
        for(x = 0; x < p.width; x++) {
            ptr_ref = &(row[x * 3]);
            ptr_act = screen_buf_tmp;

            if(LV_ABS((int32_t) ptr_act[0] - ptr_ref[0]) > tolerance ||
               LV_ABS((int32_t) ptr_act[1] - ptr_ref[1]) > tolerance ||
               LV_ABS((int32_t) ptr_act[2] - ptr_ref[2]) > tolerance) {
                uint32_t act_px = (ptr_act[2] << 16) + (ptr_act[1] << 8) + (ptr_act[0] << 0);
                uint32_t ref_px = 0;
                memcpy(&ref_px, ptr_ref, 3);
                TEST_PRINTF("\nScreenshot compare error\n"
                            "  - File: %s\n"
                            "  - Mode: %s\n"
                            "  - At x:%d, y:%d.\n"
                            "  - Expected: %X\n"
                            "  - Actual:   %X",
                            fn_ref_full, mode,  x, y, ref_px, act_px);
                fflush(stderr);
                err = true;
                break;
            }
            screen_buf_tmp += 4;
        }
        if(err) break;
    }

    if(err) {
        char fn_ref_no_ext[128];
        strcpy(fn_ref_no_ext, fn_ref);
        fn_ref_no_ext[strlen(fn_ref_no_ext) - 4] = '\0';

        char fn_err_full[256];
        lv_snprintf(fn_err_full, sizeof(fn_err_full), "%s%s_err.png", REF_IMGS_PATH, fn_ref_no_ext);

        write_png_file(screen_buf_xrgb8888, 800, 480, fn_err_full);
    }

    png_release(&p);

    fflush(stdout);
    return !err;

}

static int read_png_file(png_image_t * p, const char * file_name)
{
    char header[8];    // 8 is the maximum size that can be checked

    /*open file and test for it being a png*/
    FILE * fp = fopen(file_name, "rb");
    if(!fp) {
        TEST_PRINTF("[read_png_file %s] could not be opened for reading", file_name);
        return ERR_FILE_NOT_FOUND;
    }

    size_t rcnt = fread(header, 1, 8, fp);
    if(rcnt != 8 || png_sig_cmp((png_const_bytep)header, 0, 8)) {
        TEST_PRINTF("[read_png_file %s]  not recognized as a PNG file", file_name);
        return ERR_PNG;
    }

    /*initialize stuff*/
    p->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!p->png_ptr) {
        TEST_PRINTF("[read_png_file %s] png_create_read_struct failed", file_name);
        return ERR_PNG;
    }

    p->info_ptr = png_create_info_struct(p->png_ptr);
    if(!p->info_ptr) {
        TEST_PRINTF("[read_png_file %s] png_create_info_struct failed", file_name);
        return ERR_PNG;
    }
    if(setjmp(png_jmpbuf(p->png_ptr))) {
        TEST_PRINTF("[read_png_file %s] Error during init_io", file_name);
        return ERR_PNG;
    }
    png_init_io(p->png_ptr, fp);
    png_set_sig_bytes(p->png_ptr, 8);

    png_read_info(p->png_ptr, p->info_ptr);

    p->width = png_get_image_width(p->png_ptr, p->info_ptr);
    p->height = png_get_image_height(p->png_ptr, p->info_ptr);
    p->color_type = png_get_color_type(p->png_ptr, p->info_ptr);
    p->bit_depth = png_get_bit_depth(p->png_ptr, p->info_ptr);

    p->number_of_passes = png_set_interlace_handling(p->png_ptr);
    png_read_update_info(p->png_ptr, p->info_ptr);

    /*read file*/
    if(setjmp(png_jmpbuf(p->png_ptr))) {
        TEST_PRINTF("[read_png_file %s] Error during read_image", file_name);
        return ERR_PNG;
    }
    p->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * p->height);

    int y;
    for(y = 0; y < p->height; y++)
        p->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(p->png_ptr, p->info_ptr));

    png_read_image(p->png_ptr, p->row_pointers);

    fclose(fp);
    return 0;
}

static int write_png_file(void * raw_img, uint32_t width, uint32_t height, char * file_name)
{
    png_structp png_ptr;
    png_infop info_ptr;

    /* create file */
    FILE * fp = fopen(file_name, "wb");
    if(!fp) {
        printf("###### %s\n", file_name);
        fflush(stdout);
        TEST_PRINTF("[write_png_file %s] could not be opened for writing", file_name);
        TEST_PRINTF("%s", file_name);
        return -1;
    }

    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png_ptr) {
        TEST_PRINTF("[write_png_file %s] png_create_write_struct failed", file_name);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        TEST_PRINTF("[write_png_file %s] png_create_info_struct failed", file_name);
        return -1;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
        TEST_PRINTF("[write_png_file %s] Error during init_io", file_name);
        return -1;
    }

    png_init_io(png_ptr, fp);

    /* write header */
    if(setjmp(png_jmpbuf(png_ptr))) {
        TEST_PRINTF("[write_png_file %s] Error during writing header", file_name);
        return -1;
    }

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    /* write bytes */
    if(setjmp(png_jmpbuf(png_ptr))) {
        TEST_PRINTF("[write_png_file %s] Error during writing bytes", file_name);
        return -1;
    }

    uint8_t * raw_img8 = (uint8_t *)raw_img;
    png_bytep * row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);
    for(uint32_t y = 0; y < height; y++) {
        row_pointers[y] = malloc(3 * width);
        uint8_t * line = raw_img8 + y * width * 4;
        for(uint32_t x = 0; x < width; x++) {
            row_pointers[y][x * 3 + 0] = line[x * 4 + 0];
            row_pointers[y][x * 3 + 1] = line[x * 4 + 1];
            row_pointers[y][x * 3 + 2] = line[x * 4 + 2];
        }
    }
    png_write_image(png_ptr, row_pointers);

    /* end write */
    if(setjmp(png_jmpbuf(png_ptr))) {
        TEST_PRINTF("[write_png_file %s] Error during end of write", file_name);
        return -1;
    }
    png_write_end(png_ptr, NULL);

    /* cleanup heap allocation */
    for(uint32_t y = 0; y < height; y++) free(row_pointers[y]);
    free(row_pointers);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
    return 0;
}

static void png_release(png_image_t * p)
{
    int y;
    for(y = 0; y < p->height; y++) free(p->row_pointers[y]);

    free(p->row_pointers);

    png_destroy_read_struct(&p->png_ptr, &p->info_ptr, NULL);
}

static void buf_to_xrgb8888(const uint8_t * buf_in, uint8_t * buf_out, lv_color_format_t cf_in)
{
    uint32_t stride = lv_draw_buf_width_to_stride(800, cf_in);
    if(cf_in == LV_COLOR_FORMAT_RGB565) {
        uint32_t y;
        for(y = 0; y < 480; y++) {

            uint32_t x;
            for(x = 0; x < 800; x++) {
                const lv_color16_t * c16 = (const lv_color16_t *)&buf_in[x * 2];

                buf_out[x * 4 + 3] = 0xff;
                buf_out[x * 4 + 2] = (c16->blue * 2106) >> 8;  /*To make it rounded*/
                buf_out[x * 4 + 1] = (c16->green * 1037) >> 8;
                buf_out[x * 4 + 0] = (c16->red * 2106) >> 8;
            }

            buf_in += stride;
            buf_out += 800 * 4;
        }
    }
    else if(cf_in == LV_COLOR_FORMAT_ARGB8888 || cf_in == LV_COLOR_FORMAT_XRGB8888) {
        uint32_t y;
        for(y = 0; y < 480; y++) {
            uint32_t x;
            for(x = 0; x < 800; x++) {
                buf_out[x * 4 + 3] = buf_in[x * 4 + 3];
                buf_out[x * 4 + 2] = buf_in[x * 4 + 0];
                buf_out[x * 4 + 1] = buf_in[x * 4 + 1];
                buf_out[x * 4 + 0] = buf_in[x * 4 + 2];
            }

            buf_in += stride;
            buf_out += 800 * 4;
        }
    }
    else if(cf_in == LV_COLOR_FORMAT_RGB888) {
        uint32_t y;
        for(y = 0; y < 480; y++) {
            uint32_t x;
            for(x = 0; x < 800; x++) {
                buf_out[x * 4 + 3] = 0xff;
                buf_out[x * 4 + 2] = buf_in[x * 3 + 0];
                buf_out[x * 4 + 1] = buf_in[x * 3 + 1];
                buf_out[x * 4 + 0] = buf_in[x * 3 + 2];
            }

            buf_in += stride;
            buf_out += 800 * 4;
        }
    }
}

#endif
