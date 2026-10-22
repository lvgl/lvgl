#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_BARCODE

#include <string.h>

#define BARCODE_DATA  "https://lvgl.io"
#define BARCODE_SIZE  460
#define BARCODE_SCALE 2
#define BARCODE_THICK 50
#define COLOR_LIGHT lv_color_hex3(0xFAA)
#define COLOR_DARK lv_color_hex3(0xB00)

static lv_obj_t * active_screen = NULL;
static uint32_t redraw_warning_cnt;
static uint32_t encode_cnt;

/*A horizontal barcode with the defaults the screenshots were taken with*/
static lv_obj_t * barcode_create_hor(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);

    lv_obj_set_width(barcode, BARCODE_SIZE);
    lv_obj_set_height(barcode, BARCODE_THICK);
    lv_barcode_set_scale(barcode, BARCODE_SCALE);
    return barcode;
}

void setUp(void)
{
    active_screen = lv_screen_active();
    redraw_warning_cnt = 0;
    encode_cnt = 0;
}

void tearDown(void)
{
    lv_log_register_print_cb(NULL);
    lv_obj_clean(active_screen);
}

void test_barcode_default(void)
{
    lv_barcode_create(active_screen);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_0.png");
}
void test_barcode_basics(void)
{
    lv_obj_t * barcode = barcode_create_hor();

    lv_barcode_set_dark_color(barcode, COLOR_DARK);
    lv_barcode_set_light_color(barcode, COLOR_LIGHT);

    TEST_ASSERT_EQUAL_COLOR(COLOR_DARK, lv_barcode_get_dark_color(barcode));
    TEST_ASSERT_EQUAL_COLOR(COLOR_LIGHT, lv_barcode_get_light_color(barcode));
    TEST_ASSERT_EQUAL(BARCODE_SCALE, lv_barcode_get_scale(barcode));
    TEST_ASSERT_EQUAL(LV_DIR_HOR, lv_barcode_get_direction(barcode));

    /*Horizontal*/
    lv_barcode_set_data(barcode, BARCODE_DATA);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    /*Vertical. The length along the bars is the same; the thickness moves to the width.*/
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_height(barcode, BARCODE_SIZE);
    lv_obj_set_width(barcode, BARCODE_THICK);
    TEST_ASSERT_EQUAL(LV_DIR_VER, lv_barcode_get_direction(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");
}

void test_barcode_properties_after_data(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);
    lv_obj_center(barcode);

    /*The data comes first here, the properties after. The result has to be the same
     *bitmap test_barcode_basics() gets by setting them the other way round.*/
    lv_barcode_set_data(barcode, BARCODE_DATA);

    lv_barcode_set_dark_color(barcode, COLOR_DARK);
    lv_barcode_set_light_color(barcode, COLOR_LIGHT);
    lv_obj_set_width(barcode, BARCODE_SIZE);
    lv_obj_set_height(barcode, BARCODE_THICK);
    lv_barcode_set_scale(barcode, BARCODE_SCALE);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_1.png");

    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_obj_set_height(barcode, BARCODE_SIZE);
    lv_obj_set_width(barcode, BARCODE_THICK);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_2.png");
}

void test_barcode_scale_and_content_size(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    lv_obj_center(barcode);
    lv_obj_set_height(barcode, BARCODE_THICK);
    lv_barcode_set_dark_color(barcode, COLOR_DARK);
    lv_barcode_set_light_color(barcode, COLOR_LIGHT);

    /*One pixel bars and a content sized width by default*/
    TEST_ASSERT_EQUAL(1, lv_barcode_get_scale(barcode));
    TEST_ASSERT_EQUAL(LV_SIZE_CONTENT, lv_obj_get_style_width(barcode, LV_PART_MAIN));

    /*No data: nothing to show*/
    lv_obj_update_layout(barcode);
    TEST_ASSERT_EQUAL(0, lv_obj_get_width(barcode));

    lv_barcode_set_data(barcode, "a");
    lv_obj_update_layout(barcode);
    const int32_t bar_cnt = lv_obj_get_width(barcode);
    TEST_ASSERT_GREATER_THAN(0, bar_cnt);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_3_1.png");

    /*The width follows the scale*/
    lv_barcode_set_scale(barcode, 5);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_EQUAL(bar_cnt * 5, lv_obj_get_width(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_3_2.png");

    /*Fixed width: the bars are centered and the padding is respected*/
    lv_obj_set_width(barcode, BARCODE_SIZE);
    lv_obj_set_style_pad_ver(barcode, 10, 0);
    lv_obj_set_style_bg_opa(barcode, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(barcode, lv_color_hex3(0xccc), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_3_3.png");

    /*Can't fit: nothing is drawn*/
    lv_obj_set_width(barcode, 5);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_3_4.png");

    /*Vertical with content sized height*/
    lv_barcode_set_direction(barcode, LV_DIR_VER);
    lv_barcode_set_scale(barcode, 2);
    lv_obj_set_height(barcode, LV_SIZE_CONTENT);
    lv_obj_set_width(barcode, BARCODE_THICK);
    lv_obj_update_layout(barcode);
    TEST_ASSERT_EQUAL(bar_cnt * 2 + 20, lv_obj_get_height(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_3_5.png");

    /*Clearing the data shrinks it back*/
    lv_barcode_set_data(barcode, "");
    lv_obj_update_layout(barcode);
    TEST_ASSERT_EQUAL(20, lv_obj_get_height(barcode));
}

void test_barcode_encoding_after_data(void)
{
    lv_obj_t * barcode = barcode_create_hor();

    /*GS1 strips spaces, raw encodes them, so the two need a different scale to fit*/
    lv_barcode_set_data(barcode, "LVGL 10");
    TEST_ASSERT_EQUAL(LV_BARCODE_ENCODING_CODE128_GS1, lv_barcode_get_encoding(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_4.png");

    lv_barcode_set_encoding(barcode, LV_BARCODE_ENCODING_CODE128_RAW);
    TEST_ASSERT_EQUAL(LV_BARCODE_ENCODING_CODE128_RAW, lv_barcode_get_encoding(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_5.png");

    lv_barcode_set_encoding(barcode, LV_BARCODE_ENCODING_CODE128_GS1);
    TEST_ASSERT_EQUAL(LV_BARCODE_ENCODING_CODE128_GS1, lv_barcode_get_encoding(barcode));
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/barcode_4.png");
}

void test_barcode_get_data_returns_the_stored_copy(void)
{
    lv_obj_t * barcode = lv_barcode_create(active_screen);
    TEST_ASSERT_NOT_NULL(barcode);

    /*Nothing set yet*/
    TEST_ASSERT_NULL(lv_barcode_get_data(barcode));

    char data[] = BARCODE_DATA;
    lv_barcode_set_data(barcode, data);

    /*A copy, not the caller's buffer*/
    TEST_ASSERT_NOT_EQUAL(data, lv_barcode_get_data(barcode));
    TEST_ASSERT_EQUAL_STRING(BARCODE_DATA, lv_barcode_get_data(barcode));

    /*Emptying forgets it*/
    lv_barcode_set_data(barcode, "");
    TEST_ASSERT_NULL(lv_barcode_get_data(barcode));
}


#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_barcode_basics(void)
{
}

#endif

#endif
