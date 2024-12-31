#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static lv_obj_t * label_create(const lv_font_t * font, lv_style_t * style, const char * text_base)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text_fmt(label, "%s: the quick brown fox jumps over the lazy dog", text_base);
    //    lv_label_set_text_fmt(label, "l");
    lv_obj_set_style_text_font(label, font, 0);
    if(style) lv_obj_add_style(label, style, 0);

    return label;
}

static void all_labels_create(const char * name, lv_style_t * style)
{
    LV_FONT_DECLARE(test_font_montserrat_ascii_1bpp);
    LV_FONT_DECLARE(test_font_montserrat_ascii_2bpp);
    LV_FONT_DECLARE(test_font_montserrat_ascii_4bpp);
    LV_FONT_DECLARE(test_font_montserrat_ascii_3bpp_compressed);

    label_create(&test_font_montserrat_ascii_1bpp, style, "1bpp font");
    label_create(&test_font_montserrat_ascii_2bpp, style, "2bpp font");
    label_create(&test_font_montserrat_ascii_4bpp, style, "4bpp font");
    label_create(&test_font_montserrat_ascii_3bpp_compressed, style, "3bpp compressed font");

    char buf[64];
    lv_snprintf(buf, sizeof(buf), "draw/label_%s.png", name);
    TEST_ASSERT_EQUAL_SCREENSHOT(buf);
}

void test_draw_label_normal(void)
{
    all_labels_create("normal", NULL);
}

void test_draw_label_color(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_RED));
    all_labels_create("color", &style);

}

void test_draw_label_opa(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_RED));
    all_labels_create("opa", &style);
}

void test_draw_label_color_and_opa(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_text_opa(&style, LV_OPA_50);
    all_labels_create("color_and_opa", &style);
}

static lv_obj_t * decor_label_create(lv_text_decor_t decor, lv_text_align_t align, lv_opa_t opa)
{
    lv_color_t color = lv_palette_main(LV_PALETTE_BLUE);
    lv_color_t sel_bg_color = lv_palette_lighten(LV_PALETTE_RED, 4);
    lv_color_t sel_color = lv_palette_darken(LV_PALETTE_RED, 4);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hi,\nTesting the\nlabels.");
    lv_obj_set_style_text_decor(label, decor, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_opa(label, opa, 0);
    lv_obj_set_style_text_align(label, align, 0);
    lv_obj_set_style_bg_color(label, sel_bg_color, LV_PART_SELECTED);
    lv_obj_set_style_text_color(label, sel_color, LV_PART_SELECTED);
    lv_label_set_text_selection_start(label, 10);
    lv_label_set_text_selection_end(label, 19);

    return label;
}

static void all_decor_labels_create(lv_text_decor_t decor)
{

    lv_obj_t * label;
    label = decor_label_create(decor, LV_TEXT_ALIGN_LEFT, LV_OPA_COVER);
    lv_obj_add_flag(label, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    label = decor_label_create(decor, LV_TEXT_ALIGN_CENTER, LV_OPA_COVER);
    label = decor_label_create(decor, LV_TEXT_ALIGN_RIGHT, LV_OPA_COVER);

    label = decor_label_create(decor, LV_TEXT_ALIGN_LEFT, LV_OPA_50);
    label = decor_label_create(decor, LV_TEXT_ALIGN_CENTER, LV_OPA_50);
    label = decor_label_create(decor, LV_TEXT_ALIGN_RIGHT, LV_OPA_50);
}

void test_label_decor(void)
{
    all_decor_labels_create(LV_TEXT_DECOR_NONE);
    all_decor_labels_create(LV_TEXT_DECOR_UNDERLINE);
    all_decor_labels_create(LV_TEXT_DECOR_STRIKETHROUGH);
    all_decor_labels_create(LV_TEXT_DECOR_UNDERLINE | LV_TEXT_DECOR_STRIKETHROUGH);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/label_decor.png");
}

void test_label_selection_and_recolor(void)
{
    lv_text_decor_t decor = LV_TEXT_DECOR_NONE;
    lv_color_t color = lv_palette_main(LV_PALETTE_BLUE);
    lv_color_t sel_bg_color = lv_palette_lighten(LV_PALETTE_RED, 4);
    lv_color_t sel_color = lv_palette_darken(LV_PALETTE_RED, 4);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_recolor(label, true);
    lv_label_set_text(label, "Hi, Testing the #00ff00 colored labels.#");
    lv_obj_set_style_text_decor(label, decor, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_bg_color(label, sel_bg_color, LV_PART_SELECTED);
    lv_obj_set_style_text_color(label, sel_color, LV_PART_SELECTED);
    lv_label_set_text_selection_start(label, 10);
    lv_label_set_text_selection_end(label, 25);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/label_selection_and_recolor.png");
}

#endif
