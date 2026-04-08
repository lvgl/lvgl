#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_led_render_1(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);
    lv_obj_align(led, LV_ALIGN_CENTER, -80, 0);
    lv_led_off(led);

    led  = lv_led_create(active_screen);
    lv_obj_align(led, LV_ALIGN_CENTER, 0, 0);
    lv_led_set_brightness(led, 150);
    lv_led_set_color(led, lv_palette_main(LV_PALETTE_RED));

    led  = lv_led_create(active_screen);
    lv_obj_align(led, LV_ALIGN_CENTER, 80, 0);
    lv_led_on(led);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/led_1.png");
}

void test_led_set_color_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);
    lv_led_t * ledObj = (lv_led_t *)led;

    /* Test by changing colors randomly. */
    lv_led_set_color(led, lv_palette_main(LV_PALETTE_AMBER));
    TEST_ASSERT_EQUAL_COLOR(lv_palette_main(LV_PALETTE_AMBER), ledObj->color);

    lv_led_set_color(led, lv_palette_main(LV_PALETTE_GREY));
    TEST_ASSERT_EQUAL_COLOR(lv_palette_main(LV_PALETTE_GREY), ledObj->color);

    lv_led_set_color(led, lv_palette_main(LV_PALETTE_RED));
    TEST_ASSERT_EQUAL_COLOR(lv_palette_main(LV_PALETTE_RED), ledObj->color);
}

void test_led_set_brightness_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);

    /* Test by changing brightness randomly. */
    lv_led_set_brightness(led, 150);
    TEST_ASSERT_EQUAL(150, lv_led_get_brightness(led));

    lv_led_set_brightness(led, 10);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, lv_led_get_brightness(led));

    lv_led_set_brightness(led, 255);
    TEST_ASSERT_EQUAL(255, lv_led_get_brightness(led));
}

void test_led_on_off_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);

    lv_led_on(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MAX, lv_led_get_brightness(led));

    lv_led_off(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, lv_led_get_brightness(led));

    lv_led_on(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MAX, lv_led_get_brightness(led));
}

void test_led_toggle_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);

    lv_led_on(led);
    lv_led_toggle(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, lv_led_get_brightness(led));
    lv_led_toggle(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MAX, lv_led_get_brightness(led));
    lv_led_toggle(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, lv_led_get_brightness(led));
}

void test_led_get_brightness_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);

    lv_led_on(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MAX, lv_led_get_brightness(led));
    lv_led_off(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, lv_led_get_brightness(led));
    lv_led_set_brightness(led, 150);
    TEST_ASSERT_EQUAL(150, lv_led_get_brightness(led));
}

void test_led_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_led_create(lv_screen_active());
    lv_property_t prop = { };

    /* Test COLOR property */
    prop.id = LV_PROPERTY_LED_COLOR;
    prop.color = lv_palette_main(LV_PALETTE_RED);
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_COLOR(lv_palette_main(LV_PALETTE_RED), lv_obj_get_property(obj, LV_PROPERTY_LED_COLOR).color);

    /* Test BRIGHTNESS property */
    prop.id = LV_PROPERTY_LED_BRIGHTNESS;
    prop.num = 200;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(200, lv_obj_get_property(obj, LV_PROPERTY_LED_BRIGHTNESS).num);
    TEST_ASSERT_EQUAL_INT(200, lv_led_get_brightness(obj));

    lv_obj_delete(obj);
#endif
}

#endif
