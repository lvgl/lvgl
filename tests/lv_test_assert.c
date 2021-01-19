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
#include "lv_test_assert.h"
#include "../lvgl.h"
#if LV_BUILD_TEST

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

/*********************
 *      DEFINES
 *********************/
//#define REF_IMGS_PATH "lvgl/tests/lv_test_ref_imgs/"
#define REF_IMGS_PATH "lv_test_ref_imgs/"

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
}png_img_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void read_png_file(png_img_t * p, const char* file_name);
//static void write_png_file(png_img_t * p, const char* file_name);
static void png_release(png_img_t * p);
//static void process_file(png_img_t * p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_test_print(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stdout, s, args);
    fprintf(stdout, "\n");
    va_end(args);
}

void lv_test_exit(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(1);
}

void lv_test_error(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

void lv_test_assert_true(int32_t expression, const char * s)
{
    if(!expression) {
        lv_test_error("   FAIL: %s. (Expected: not zero)", s, expression);
    } else {
        lv_test_print("   PASS: %s. (Expected: not zero)", s, expression);
    }
}

void lv_test_assert_int_eq(int32_t n_ref, int32_t n_act, const char * s)
{
    if(n_ref != n_act) {
        lv_test_error("   FAIL: %s. (Expected:  %d, Actual: %d)", s, n_ref, n_act);
    } else {
        lv_test_print("   PASS: %s. (Expected: %d)", s, n_ref);
    }
}

void lv_test_assert_int_gt(int32_t n_ref, int32_t n_act, const char * s)
{
    if(n_act <= n_ref) {
        lv_test_error("   FAIL: %s. (Expected:  > %d, Actual: %d)", s, n_ref, n_act);
    } else {
        lv_test_print("   PASS: %s. (Expected: > %d, , Actual: %d)", s, n_ref, n_act);
    }
}

void lv_test_assert_int_lt(int32_t n_ref, int32_t n_act, const char * s)
{
    if(n_act >= n_ref) {
        lv_test_error("   FAIL: %s. (Expected:  < %d, Actual: %d)", s, n_ref, n_act);
    } else {
        lv_test_print("   PASS: %s. (Expected: < %d, , Actual: %d)", s, n_ref, n_act);
    }
}

void lv_test_assert_str_eq(const char * s_ref, const char * s_act, const char * s)
{
    if(strcmp(s_ref, s_act) != 0) {
        lv_test_error("   FAIL: %s. (Expected:  %s, Actual: %s)", s, s_ref, s_act);
    } else {
        lv_test_print("   PASS: %s. (Expected: %s)", s, s_ref);
    }
}

void lv_test_assert_array_eq(const uint8_t *p_ref, const uint8_t *p_act, int32_t size, const char * s)
{
    if(memcmp(p_ref, p_act, size) != 0) {
        lv_test_error("   FAIL: %s. (Expected: all %d bytes should be equal)", s, size);
    } else {
        lv_test_print("   PASS: %s. (Expected: all %d bytes should be equal)", s, size);
    }
}

void lv_test_assert_ptr_eq(const void * p_ref, const void * p_act, const char * s)
{
    if(p_ref != p_act) {
        lv_test_error("   FAIL: %s. (Expected:  0x%lx, Actual: 0x%lx)", s, p_ref, p_act);
    } else {
        lv_test_print("   PASS: %s. (Expected: 0x%lx)", s, p_ref);
    }
}

void lv_test_assert_color_eq(lv_color_t c_ref, lv_color_t c_act, const char * s)
{
    if(c_ref.full != c_act.full) {
        lv_test_error("   FAIL: %s. (Expected:  R:%02x, G:%02x, B:%02x, Actual: R:%02x, G:%02x, B:%02x)",  s,
                c_ref.ch.red, c_ref.ch.green, c_ref.ch.blue,
                c_act.ch.red, c_act.ch.green, c_act.ch.blue);
    } else {
        lv_test_print("   PASS: %s. (Expected: R:%02x, G:%02x, B:%02x)", s,
                c_ref.ch.red, c_ref.ch.green, c_ref.ch.blue);
    }
}

void lv_test_assert_img_eq(const char * fn_ref, const char * s)
{
#if LV_COLOR_DEPTH != 32
    lv_test_print("   SKIP: Can't compare '%s' because LV_COLOR_DEPTH != 32", fn_ref);
    return;
#endif

#if LV_HOR_RES_MAX != 800 || LV_VER_RES_MAX != 480
    lv_test_print("   SKIP: Can't compare '%s' because the resolution needs to be 800x480 (LV_HOR_RES_MAX, LV_VER_RES_MAX)", fn_ref);
    return;
#endif

    char fn_ref_full[512];
    sprintf(fn_ref_full, "%s%s", REF_IMGS_PATH, fn_ref);

    png_img_t p;
    read_png_file(&p, fn_ref_full);
    uint8_t * screen_buf;

    lv_disp_t * disp = lv_disp_get_default();
    lv_obj_invalidate(lv_disp_get_scr_act(disp));
    lv_refr_now(disp);

    extern lv_color_t test_fb[];

    screen_buf = (uint8_t *)test_fb;

    uint8_t * ptr_act = NULL;
    const png_byte* ptr_ref = NULL;

    bool err = false;
    int x, y, i_buf = 0;
    for (y=0; y<p.height; y++) {
        png_byte* row = p.row_pointers[y];
        for (x=0; x<p.width; x++) {
            ptr_ref = &(row[x*3]);
            ptr_act = &(screen_buf[i_buf*4]);
            uint8_t tmp = ptr_act[0];
            ptr_act[0] = ptr_act[2];
            ptr_act[2] = tmp;

            if(memcmp(ptr_act, ptr_ref, 3) != 0) {
                err = true;
                break;
            }
            i_buf++;
        }
        if(err) break;
    }

    png_release(&p);

    if(err) {
        uint32_t ref_px = 0;
        uint32_t act_px = 0;
        memcpy(&ref_px, ptr_ref, 3);
        memcpy(&act_px, ptr_act, 3);
        lv_test_error("   FAIL: %s. (Expected:  %s, diff. at (%d;%d), %08x instead of %08x)", s, fn_ref, x, y, act_px, ref_px);
    } else {
        lv_test_print("   PASS: %s. (Expected: %s)", s, fn_ref);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void read_png_file(png_img_t * p, const char* file_name)
{
    char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
        lv_test_exit("[read_png_file] File %s could not be opened for reading", file_name);
    size_t rcnt = fread(header, 1, 8, fp);
    if (rcnt != 8 || png_sig_cmp((png_const_bytep)header, 0, 8))
        lv_test_exit("[read_png_file] File %s is not recognized as a PNG file", file_name);

    /* initialize stuff */
    p->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!p->png_ptr)
        lv_test_exit("[read_png_file] png_create_read_struct failed");

    p->info_ptr = png_create_info_struct(p->png_ptr);
    if (!p->info_ptr)
        lv_test_exit("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(p->png_ptr)))
        lv_test_exit("[read_png_file] Error during init_io");

    png_init_io(p->png_ptr, fp);
    png_set_sig_bytes(p->png_ptr, 8);

    png_read_info(p->png_ptr, p->info_ptr);

    p->width = png_get_image_width(p->png_ptr, p->info_ptr);
    p->height = png_get_image_height(p->png_ptr, p->info_ptr);
    p->color_type = png_get_color_type(p->png_ptr, p->info_ptr);
    p->bit_depth = png_get_bit_depth(p->png_ptr, p->info_ptr);

    p->number_of_passes = png_set_interlace_handling(p->png_ptr);
    png_read_update_info(p->png_ptr, p->info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(p->png_ptr)))
        lv_test_exit("[read_png_file] Error during read_image");

    p->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * p->height);

    int y;
    for (y=0; y<p->height; y++)
        p->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(p->png_ptr,p->info_ptr));

    png_read_image(p->png_ptr, p->row_pointers);

    fclose(fp);
}
//
//
//static void write_png_file(png_img_t * p, const char* file_name)
//{
//    /* create file */
//    FILE *fp = fopen(file_name, "wb");
//    if (!fp)
//        lv_test_exit("[write_png_file] File %s could not be opened for writing", file_name);
//
//
//    /* initialize stuff */
//    p->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//
//    if (!p->png_ptr)
//        lv_test_exit("[write_png_file] png_create_write_struct failed");
//
//    p->info_ptr = png_create_info_struct(p->png_ptr);
//    if (!p->info_ptr)
//        lv_test_exit("[write_png_file] png_create_info_struct failed");
//
//    if (setjmp(png_jmpbuf(p->png_ptr)))
//        lv_test_exit("[write_png_file] Error during init_io");
//
//    png_init_io(p->png_ptr, fp);
//
//
//    /* write header */
//    if (setjmp(png_jmpbuf(p->png_ptr)))
//        lv_test_exit("[write_png_file] Error during writing header");
//
//    png_set_IHDR(p->png_ptr, p->info_ptr, p->width, p->height,
//            p->bit_depth, p->color_type, PNG_INTERLACE_NONE,
//            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
//
//    png_write_info(p->png_ptr, p->info_ptr);
//
//
//    /* write bytes */
//    if (setjmp(png_jmpbuf(p->png_ptr)))
//        lv_test_exit("[write_png_file] Error during writing bytes");
//
//    png_write_image(p->png_ptr, p->row_pointers);
//
//
//    /* end write */
//    if (setjmp(png_jmpbuf(p->png_ptr)))
//        lv_test_exit("[write_png_file] Error during end of write");
//
//    png_write_end(p->png_ptr, NULL);
//
//    fclose(fp);
//}
//
static void png_release(png_img_t * p)
{
    int y;
      for (y=0; y<p->height; y++)
          free(p->row_pointers[y]);
      free(p->row_pointers);
}

//static void process_file(png_img_t * p)
//{
//    if (png_get_color_type(p->png_ptr, p->info_ptr) == PNG_COLOR_TYPE_RGB)
//        lv_test_exit("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
//                "(lacks the alpha channel)");
//
//    if (png_get_color_type(p->png_ptr, p->info_ptr) != PNG_COLOR_TYPE_RGBA)
//        lv_test_exit("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
//                PNG_COLOR_TYPE_RGBA, png_get_color_type(p->png_ptr, p->info_ptr));
//
//    int x, y;
//    for (y=0; y<p->height; y++) {
//        png_byte* row = p->row_pointers[y];
//        for (x=0; x<p->width; x++) {
//            png_byte* ptr = &(row[x*4]);
//            printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
//                    x, y, ptr[0], ptr[1], ptr[2], ptr[3]);
//
//            /* set red value to 0 and green value to the blue one */
//            ptr[0] = 0;
//            ptr[1] = ptr[2];
//        }
//    }
//}
#endif
