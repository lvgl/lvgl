#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_event_code_t exp_evt_code;
static bool event_triggered = false;

void setUp(void)
{
    active_screen = lv_scr_act();
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

    /* TODO: Add screen compare assert. */
}

void test_led_set_color_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);
    lv_led_t * ledObj = (lv_led_t *)led;

    /* Test by changing colors randomly. */
    lv_led_set_color(led, lv_palette_main(LV_PALETTE_AMBER));
    TEST_ASSERT_EQUAL(lv_palette_main(LV_PALETTE_AMBER).full, ledObj->color.full);

    lv_led_set_color(led, lv_palette_main(LV_PALETTE_GREY));
    TEST_ASSERT_EQUAL(lv_palette_main(LV_PALETTE_GREY).full, ledObj->color.full);

    lv_led_set_color(led, lv_palette_main(LV_PALETTE_RED));
    TEST_ASSERT_EQUAL(lv_palette_main(LV_PALETTE_RED).full, ledObj->color.full);
}

void test_led_set_brightness_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);
    lv_led_t * ledObj = (lv_led_t *)led;
    
    /* Test by changing brightness randomly. */
    lv_led_set_brightness(led, 150);
    TEST_ASSERT_EQUAL(150, ledObj->bright);

    lv_led_set_brightness(led, 10);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, ledObj->bright);

    lv_led_set_brightness(led, 255);
    TEST_ASSERT_EQUAL(255, ledObj->bright);
}

void test_led_on_off_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);
    lv_led_t * ledObj = (lv_led_t *)led;
    
    lv_led_on(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MAX, ledObj->bright);

    lv_led_off(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, ledObj->bright);

    lv_led_on(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MAX, ledObj->bright);
}

void test_led_toggle_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);
    lv_led_t * ledObj = (lv_led_t *)led;
    
    lv_led_on(led);
    lv_led_toggle(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, ledObj->bright);
    lv_led_toggle(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MAX, ledObj->bright);
    lv_led_toggle(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, ledObj->bright);
}

void test_led_get_brightness_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);
    lv_led_t * ledObj = (lv_led_t *)led;

    lv_led_on(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MAX, lv_led_get_brightness(led));
    lv_led_off(led);
    TEST_ASSERT_EQUAL(LV_LED_BRIGHT_MIN, lv_led_get_brightness(led));
    lv_led_set_brightness(led, 150);
    TEST_ASSERT_EQUAL(150, lv_led_get_brightness(led));
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == exp_evt_code) {
        event_triggered = true;
    }
}
/* Added this test to increase code coverage. */
void test_led_event_draw_main_works(void)
{
    lv_obj_t * led  = lv_led_create(active_screen);    
    lv_obj_add_event_cb(led, event_handler, LV_EVENT_DRAW_MAIN, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_DRAW_MAIN;
    lv_event_send(led, LV_EVENT_DRAW_MAIN, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

#endif
