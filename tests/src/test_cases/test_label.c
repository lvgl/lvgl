#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include <string.h>

static const char * long_text =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras malesuada ultrices magna in rutrum.";
static const char * long_text_multiline =
    "Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit.\nCras malesuada ultrices magna in rutrum.\n";
static const char * empty_text = "";

static lv_obj_t * active_screen = NULL;
static lv_obj_t * label;
static lv_obj_t * long_label;
static lv_obj_t * long_label_multiline;
static lv_obj_t * empty_label;

void setUp(void)
{
    active_screen = lv_scr_act();
    label = lv_label_create(active_screen);
    long_label = lv_label_create(active_screen);
    long_label_multiline = lv_label_create(active_screen);
    empty_label = lv_label_create(active_screen);

    lv_label_set_text(long_label, long_text);
    lv_label_set_text(long_label_multiline, long_text_multiline);
    lv_label_set_text(empty_label, empty_text);
}

void tearDown(void)
{
    lv_obj_clean(lv_scr_act());
}

void test_label_max_width(void)
{
    lv_obj_clean(lv_scr_act());

    lv_obj_t * test_label1 = lv_label_create(lv_scr_act());
    lv_label_set_text(test_label1, long_text);
    lv_obj_set_width(test_label1, 600);
    lv_obj_set_style_max_width(test_label1, 200, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_label1, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(test_label1, LV_OPA_100, LV_PART_MAIN);

    lv_obj_t * test_label2 = lv_label_create(lv_scr_act());
    lv_label_set_text(test_label2, long_text);
    lv_obj_set_width(test_label2, 600);
    lv_obj_set_height(test_label2, 50);
    lv_obj_set_x(test_label2, 300);
    lv_obj_set_style_max_width(test_label2, 200, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_label2, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(test_label2, LV_OPA_100, LV_PART_MAIN);
    lv_label_set_long_mode(test_label2, LV_LABEL_LONG_DOT);

    TEST_ASSERT_EQUAL_SCREENSHOT("label_max_width.png");
}

#endif
