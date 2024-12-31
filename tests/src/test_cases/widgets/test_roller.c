#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "lv_test_indev.h"
#include <string.h>

#define OPTION_BUFFER_SZ        (20U)
#define OPTION_SMALL_BUFFER_SZ  (3U)

static lv_obj_t * active_screen = NULL;
static lv_obj_t * roller = NULL;
static lv_obj_t * roller_infinite = NULL;
static lv_obj_t * roller_mouse = NULL;
static lv_group_t * g = NULL;
static lv_group_t * encoder_g = NULL;
static lv_group_t * mouse_g = NULL;

static const char * default_roller_options = "One\nTwo\nThree";
static const char * default_infinite_roller_options = "One\nTwo\nThree\nFour\nFive\nSix\nSeven\nEight\nNine\nTen";

void setUp(void)
{
    active_screen = lv_screen_active();
    roller = lv_roller_create(active_screen);
    roller_infinite = lv_roller_create(active_screen);
    roller_mouse = lv_roller_create(active_screen);

    lv_roller_set_options(roller, default_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_options(roller_infinite, default_infinite_roller_options, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_options(roller_mouse, default_roller_options, LV_ROLLER_MODE_NORMAL);

    g = lv_group_create();
    lv_indev_set_group(lv_test_keypad_indev, g);

    encoder_g = lv_group_create();
    lv_indev_set_group(lv_test_encoder_indev, encoder_g);

    mouse_g = lv_group_create();
    lv_indev_set_group(lv_test_mouse_indev, mouse_g);

    lv_group_add_obj(g, roller);
    lv_group_add_obj(encoder_g, roller_infinite);
    lv_group_add_obj(mouse_g, roller_mouse);
}

void tearDown(void)
{
    lv_group_remove_obj(roller);
    lv_group_remove_obj(roller_infinite);
    lv_group_remove_obj(roller_mouse);
    lv_obj_clean(active_screen);
}

void test_roller_get_options(void)
{
    TEST_ASSERT_EQUAL_STRING(default_roller_options, lv_roller_get_options(roller));
}

void test_roller_get_selected_option(void)
{
    char actual_str[OPTION_BUFFER_SZ] = {0x00};
    int16_t expected_index = 2;
    int16_t actual_index = 0;
    char * expected_index_str = "Three";

    /* Select the last option, index starts at 0 */
    uint16_t option_count = lv_roller_get_option_count(roller);
    lv_roller_set_selected(roller, option_count - 1, LV_ANIM_OFF);

    actual_index = lv_roller_get_selected(roller);
    TEST_ASSERT_EQUAL(expected_index, actual_index);

    /* Get the index string */
    lv_roller_get_selected_str(roller, actual_str, OPTION_BUFFER_SZ);

    TEST_ASSERT_EQUAL_STRING(expected_index_str, actual_str);
}

void test_roller_get_selected_option_truncated_buffer(void)
{
    char actual_str[OPTION_SMALL_BUFFER_SZ] = {0x00};
    char * expected_index_str = "Th";

    /* Select the last option, index starts at 0 */
    uint16_t option_count = lv_roller_get_option_count(roller);
    lv_roller_set_selected(roller, option_count - 1, LV_ANIM_OFF);

    /* Get the index string */
    lv_roller_get_selected_str(roller, actual_str, OPTION_SMALL_BUFFER_SZ);

    TEST_ASSERT_EQUAL_STRING(expected_index_str, actual_str);
}

void test_roller_infinite_mode_get_selected_option(void)
{
    char actual_str[OPTION_BUFFER_SZ] = {0x00};
    int16_t expected_index = 9;
    int16_t actual_index = 0;

    /* Select the last option of page 2 */
    uint16_t option_count = lv_roller_get_option_count(roller_infinite);
    option_count = (option_count * 2) - 1;
    lv_roller_set_selected(roller_infinite, option_count, LV_ANIM_OFF);

    actual_index = lv_roller_get_selected(roller_infinite);
    TEST_ASSERT_EQUAL(expected_index, actual_index);

    /* Get the index string */
    lv_roller_get_selected_str(roller_infinite, actual_str, OPTION_BUFFER_SZ);

    TEST_ASSERT_EQUAL_STRING("Ten", actual_str);
    memset(actual_str, 0x00, OPTION_BUFFER_SZ);

    /* Select the second option of page */
    lv_roller_set_selected(roller_infinite, 1, LV_ANIM_OFF);

    actual_index = lv_roller_get_selected(roller_infinite);
    TEST_ASSERT_EQUAL(1, actual_index);

    /* Get the index string */
    lv_roller_get_selected_str(roller_infinite, actual_str, OPTION_BUFFER_SZ);

    TEST_ASSERT_EQUAL_STRING("Two", actual_str);
}

void test_roller_set_selected_option_str(void)
{
    bool selected;
    TEST_ASSERT_EQUAL(0, lv_roller_get_selected(roller));

    /* Test an item that exists in the roller */
    selected = lv_roller_set_selected_str(roller, "Two", LV_ANIM_OFF);
    TEST_ASSERT_TRUE(selected);

    TEST_ASSERT_EQUAL(1, lv_roller_get_selected(roller));

    /* Try to select an item that does not exist in the roller */
    selected = lv_roller_set_selected_str(roller, "No", LV_ANIM_OFF);
    TEST_ASSERT_FALSE(selected);

    /* Make sure that the selection did not change */
    TEST_ASSERT_EQUAL(1, lv_roller_get_selected(roller));
}

void test_roller_keypad_events(void)
{
    int16_t expected_index = 1;
    int16_t actual_index = 0;

    lv_test_indev_wait(20);

    return;

    /* Select option index 1 with LV_KEY_RIGHT event */
    lv_roller_set_selected(roller, 0, LV_ANIM_OFF);
    lv_test_key_hit(LV_KEY_RIGHT);

    actual_index = lv_roller_get_selected(roller);
    TEST_ASSERT_EQUAL(expected_index, actual_index);

    /* Select next option with LV_KEY_DOWN */
    expected_index = 2;
    lv_test_key_hit(LV_KEY_DOWN);

    actual_index = lv_roller_get_selected(roller);
    TEST_ASSERT_EQUAL(expected_index, actual_index);

    /* Select previous option with LV_KEY_LEFT */
    expected_index = 1;
    lv_test_key_hit(LV_KEY_LEFT);

    actual_index = lv_roller_get_selected(roller);
    TEST_ASSERT_EQUAL(expected_index, actual_index);

    /* Select previous option with LV_KEY_UP */
    expected_index = 0;
    lv_test_key_hit(LV_KEY_UP);

    actual_index = lv_roller_get_selected(roller);
    TEST_ASSERT_EQUAL(expected_index, actual_index);
}

void test_roller_with_overlay_and_bubble_events_enabled(void)
{
    lv_obj_t * overlay = lv_obj_create(roller);
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_send_event(overlay, LV_EVENT_PRESSED, NULL);
}

//void test_roller_infinite_mode_first_option_gets_selected_after_last_option(void)
//{
//    char actual_str[OPTION_BUFFER_SZ] = {0x00};
//
//    lv_group_remove_obj(roller);
//    lv_group_add_obj(g, roller_infinite);
//
//    /* Select the last option of page 2 */
//    uint16_t option_count = lv_roller_get_option_count(roller_infinite);
//    option_count = (option_count * 2) - 1;
//    lv_roller_set_selected(roller_infinite, option_count, LV_ANIM_OFF);
//
//    /* Get the index string */
//    lv_roller_get_selected_str(roller_infinite, actual_str, OPTION_BUFFER_SZ);
//
//    TEST_ASSERT_EQUAL_STRING("Ten", actual_str);
//    memset(actual_str, 0x00, OPTION_BUFFER_SZ);
//
//    lv_test_key_hit(LV_KEY_DOWN);
//
//    /* Get the index string */
//    lv_roller_get_selected_str(roller_infinite, actual_str, OPTION_BUFFER_SZ);
//    TEST_ASSERT_EQUAL_STRING("One", actual_str);
//
//    lv_group_remove_obj(roller_infinite);
//}

//void test_roller_rendering_test(void)
//{
//#if LV_FONT_MONTSERRAT_24
//    static lv_style_t style_sel;
//    lv_style_init(&style_sel);
//    lv_style_set_text_font(&style_sel, &lv_font_montserrat_24);
//    lv_style_set_bg_color(&style_sel, lv_color_hex3(0xf88));
//    lv_style_set_border_width(&style_sel, 2);
//    lv_style_set_border_color(&style_sel, lv_color_hex3(0xf00));
//
//    lv_obj_add_style(roller, &style_sel, LV_PART_SELECTED);
//    lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_RIGHT, 0);
//    lv_roller_set_options(roller, "One\nTwo\nThree\nFour\nFive", LV_ROLLER_MODE_NORMAL);
//    lv_roller_set_selected(roller, 1, LV_ANIM_OFF);
//    lv_obj_center(roller);
//
//    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/roller_1.png");
//#else
//    TEST_PASS();
//#endif
//}
//
//void test_roller_select_option_with_click(void)
//{
//    char actual_str[OPTION_BUFFER_SZ] = {0x00};
//
//    lv_test_encoder_click();
//    lv_test_encoder_turn(1);
//
//    /* Get the index string */
//    lv_roller_get_selected_str(roller_infinite, actual_str, OPTION_BUFFER_SZ);
//
//    TEST_ASSERT_EQUAL_STRING("Two", actual_str);
//    memset(actual_str, 0x00, OPTION_BUFFER_SZ);
//}
//
//void test_roller_release_handler_pointer_indev(void)
//{
//    /* Click in the widget */
//    lv_test_mouse_click_at(roller_mouse->coords.x1 + 5, roller_mouse->coords.y1 + 5);
//    /* Check which is the selected option */
//    TEST_ASSERT_EQUAL(0, lv_roller_get_selected(roller_mouse));
//
//    /* Click further down the roller */
//    lv_test_mouse_click_at(roller_mouse->coords.x1 + 5, roller_mouse->coords.y1 + 100);
//    /* Check which is the selected option */
//    TEST_ASSERT_NOT_EQUAL(0, lv_roller_get_selected(roller_mouse));
//}

void test_roller_appearance(void)
{
    /* use a number, a symbol, a high letter, a low letter */
    const char * opts =
        "0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg\n0@Tg";

    lv_obj_add_flag(roller_mouse, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t * rollers[10] = {roller, roller_infinite};
    uint32_t i = 2;

    /* a normal and infinite roller with the same size font for the main and selected parts */
    lv_obj_set_pos(roller, 20, 20);
    lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
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

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/roller_2.png");

    /* test that the selected label stays in sync with the main label for scrolling */
    for(i = 0; i < 10; i++) {
        lv_roller_set_selected(rollers[i], lv_roller_get_option_count(rollers[i]) - 1, LV_ANIM_OFF);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/roller_3.png");
}

void test_roller_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_roller_create(lv_screen_active());
    lv_property_t prop = { };

    prop.id = LV_PROPERTY_ROLLER_OPTIONS;
    prop.arg1.ptr = "One\nTwo\nThree";
    prop.arg2.num = LV_ROLLER_MODE_NORMAL;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_STRING("One\nTwo\nThree", lv_roller_get_options(obj));
    TEST_ASSERT_EQUAL_STRING("One\nTwo\nThree", lv_obj_get_property(obj, LV_PROPERTY_ROLLER_OPTIONS).ptr);

    prop.id = LV_PROPERTY_ROLLER_SELECTED;
    prop.arg1.num = 1;
    prop.arg2.enable = LV_ANIM_ON;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(1, lv_roller_get_selected(obj));
    TEST_ASSERT_EQUAL_INT(1, lv_obj_get_property(obj, LV_PROPERTY_ROLLER_SELECTED).num);
#endif
}

#endif
