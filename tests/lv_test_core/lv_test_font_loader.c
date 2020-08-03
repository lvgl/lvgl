/**
 * @file lv_test_font_loader.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lv_test_assert.h"
#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../src/lv_font/lv_font_fmt_txt.h"
#include "../src/lv_font/lv_font.h"
#include "../src/lv_font/lv_font_loader.h"

#include "lv_test_font_loader.h" 

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int compare_fonts(lv_font_t * f1, lv_font_t * f2);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_test_font_loader(void)
{
    lv_font_t * font = lv_font_load("f:lv_font_montserrat_12.bin");
    compare_fonts(&lv_font_montserrat_12, font);
}

static int compare_fonts(lv_font_t * f1, lv_font_t * f2)
{
    lv_test_assert_true(f1 != NULL && f2 != NULL, "error loading font");

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

            for(int k = 0; k < cmaps1->list_length; ++k) {
                lv_test_assert_int_eq(cmaps1->unicode_list[k], cmaps2->unicode_list[k], "unicode_list");
            }
            total_glyphs += cmaps1->list_length;
        }
        else {
            total_glyphs += cmaps1->range_length;
            lv_test_assert_ptr_eq(cmaps1->unicode_list, cmaps2->unicode_list, "unicode_list");
        }

        if(cmaps1->glyph_id_ofs_list != NULL && cmaps2->glyph_id_ofs_list != NULL) {
            uint8_t * ids1 = (uint8_t *) cmaps1->glyph_id_ofs_list;
            uint8_t * ids2 = (uint8_t *) cmaps2->glyph_id_ofs_list;

            for(int j = 0; j < cmaps1->range_length; j++) {
                lv_test_assert_int_eq(ids1[j], ids2[j], "glyph_id_ofs_list");
            }
        }
        else {
            lv_test_assert_ptr_eq(cmaps1->glyph_id_ofs_list, cmaps2->glyph_id_ofs_list, "glyph_id_ofs_list");
        }
    }

    // kern_dsc
    lv_font_fmt_txt_kern_classes_t * kern1 = (lv_font_fmt_txt_kern_classes_t *) dsc1->kern_dsc;
    lv_font_fmt_txt_kern_classes_t * kern2 = (lv_font_fmt_txt_kern_classes_t *) dsc2->kern_dsc;

    lv_test_assert_int_eq(kern1->right_class_cnt, kern2->right_class_cnt, "right_class_cnt");
    lv_test_assert_int_eq(kern1->left_class_cnt, kern2->left_class_cnt, "left_class_cnt");

    for(int i = 0; i < kern1->left_class_cnt; ++i) {
        lv_test_assert_int_eq(kern1->left_class_mapping[i],
                      kern2->left_class_mapping[i], "left_class_mapping");
    }
    for(int i = 0; i < kern1->right_class_cnt; ++i) {
        lv_test_assert_int_eq(kern1->right_class_mapping[i],
                      kern2->right_class_mapping[i], "right_class_mapping");
    }

    for(int i = 0; i < kern1->right_class_cnt * kern1->left_class_cnt; ++i) {
        lv_test_assert_int_eq(kern1->class_pair_values[i],
                      kern2->class_pair_values[i], "class_pair_values");
    }

    // TODO: glyph_bitmap

    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc1 = (lv_font_fmt_txt_glyph_dsc_t *) dsc1->glyph_dsc;
    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc2 = (lv_font_fmt_txt_glyph_dsc_t *) dsc2->glyph_dsc;

    for(int i = 0; i < total_glyphs; ++i) {
        //lv_test_assert_int_eq(glyph_dsc1[i].bitmap_index, glyph_dsc2[i].bitmap_index, "bitmap_index");
        lv_test_assert_int_eq(glyph_dsc1[i].adv_w, glyph_dsc2[i].adv_w, "adv_w");
        lv_test_assert_int_eq(glyph_dsc1[i].box_w, glyph_dsc2[i].box_w, "box_w");
        lv_test_assert_int_eq(glyph_dsc1[i].box_h, glyph_dsc2[i].box_h, "box_h");
        lv_test_assert_int_eq(glyph_dsc1[i].ofs_x, glyph_dsc2[i].ofs_x, "ofs_x");
        lv_test_assert_int_eq(glyph_dsc1[i].ofs_y, glyph_dsc2[i].ofs_y, "ofs_y");
    }

    LV_LOG_INFO("No differences found!");
    return 0;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif // LV_BUILD_TEST

