/**
 * @file lv_test_font_loader.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"
#if LV_BUILD_TEST
#include "../lv_test_assert.h"
#include "../src/lv_font/lv_font_fmt_txt.h"
#include "../src/lv_font/lv_font.h"
#include "../src/lv_font/lv_font_loader.h"

#include "lv_test_font_loader.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if LV_USE_FILESYSTEM
static int compare_fonts(lv_font_t * f1, lv_font_t * f2);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern lv_font_t font_1;
extern lv_font_t font_2;
extern lv_font_t font_3;

void lv_test_font_loader(void)
{
#if LV_USE_FILESYSTEM
    lv_test_print("");
    lv_test_print("===================");
    lv_test_print("Start lv_font tests");
    lv_test_print("===================");

    lv_font_t * font_1_bin = lv_font_load("f:font_1.fnt");
    lv_font_t * font_2_bin = lv_font_load("f:font_2.fnt");
    lv_font_t * font_3_bin = lv_font_load("f:font_3.fnt");

    compare_fonts(&font_1, font_1_bin);
    compare_fonts(&font_2, font_2_bin);
    compare_fonts(&font_3, font_3_bin);

    lv_font_free(font_1_bin);
    lv_font_free(font_2_bin);
    lv_font_free(font_3_bin);
#else
    lv_test_print("SKIP: font load test because it requires LV_USE_FILESYSTEM 1 and LV_FONT_FMT_TXT_LARGE 0");
#endif
}

#if LV_USE_FILESYSTEM
static int compare_fonts(lv_font_t * f1, lv_font_t * f2)
{
    lv_test_assert_true(f1 != NULL && f2 != NULL, "font not null");

    lv_test_assert_ptr_eq(f1->get_glyph_dsc, f2->get_glyph_dsc, "glyph_dsc");
    lv_test_assert_ptr_eq(f1->get_glyph_bitmap, f2->get_glyph_bitmap, "glyph_bitmap");
    lv_test_assert_int_eq(f1->line_height, f2->line_height, "line_height");
    lv_test_assert_int_eq(f1->base_line, f2->base_line, "base_line");
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    lv_test_assert_int_eq(f1->subpx, f2->subpx, "subpx");
#endif
    lv_font_fmt_txt_dsc_t * dsc1 = (lv_font_fmt_txt_dsc_t *) f1->dsc;
    lv_font_fmt_txt_dsc_t * dsc2 = (lv_font_fmt_txt_dsc_t *) f2->dsc;

    lv_test_assert_int_eq(dsc1->kern_scale, dsc2->kern_scale, "kern_scale");
    lv_test_assert_int_eq(dsc1->cmap_num, dsc2->cmap_num, "cmap_num");
    lv_test_assert_int_eq(dsc1->bpp, dsc2->bpp, "bpp");
    lv_test_assert_int_eq(dsc1->kern_classes, dsc2->kern_classes, "kern_classes");
    lv_test_assert_int_eq(dsc1->bitmap_format, dsc2->bitmap_format, "bitmap_format");

    // cmaps
    int total_glyphs = 0;
    for(int i = 0; i < dsc1->cmap_num; ++i) {
        lv_font_fmt_txt_cmap_t * cmaps1 = (lv_font_fmt_txt_cmap_t *) &dsc1->cmaps[i];
        lv_font_fmt_txt_cmap_t * cmaps2 = (lv_font_fmt_txt_cmap_t *) &dsc2->cmaps[i];

        lv_test_assert_int_eq(cmaps1->range_start, cmaps2->range_start, "range_start");
        lv_test_assert_int_eq(cmaps1->range_length, cmaps2->range_length, "range_length");
        lv_test_assert_int_eq(cmaps1->glyph_id_start, cmaps2->glyph_id_start, "glyph_id_start");
        lv_test_assert_int_eq(cmaps1->type, cmaps2->type, "type");
        lv_test_assert_int_eq(cmaps1->list_length, cmaps2->list_length, "list_length");

        if(cmaps1->unicode_list != NULL && cmaps2->unicode_list != NULL) {
            lv_test_assert_true(cmaps1->unicode_list && cmaps2->unicode_list, "unicode_list");

            lv_test_assert_array_eq(
                    (uint8_t *) cmaps1->unicode_list,
                    (uint8_t *) cmaps2->unicode_list,
                    sizeof(uint16_t) * cmaps1->list_length,
                    "unicode_list");
            total_glyphs += cmaps1->list_length;
        }
        else {
            total_glyphs += cmaps1->range_length;
            lv_test_assert_ptr_eq(cmaps1->unicode_list, cmaps2->unicode_list, "unicode_list");
        }

        if(cmaps1->glyph_id_ofs_list != NULL && cmaps2->glyph_id_ofs_list != NULL) {
            uint8_t * ids1 = (uint8_t *) cmaps1->glyph_id_ofs_list;
            uint8_t * ids2 = (uint8_t *) cmaps2->glyph_id_ofs_list;

            lv_test_assert_array_eq(ids1, ids2, cmaps1->list_length, "glyph_id_ofs_list");
        }
        else {
            lv_test_assert_ptr_eq(cmaps1->glyph_id_ofs_list, cmaps2->glyph_id_ofs_list, "glyph_id_ofs_list");
        }
    }

    // kern_dsc
    if (dsc1->kern_classes == 1 && dsc2->kern_classes == 1) {
        lv_font_fmt_txt_kern_classes_t * kern1 = (lv_font_fmt_txt_kern_classes_t *) dsc1->kern_dsc;
        lv_font_fmt_txt_kern_classes_t * kern2 = (lv_font_fmt_txt_kern_classes_t *) dsc2->kern_dsc;
        if (kern1 != NULL && kern2 != NULL) {
            lv_test_assert_int_eq(kern1->right_class_cnt, kern2->right_class_cnt, "right_class_cnt");
            lv_test_assert_int_eq(kern1->left_class_cnt, kern2->left_class_cnt, "left_class_cnt");

            lv_test_assert_array_eq(
                    (uint8_t *) kern1->left_class_mapping,
                    (uint8_t *) kern2->left_class_mapping,
                    kern1->left_class_cnt,
                    "left_class_mapping");

            lv_test_assert_array_eq(
                    (uint8_t *) kern1->right_class_mapping,
                    (uint8_t *) kern2->right_class_mapping,
                    kern1->right_class_cnt,
                    "right_class_mapping");

            lv_test_assert_array_eq(
                    (uint8_t *) kern1->class_pair_values,
                    (uint8_t *) kern2->class_pair_values,
                    kern1->right_class_cnt * kern1->left_class_cnt,
                    "class_pair_values");
        }
        else {
            lv_test_assert_ptr_eq(kern1, kern2, "kern");
        }
    }
    else if (dsc1->kern_classes == 0 && dsc2->kern_classes == 0) {
        lv_font_fmt_txt_kern_pair_t * kern1 = (lv_font_fmt_txt_kern_pair_t *) dsc1->kern_dsc;
        lv_font_fmt_txt_kern_pair_t * kern2 = (lv_font_fmt_txt_kern_pair_t *) dsc2->kern_dsc;
        if (kern1 != NULL && kern2 != NULL) {
            lv_test_assert_int_eq(kern1->glyph_ids_size, kern2->glyph_ids_size, "glyph_ids_size");
            lv_test_assert_int_eq(kern1->pair_cnt, kern2->pair_cnt, "pair_cnt");

            int ids_size;

            if (kern1->glyph_ids_size == 0) {
                ids_size = sizeof(int8_t) * 2 * kern1->pair_cnt;
            }
            else {
                ids_size = sizeof(int16_t) * 2 * kern1->pair_cnt;
            }

            lv_test_assert_array_eq(kern1->glyph_ids, kern2->glyph_ids, ids_size, "glyph_ids");
            lv_test_assert_array_eq(
                    (uint8_t * ) kern1->values,
                    (uint8_t * ) kern2->values,
                    kern1->pair_cnt,
                    "glyph_values");
        }
    }

    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc1 = (lv_font_fmt_txt_glyph_dsc_t *) dsc1->glyph_dsc;
    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc2 = (lv_font_fmt_txt_glyph_dsc_t *) dsc2->glyph_dsc;

    for(int i = 0; i < total_glyphs; ++i) {
        if (i < total_glyphs - 1) {
            int size1 = glyph_dsc1[i+1].bitmap_index - glyph_dsc1[i].bitmap_index;

            if (size1 > 0) {
                lv_test_assert_array_eq(
                        dsc1->glyph_bitmap + glyph_dsc1[i].bitmap_index,
                        dsc2->glyph_bitmap + glyph_dsc2[i].bitmap_index,
                        size1 - 1, "glyph_bitmap");
            }
        }
        lv_test_assert_int_eq(glyph_dsc1[i].adv_w, glyph_dsc2[i].adv_w, "adv_w");
        lv_test_assert_int_eq(glyph_dsc1[i].box_w, glyph_dsc2[i].box_w, "box_w");
        lv_test_assert_int_eq(glyph_dsc1[i].box_h, glyph_dsc2[i].box_h, "box_h");
        lv_test_assert_int_eq(glyph_dsc1[i].ofs_x, glyph_dsc2[i].ofs_x, "ofs_x");
        lv_test_assert_int_eq(glyph_dsc1[i].ofs_y, glyph_dsc2[i].ofs_y, "ofs_y");
    }

    LV_LOG_INFO("No differences found!");
    return 0;
}
#endif

#pragma GCC diagnostic pop
/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif // LV_BUILD_TEST
