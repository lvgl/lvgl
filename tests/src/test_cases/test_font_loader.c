/**
 * @file test_font_loader.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#if LV_BUILD_TEST
#include "../../lvgl.h"

#include "unity/unity.h"

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
void test_font_loader_with_cache(void);
void test_font_loader_no_cache(void);
void test_font_loader_from_buffer(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/* fonts converted to C structs using the LVGL Font Converter */
extern lv_font_t font_1;
extern lv_font_t font_2;
extern lv_font_t font_3;

/* font binaries converted to plain C arrays */
extern uint8_t const font_1_buf[6876];
extern uint8_t const font_2_buf[7252];
extern uint8_t const font_3_buf[4892];

static lv_font_t * font_1_bin = NULL;
static lv_font_t * font_2_bin = NULL;
static lv_font_t * font_3_bin = NULL;

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */

}

static void common(void)
{
    compare_fonts(&font_1, font_1_bin);
    compare_fonts(&font_2, font_2_bin);
    compare_fonts(&font_3, font_3_bin);

    /* create labels for testing */
    lv_obj_t * scr = lv_screen_active();
    lv_obj_t * label1 = lv_label_create(scr);
    lv_obj_t * label2 = lv_label_create(scr);
    lv_obj_t * label3 = lv_label_create(scr);

    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_label_set_text(label1, "The quick brown fox jumped over the lazy dog");
    lv_obj_set_style_text_font(label1, font_1_bin, 0);
    lv_label_set_text(label2, "The quick brown fox jumped over the lazy dog");
    lv_obj_set_style_text_font(label2, font_2_bin, 0);
    lv_label_set_text(label3, "The quick brown fox jumped over the lazy dog");
    lv_obj_set_style_text_font(label3, font_3_bin, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("font_loader_1.png");

    lv_obj_clean(lv_screen_active());

    lv_binfont_destroy(font_1_bin);
    lv_binfont_destroy(font_2_bin);
    lv_binfont_destroy(font_3_bin);
}

void test_font_loader_with_cache(void)
{
    /*Test with cache ('A' has cache)*/

    font_1_bin = lv_binfont_create("A:src/test_assets/font_1.fnt");
    TEST_ASSERT_NOT_NULL(font_1_bin);

    font_2_bin = lv_binfont_create("A:src/test_assets/font_2.fnt");
    TEST_ASSERT_NOT_NULL(font_2_bin);

    font_3_bin = lv_binfont_create("A:src/test_assets/font_3.fnt");
    TEST_ASSERT_NOT_NULL(font_3_bin);

    common();
}

void test_font_loader_no_cache(void)
{
    /*Test without cache ('B' has NO cache)*/

    font_1_bin = lv_binfont_create("B:src/test_assets/font_1.fnt");
    TEST_ASSERT_NOT_NULL(font_1_bin);

    font_2_bin = lv_binfont_create("B:src/test_assets/font_2.fnt");
    TEST_ASSERT_NOT_NULL(font_2_bin);

    font_3_bin = lv_binfont_create("B:src/test_assets/font_3.fnt");
    TEST_ASSERT_NOT_NULL(font_3_bin);

    common();
}

void test_font_loader_from_buffer(void)
{
    /*Test with memfs*/

    font_1_bin = lv_binfont_create_from_buffer((void *)&font_1_buf, sizeof(font_1_buf));
    TEST_ASSERT_NOT_NULL(font_1_bin);

    font_2_bin = lv_binfont_create_from_buffer((void *)&font_2_buf, sizeof(font_2_buf));
    TEST_ASSERT_NOT_NULL(font_2_bin);

    font_3_bin = lv_binfont_create_from_buffer((void *)&font_3_buf, sizeof(font_3_buf));
    TEST_ASSERT_NOT_NULL(font_3_bin);

    common();
}

void test_font_loader_reload(void)
{
    /*Reload a font which is being used by a label*/
    lv_obj_t * scr = lv_screen_active();
    lv_obj_t * label = lv_label_create(scr);
    lv_obj_center(label);
    lv_label_set_text(label, "The quick brown fox jumped over the lazy dog");

    lv_font_t style_font;
    lv_font_t * font;
    font = lv_binfont_create("A:src/test_assets/font_2.fnt");
    TEST_ASSERT_NOT_NULL(font);
    lv_memcpy(&style_font, font, sizeof(lv_font_t));

    lv_obj_set_style_text_font(label, &style_font, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("font_loader_2.png");

    lv_binfont_destroy(font);

    font = lv_binfont_create("A:src/test_assets/font_3.fnt");
    TEST_ASSERT_NOT_NULL(font);
    lv_memcpy(&style_font, font, sizeof(lv_font_t));

    lv_obj_report_style_change(NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("font_loader_3.png");

    lv_obj_delete(label);

    lv_binfont_destroy(font);
}

static int compare_fonts(lv_font_t * f1, lv_font_t * f2)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(f1, "font not null");
    TEST_ASSERT_NOT_NULL_MESSAGE(f2, "font not null");

    //    Skip these test because -Wpedantic tells
    //    ISO C forbids passing argument 1 of ‘TEST_ASSERT_EQUAL_PTR_MESSAGE’ between function pointer and ‘void *’
    //    TEST_ASSERT_EQUAL_PTR_MESSAGE(f1->get_glyph_dsc, f2->get_glyph_dsc, "glyph_dsc");
    //    TEST_ASSERT_EQUAL_PTR_MESSAGE(f1->get_glyph_bitmap, f2->get_glyph_bitmap, "glyph_bitmap");

    TEST_ASSERT_EQUAL_INT_MESSAGE(f1->line_height, f2->line_height, "line_height");
    TEST_ASSERT_EQUAL_INT_MESSAGE(f1->base_line, f2->base_line, "base_line");
    TEST_ASSERT_EQUAL_INT_MESSAGE(f1->subpx, f2->subpx, "subpx");
    lv_font_fmt_txt_dsc_t * dsc1 = (lv_font_fmt_txt_dsc_t *)f1->dsc;
    lv_font_fmt_txt_dsc_t * dsc2 = (lv_font_fmt_txt_dsc_t *)f2->dsc;

    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->kern_scale, dsc2->kern_scale, "kern_scale");
    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->cmap_num, dsc2->cmap_num, "cmap_num");
    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->bpp, dsc2->bpp, "bpp");
    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->kern_classes, dsc2->kern_classes, "kern_classes");
    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->bitmap_format, dsc2->bitmap_format, "bitmap_format");

    // cmaps
    int total_glyphs = 0;
    for(int i = 0; i < dsc1->cmap_num; ++i) {
        lv_font_fmt_txt_cmap_t * cmaps1 = (lv_font_fmt_txt_cmap_t *)&dsc1->cmaps[i];
        lv_font_fmt_txt_cmap_t * cmaps2 = (lv_font_fmt_txt_cmap_t *)&dsc2->cmaps[i];

        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->range_start, cmaps2->range_start, "range_start");
        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->range_length, cmaps2->range_length, "range_length");
        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->glyph_id_start, cmaps2->glyph_id_start, "glyph_id_start");
        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->type, cmaps2->type, "type");
        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->list_length, cmaps2->list_length, "list_length");

        if(cmaps1->unicode_list != NULL && cmaps2->unicode_list != NULL) {
            TEST_ASSERT_TRUE_MESSAGE(cmaps1->unicode_list && cmaps2->unicode_list, "unicode_list");

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *)cmaps1->unicode_list,
                (uint8_t *)cmaps2->unicode_list,
                sizeof(uint16_t) * cmaps1->list_length,
                "unicode_list");
            total_glyphs += cmaps1->list_length;
        }
        else {
            total_glyphs += cmaps1->range_length;
            TEST_ASSERT_EQUAL_PTR_MESSAGE(cmaps1->unicode_list, cmaps2->unicode_list, "unicode_list");
        }

        if(cmaps1->glyph_id_ofs_list != NULL && cmaps2->glyph_id_ofs_list != NULL) {
            uint8_t * ids1 = (uint8_t *)cmaps1->glyph_id_ofs_list;
            uint8_t * ids2 = (uint8_t *)cmaps2->glyph_id_ofs_list;

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(ids1, ids2, cmaps1->list_length, "glyph_id_ofs_list");
        }
        else {
            TEST_ASSERT_EQUAL_PTR_MESSAGE(cmaps1->glyph_id_ofs_list, cmaps2->glyph_id_ofs_list, "glyph_id_ofs_list");
        }
    }

    // kern_dsc
    if(dsc1->kern_classes == 1 && dsc2->kern_classes == 1) {
        lv_font_fmt_txt_kern_classes_t * kern1 = (lv_font_fmt_txt_kern_classes_t *)dsc1->kern_dsc;
        lv_font_fmt_txt_kern_classes_t * kern2 = (lv_font_fmt_txt_kern_classes_t *)dsc2->kern_dsc;
        if(kern1 != NULL && kern2 != NULL) {
            TEST_ASSERT_EQUAL_INT_MESSAGE(kern1->right_class_cnt, kern2->right_class_cnt, "right_class_cnt");
            TEST_ASSERT_EQUAL_INT_MESSAGE(kern1->left_class_cnt, kern2->left_class_cnt, "left_class_cnt");

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *)kern1->left_class_mapping,
                (uint8_t *)kern2->left_class_mapping,
                kern1->left_class_cnt,
                "left_class_mapping");

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *)kern1->right_class_mapping,
                (uint8_t *)kern2->right_class_mapping,
                kern1->right_class_cnt,
                "right_class_mapping");

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *)kern1->class_pair_values,
                (uint8_t *)kern2->class_pair_values,
                kern1->right_class_cnt * kern1->left_class_cnt,
                "class_pair_values");
        }
        else {
            TEST_ASSERT_EQUAL_PTR_MESSAGE(kern1, kern2, "kern");
        }
    }
    else if(dsc1->kern_classes == 0 && dsc2->kern_classes == 0) {
        lv_font_fmt_txt_kern_pair_t * kern1 = (lv_font_fmt_txt_kern_pair_t *)dsc1->kern_dsc;
        lv_font_fmt_txt_kern_pair_t * kern2 = (lv_font_fmt_txt_kern_pair_t *)dsc2->kern_dsc;
        if(kern1 != NULL && kern2 != NULL) {
            TEST_ASSERT_EQUAL_INT_MESSAGE(kern1->glyph_ids_size, kern2->glyph_ids_size, "glyph_ids_size");
            TEST_ASSERT_EQUAL_INT_MESSAGE(kern1->pair_cnt, kern2->pair_cnt, "pair_cnt");

            int ids_size;

            if(kern1->glyph_ids_size == 0) {
                ids_size = sizeof(int8_t) * 2 * kern1->pair_cnt;
            }
            else {
                ids_size = sizeof(int16_t) * 2 * kern1->pair_cnt;
            }

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(kern1->glyph_ids, kern2->glyph_ids, ids_size, "glyph_ids");
            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *) kern1->values,
                (uint8_t *) kern2->values,
                kern1->pair_cnt,
                "glyph_values");
        }
    }

    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc1 = (lv_font_fmt_txt_glyph_dsc_t *)dsc1->glyph_dsc;
    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc2 = (lv_font_fmt_txt_glyph_dsc_t *)dsc2->glyph_dsc;

    for(int i = 0; i < total_glyphs; ++i) {
        if(i < total_glyphs - 1) {
            int size1 = glyph_dsc1[i + 1].bitmap_index - glyph_dsc1[i].bitmap_index;

            if(size1 > 0) {
                TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                    dsc1->glyph_bitmap + glyph_dsc1[i].bitmap_index,
                    dsc2->glyph_bitmap + glyph_dsc2[i].bitmap_index,
                    size1 - 1, "glyph_bitmap");
            }
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].adv_w, glyph_dsc2[i].adv_w, "adv_w");
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].box_w, glyph_dsc2[i].box_w, "box_w");
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].box_h, glyph_dsc2[i].box_h, "box_h");
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].ofs_x, glyph_dsc2[i].ofs_x, "ofs_x");
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].ofs_y, glyph_dsc2[i].ofs_y, "ofs_y");
    }

    LV_LOG_INFO("No differences found!");
    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif // LV_BUILD_TEST
