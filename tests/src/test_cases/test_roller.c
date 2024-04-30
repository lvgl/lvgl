#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

/**
 * see test_roller.c from v9.1 to borrow more test fixture components
*/

static lv_obj_t * active_screen = NULL;
static lv_obj_t * roller = NULL;
static lv_obj_t * roller_infinite = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    roller = lv_roller_create(active_screen);
    roller_infinite = lv_roller_create(active_screen);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_roller_appearance(void)
{
    /* use a number, a symbol, a high letter, a low letter */
    const char * opts =
        "0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg";

    lv_obj_t * rollers[10] = {roller, roller_infinite};
    uint32_t i = 2;

    /* a normal and infinite roller with the same size font for the main and selected parts */
    lv_obj_set_pos(roller, 20, 20);
    lv_roller_set_options(roller, opts, LV_PART_MAIN);
    lv_obj_set_pos(roller_infinite, 20, 200);
    lv_roller_set_options(roller_infinite, opts, LV_ROLLER_MODE_INFINITE);

    /* a normal and infinite roller with slightly different size fonts for the main and selected parts */
    lv_obj_t * r = lv_roller_create(active_screen);
    lv_obj_set_pos(r, 130, 20);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_24, LV_PART_SELECTED);
    rollers[i++] = r;
    r = lv_roller_create(active_screen);
    lv_obj_set_pos(r, 130, 200);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_INFINITE);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_24, LV_PART_SELECTED);
    rollers[i++] = r;

    /* same as previous pair but the fonts are swapped for the main and selected parts */
    r = lv_roller_create(active_screen);
    lv_obj_set_pos(r, 270, 20);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_16, LV_PART_SELECTED);
    rollers[i++] = r;
    r = lv_roller_create(active_screen);
    lv_obj_set_pos(r, 270, 200);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_INFINITE);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_16, LV_PART_SELECTED);
    rollers[i++] = r;

    /* a normal and infinite roller with extremely different size fonts for the main and selected parts */
    r = lv_roller_create(active_screen);
    lv_obj_set_pos(r, 410, 20);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_8, LV_PART_MAIN);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_36, LV_PART_SELECTED);
    rollers[i++] = r;
    r = lv_roller_create(active_screen);
    lv_obj_set_pos(r, 410, 200);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_INFINITE);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_8, LV_PART_MAIN);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_36, LV_PART_SELECTED);
    rollers[i++] = r;

    /* same as previous pair but the fonts are swapped for the main and selected parts */
    r = lv_roller_create(active_screen);
    lv_obj_set_pos(r, 580, 20);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_36, LV_PART_MAIN);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_8, LV_PART_SELECTED);
    rollers[i++] = r;
    r = lv_roller_create(active_screen);
    lv_obj_set_pos(r, 580, 200);
    lv_roller_set_options(r, opts, LV_ROLLER_MODE_INFINITE);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_36, LV_PART_MAIN);
    lv_obj_set_style_text_font(r, &lv_font_montserrat_8, LV_PART_SELECTED);
    rollers[i++] = r;

    TEST_ASSERT_EQUAL_SCREENSHOT("roller_1.png");

    /* test that the selected label stays in sync with the main label for scrolling */
    for(i = 0; i < 10; i++) {
        lv_roller_set_selected(rollers[i], lv_roller_get_option_cnt(rollers[i]) - 1, LV_ANIM_OFF);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("roller_2.png");
}

#endif
