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
static int read_png_file(png_img_t * p, const char* file_name);
static void png_release(png_img_t * p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_test_assert_img_eq(const char * fn_ref)
{
  char fn_ref_full[512];
  sprintf(fn_ref_full, "%s%s", REF_IMGS_PATH, fn_ref);

  png_img_t p;
  int res = read_png_file(&p, fn_ref_full);
  if(res < 0) return false;
  uint8_t * screen_buf;

  lv_obj_invalidate(lv_scr_act());
  lv_refr_now(NULL);

  extern lv_color_t test_fb[];

  screen_buf = (uint8_t *)test_fb;

  uint8_t * ptr_act = NULL;
  const png_byte* ptr_ref = NULL;

  bool err = false;
  int x, y, i_buf = 0;
  for (y = 0; y < p.height; y++) {
    png_byte* row = p.row_pointers[y];

    for (x = 0; x < p.width; x++) {
      ptr_ref = &(row[x*3]);
      ptr_act = &(screen_buf[i_buf*4]);

      uint32_t ref_px = 0;
      uint32_t act_px = 0;
      memcpy(&ref_px, ptr_ref, 3);
      memcpy(&act_px, ptr_act, 3);
      //printf("0xFF%06x, ", act_px);

      uint8_t act_swap[3] = {ptr_act[2], ptr_act[1], ptr_act[0]};

      if(memcmp(act_swap, ptr_ref, 3) != 0) {
        err = true;
        break;
      }
      i_buf++;
    }
    if(err) break;
  }

  if(err) {
      uint32_t ref_px = 0;
      uint32_t act_px = 0;
      memcpy(&ref_px, ptr_ref, 3);
      memcpy(&act_px, ptr_act, 3);

      FILE * f = fopen("../test_screenshot_error.h", "w");

      fprintf(f, "//Diff in %s at (%d;%d), %x instead of %x)\n\n", fn_ref, x, y, act_px, ref_px);
      fprintf(f, "static const uint32_t test_screenshot_error_data[] = {\n");

      i_buf = 0;
      for (y = 0; y < 480; y++) {
        fprintf(f, "\n");
        for (x = 0; x < 800; x++) {
          ptr_act = &(screen_buf[i_buf * 4]);
          act_px = 0;
          memcpy(&act_px, ptr_act, 3);
          fprintf(f, "0xFF%06X, ", act_px);
          i_buf++;
        }
      }
      fprintf(f, "};\n\n");

      fprintf(f, "static lv_img_dsc_t test_screenshot_error_dsc = { \n"
      "  .header.w = 800,\n"
      "  .header.h = 480,\n"
      "  .header.always_zero = 0,\n"
      "  .header.cf = LV_IMG_CF_TRUE_COLOR,\n"
      "  .data_size = 800 * 480 * 4,\n"
      "  .data = test_screenshot_error_data};\n\n"
      "static inline void test_screenshot_error_show(void)\n"
      "{\n"
      "  lv_obj_t * img = lv_img_create(lv_scr_act());\n"
      "  lv_img_set_src(img, &test_screenshot_error_dsc);\n"
      "}\n");

      fclose(f);

  }


  png_release(&p);

  return !err;

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int read_png_file(png_img_t * p, const char* file_name)
{
    char header[8];    // 8 is the maximum size that can be checked

    /*open file and test for it being a png*/
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
       TEST_PRINTF("%s", "PNG file %s could not be opened for reading");
        return -1;
    }

    size_t rcnt = fread(header, 1, 8, fp);
    if (rcnt != 8 || png_sig_cmp((png_const_bytep)header, 0, 8)) {
        TEST_PRINTF("%s is not recognized as a PNG file", file_name);
        return -1;
    }

    /*initialize stuff*/
    p->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!p->png_ptr) {
        TEST_PRINTF("%s", "png_create_read_struct failed");
        return -1;
    }

    p->info_ptr = png_create_info_struct(p->png_ptr);
    if (!p->info_ptr) {
        TEST_PRINTF("%s", "png_create_info_struct failed");
        return -1;
    }
    if (setjmp(png_jmpbuf(p->png_ptr))) {
        TEST_PRINTF("%s", "Error during init_io");
        return -1;
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
    if (setjmp(png_jmpbuf(p->png_ptr))) {
        TEST_PRINTF("%s", "Error during read_image");
        return -1;
    }
    p->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * p->height);

    int y;
    for (y=0; y<p->height; y++)
        p->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(p->png_ptr,p->info_ptr));

    png_read_image(p->png_ptr, p->row_pointers);

    fclose(fp);
    return 0;
}

static void png_release(png_img_t * p)
{
    int y;
    for (y=0; y<p->height; y++) free(p->row_pointers[y]);

    free(p->row_pointers);

    png_destroy_read_struct(&p->png_ptr, &p->info_ptr, NULL);
}


#endif
