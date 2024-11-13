#if LV_BUILD_TEST == 1
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static int32_t prev_v;
static int32_t current_v;

static void observer_int(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    prev_v = lv_subject_get_previous_int(subject);
    current_v = lv_subject_get_int(subject);
}

void test_observer_add_remove(void)
{
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 5);

    lv_observer_t * observer = lv_subject_add_observer(&subject, observer_int, NULL);

    current_v = 0;
    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL(10, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, current_v);

    lv_observer_remove(observer);
    lv_subject_set_int(&subject, 15);
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, current_v);   /*The observer cb is not called*/

    static lv_subject_t uninitialized_subject;
    observer = lv_subject_add_observer(&uninitialized_subject, observer_int, NULL);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);   /*The observer must be NULL*/
}

void test_observer_int(void)
{
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 5);
    TEST_ASSERT_EQUAL(5, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(5, lv_subject_get_previous_int(&subject));

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL(10, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(5, lv_subject_get_previous_int(&subject));

    lv_subject_set_int(&subject, 15);
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, lv_subject_get_previous_int(&subject));

    /*Ignore incorrect types*/
    lv_subject_set_pointer(&subject, NULL);
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, lv_subject_get_previous_int(&subject));

    lv_subject_set_color(&subject, lv_color_black());
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, lv_subject_get_previous_int(&subject));

    lv_subject_copy_string(&subject, "hello");
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, lv_subject_get_previous_int(&subject));
}

void test_observer_string(void)
{
    char buf_current[32];
    char buf_previous[32];
    lv_subject_t subject;
    lv_subject_init_string(&subject, buf_current, buf_previous, sizeof(buf_current), "hello");
    TEST_ASSERT_EQUAL_STRING("hello", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("hello", lv_subject_get_previous_string(&subject));

    lv_subject_copy_string(&subject, "my name is John");
    TEST_ASSERT_EQUAL_STRING("my name is John", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("hello", lv_subject_get_previous_string(&subject));

    lv_subject_copy_string(&subject, "how are you?");
    TEST_ASSERT_EQUAL_STRING("how are you?", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("my name is John", lv_subject_get_previous_string(&subject));

    lv_subject_snprintf(&subject, "I ate %d pizzas", 10);
    TEST_ASSERT_EQUAL_STRING("I ate 10 pizzas", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("how are you?", lv_subject_get_previous_string(&subject));

    lv_subject_snprintf(&subject, "%d: %s", 1, "Coding is fun !");
    TEST_ASSERT_EQUAL_STRING("1: Coding is fun !", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("I ate 10 pizzas", lv_subject_get_previous_string(&subject));



    /*Clip long text*/
    lv_subject_copy_string(&subject, "text to be clipped to 32 chars.this should be clipped");
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("1: Coding is fun !", lv_subject_get_previous_string(&subject));

    /*Check if the previous string is clipped correctly*/
    lv_subject_copy_string(&subject, "a");
    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.", lv_subject_get_previous_string(&subject));

    /*Ignore incorrect types*/
    lv_subject_set_pointer(&subject, NULL);
    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.", lv_subject_get_previous_string(&subject));

    lv_subject_set_color(&subject, lv_color_black());
    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.", lv_subject_get_previous_string(&subject));

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.", lv_subject_get_previous_string(&subject));
}

void test_observer_pointer(void)
{
    static int32_t a[3] = {0};
    static lv_subject_t subject;

    lv_subject_init_pointer(&subject, &a[0]);
    TEST_ASSERT_EQUAL_PTR(&a[0], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[0], lv_subject_get_previous_pointer(&subject));

    lv_subject_set_pointer(&subject, &a[1]);
    TEST_ASSERT_EQUAL_PTR(&a[1], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[0], lv_subject_get_previous_pointer(&subject));

    lv_subject_set_pointer(&subject, &a[2]);
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[1], lv_subject_get_previous_pointer(&subject));

    /*Ignore incorrect types*/
    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[1], lv_subject_get_previous_pointer(&subject));

    lv_subject_set_color(&subject, lv_color_black());
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[1], lv_subject_get_previous_pointer(&subject));

    lv_subject_copy_string(&subject, "hello");
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[1], lv_subject_get_previous_pointer(&subject));
}

void test_observer_color(void)
{
    static lv_subject_t subject;

    lv_subject_init_color(&subject, lv_color_hex3(0x123));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x123), lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x123), lv_subject_get_previous_color(&subject));

    lv_subject_set_color(&subject, lv_color_hex3(0x456));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x456), lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x123), lv_subject_get_previous_color(&subject));

    lv_subject_set_color(&subject, lv_color_hex3(0xabc));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc), lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x456), lv_subject_get_previous_color(&subject));

    /*Ignore incorrect types*/
    lv_subject_set_pointer(&subject, NULL);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc), lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x456), lv_subject_get_previous_color(&subject));

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc), lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x456), lv_subject_get_previous_color(&subject));

    lv_subject_copy_string(&subject, "hello");
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc), lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x456), lv_subject_get_previous_color(&subject));
}

static int32_t group_observer_called;

static void group_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);
    group_observer_called++;
}

void test_observer_group(void)
{
    static lv_subject_t subject_main;
    static lv_subject_t subject_sub1;
    static lv_subject_t subject_sub2;
    static lv_subject_t * subject_list[2] = {&subject_sub1, &subject_sub2};

    lv_subject_init_int(&subject_sub1, 1);
    lv_subject_init_int(&subject_sub2, 2);

    lv_subject_init_group(&subject_main, subject_list, 2);
    TEST_ASSERT_EQUAL_PTR(&subject_sub1, lv_subject_get_group_element(&subject_main, 0));
    TEST_ASSERT_EQUAL_PTR(&subject_sub2, lv_subject_get_group_element(&subject_main, 1));
    TEST_ASSERT_EQUAL_PTR(NULL, lv_subject_get_group_element(&subject_main, 2));
    TEST_ASSERT_EQUAL_PTR(NULL, lv_subject_get_group_element(&subject_main, 1000));

    group_observer_called = 0;
    lv_subject_add_observer(&subject_main, group_observer_cb, NULL);
    TEST_ASSERT_EQUAL(1, group_observer_called);

    lv_subject_set_int(&subject_sub1, 10);
    TEST_ASSERT_EQUAL(2, group_observer_called);

    lv_subject_set_int(&subject_sub2, 20);
    TEST_ASSERT_EQUAL(3, group_observer_called);
}

void test_observer_obj_flag(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    /*Can bind only to int*/
    static lv_subject_t subject_wrong;
    lv_subject_init_pointer(&subject_wrong, NULL);
    lv_observer_t * observer = lv_obj_bind_state_if_eq(obj, &subject_wrong, LV_STATE_CHECKED, 5);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_flag_if_eq(obj, &subject, LV_OBJ_FLAG_HIDDEN, 5);
    /*Should be applied immediately*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_obj_bind_flag_if_not_eq(obj, &subject, LV_OBJ_FLAG_CHECKABLE, 10);
    /*Should be applied immediately*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE));

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE));
}

void test_observer_obj_state(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    /*Can bind only to int*/
    static lv_subject_t subject_wrong;
    lv_subject_init_pointer(&subject_wrong, NULL);
    lv_observer_t * observer = lv_obj_bind_state_if_eq(obj, &subject_wrong, LV_STATE_CHECKED, 5);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_state_if_eq(obj, &subject, LV_STATE_CHECKED, 5);
    /*Should be applied immediately*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_obj_bind_state_if_not_eq(obj, &subject, LV_STATE_DISABLED, 10);
    /*Should be applied immediately*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_DISABLED));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_DISABLED));

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_DISABLED));
}

void test_observer_button_checked(void)
{
    lv_obj_t * obj = lv_button_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_update_layout(obj);

    /*Can bind only to int*/
    static lv_subject_t subject_wrong;
    lv_subject_init_pointer(&subject_wrong, NULL);
    lv_observer_t * observer = lv_obj_bind_checked(obj, &subject_wrong);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);
    lv_obj_bind_checked(obj, &subject);

    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 0);
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_test_mouse_click_at(10, 10);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL(1, lv_subject_get_int(&subject));
}

void test_observer_label_text_normal(void)
{
    lv_obj_t * obj = lv_label_create(lv_screen_active());

    lv_observer_t * observer;

    /*Cannot bind color*/
    static lv_subject_t subject_color;
    lv_subject_init_color(&subject_color, lv_color_black());
    observer = lv_label_bind_text(obj, &subject_color, NULL);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    /*Cannot bind int*/
    static lv_subject_t subject_int;
    lv_subject_init_int(&subject_int, 0);
    observer = lv_label_bind_text(obj, &subject_int, NULL);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    /*Bind to string*/
    static char buf[32];
    static lv_subject_t subject_string;
    lv_subject_init_string(&subject_string, buf, NULL, 32, "hello");
    lv_label_bind_text(obj, &subject_string, NULL);
    TEST_ASSERT_EQUAL_STRING("hello", lv_label_get_text(obj));

    lv_subject_copy_string(&subject_string, "world");
    TEST_ASSERT_EQUAL_STRING("world", lv_label_get_text(obj));

    /*Remove the label from the subject*/
    lv_obj_remove_from_subject(obj, &subject_string);
    lv_subject_copy_string(&subject_string, "nothing");
    TEST_ASSERT_EQUAL_STRING("world", lv_label_get_text(obj));

    /*Bind to pointer*/
    static lv_subject_t subject_pointer;
    lv_subject_init_pointer(&subject_pointer, "HELLO");
    lv_label_bind_text(obj, &subject_pointer, NULL);
    TEST_ASSERT_EQUAL_STRING("HELLO", lv_label_get_text(obj));

    lv_subject_set_pointer(&subject_pointer, "WORLD");
    TEST_ASSERT_EQUAL_STRING("WORLD", lv_label_get_text(obj));

    /*Remove the label from the subject*/
    lv_obj_remove_from_subject(obj, &subject_pointer);
    lv_subject_copy_string(&subject_pointer, "NOTHING");
    TEST_ASSERT_EQUAL_STRING("WORLD", lv_label_get_text(obj));
}

void test_observer_label_text_formatted(void)
{
    lv_obj_t * obj = lv_label_create(lv_screen_active());

    lv_observer_t * observer;

    /*Cannot bind color*/
    static lv_subject_t subject_color;
    lv_subject_init_color(&subject_color, lv_color_black());
    observer = lv_label_bind_text(obj, &subject_color, NULL);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    /*Bind to int*/
    static lv_subject_t subject_int;
    lv_subject_init_int(&subject_int, 10);
    lv_label_bind_text(obj, &subject_int, "value: %d");
    TEST_ASSERT_EQUAL_STRING("value: 10", lv_label_get_text(obj));

    lv_subject_set_int(&subject_int, -20);
    TEST_ASSERT_EQUAL_STRING("value: -20", lv_label_get_text(obj));

    /*Remove the label from the subject*/
    lv_obj_remove_from_subject(obj, &subject_int);
    lv_subject_set_int(&subject_int, 100);
    TEST_ASSERT_EQUAL_STRING("value: -20", lv_label_get_text(obj));

    /*Bind to string*/
    static char buf[32];
    static lv_subject_t subject_string;
    lv_subject_init_string(&subject_string, buf, NULL, 32, "hello");
    lv_label_bind_text(obj, &subject_string, "text: %s");
    TEST_ASSERT_EQUAL_STRING("text: hello", lv_label_get_text(obj));

    lv_subject_copy_string(&subject_string, "world");
    TEST_ASSERT_EQUAL_STRING("text: world", lv_label_get_text(obj));

    /*Remove the label from the subject*/
    lv_obj_remove_from_subject(obj, &subject_string);
    lv_subject_copy_string(&subject_string, "nothing");
    TEST_ASSERT_EQUAL_STRING("text: world", lv_label_get_text(obj));

    /*Bind to pointer*/
    static lv_subject_t subject_pointer;
    lv_subject_init_pointer(&subject_pointer, "HELLO");
    lv_label_bind_text(obj, &subject_pointer, "pointer: %s");
    TEST_ASSERT_EQUAL_STRING("pointer: HELLO", lv_label_get_text(obj));

    lv_subject_set_pointer(&subject_pointer, "WORLD");
    TEST_ASSERT_EQUAL_STRING("pointer: WORLD", lv_label_get_text(obj));

    /*Remove the label from the subject*/
    lv_obj_remove_from_subject(obj, &subject_pointer);
    lv_subject_copy_string(&subject_pointer, "NOTHING");
    TEST_ASSERT_EQUAL_STRING("pointer: WORLD", lv_label_get_text(obj));
}

void test_observer_arc_value(void)
{
    lv_obj_t * obj = lv_arc_create(lv_screen_active());

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 30);
    lv_arc_bind_value(obj, &subject);

    TEST_ASSERT_EQUAL(30, lv_arc_get_value(obj));

    lv_subject_set_int(&subject, 40);
    TEST_ASSERT_EQUAL(40, lv_arc_get_value(obj));

    lv_obj_update_layout(obj);
    lv_test_mouse_release();
    lv_test_indev_wait(100);

    lv_test_mouse_move_to(65, 10);
    lv_test_mouse_press();
    lv_test_indev_wait(100);
    lv_test_mouse_release();

    TEST_ASSERT_EQUAL(50, lv_arc_get_value(obj));
    TEST_ASSERT_EQUAL(50, lv_subject_get_int(&subject));
}

void test_observer_slider_value(void)
{
    lv_obj_t * obj = lv_slider_create(lv_screen_active());

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 30);
    lv_slider_bind_value(obj, &subject);

    TEST_ASSERT_EQUAL(30, lv_slider_get_value(obj));

    lv_subject_set_int(&subject, 40);
    TEST_ASSERT_EQUAL(40, lv_slider_get_value(obj));

    lv_obj_update_layout(obj);
    lv_test_mouse_release();
    lv_test_indev_wait(100);

    lv_test_mouse_move_to(65, 10);
    lv_test_mouse_press();
    lv_test_indev_wait(100);

    lv_test_mouse_move_to(75, 10);
    lv_test_mouse_press();
    lv_test_indev_wait(100);
    lv_test_mouse_release();

    TEST_ASSERT_EQUAL(29, lv_slider_get_value(obj));
    TEST_ASSERT_EQUAL(29, lv_subject_get_int(&subject));
}

void test_observer_roller_value(void)
{
    lv_obj_t * obj = lv_roller_create(lv_screen_active());

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);
    lv_roller_bind_value(obj, &subject);

    TEST_ASSERT_EQUAL(1, lv_roller_get_selected(obj));

    lv_subject_set_int(&subject, 2);
    TEST_ASSERT_EQUAL(2, lv_roller_get_selected(obj));

    lv_obj_update_layout(obj);
    lv_test_mouse_click_at(30, 10);

    TEST_ASSERT_EQUAL(1, lv_roller_get_selected(obj));
    TEST_ASSERT_EQUAL(1, lv_subject_get_int(&subject));
}

void test_observer_dropdown_value(void)
{
    lv_obj_t * obj = lv_dropdown_create(lv_screen_active());

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);
    lv_dropdown_bind_value(obj, &subject);

    TEST_ASSERT_EQUAL(1, lv_dropdown_get_selected(obj));

    lv_subject_set_int(&subject, 2);
    TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(obj));

    lv_obj_update_layout(obj);
    lv_test_mouse_click_at(30, 10);
    lv_test_mouse_click_at(30, 60);

    TEST_ASSERT_EQUAL(0, lv_dropdown_get_selected(obj));
    TEST_ASSERT_EQUAL(0, lv_subject_get_int(&subject));
}

void test_observer_deinit(void)
{
    static lv_subject_t subject;

    uint32_t mem = lv_test_get_free_mem();
    uint32_t i;
    for(i = 0; i < 64; i++) {
        lv_obj_t * obj1 = lv_slider_create(lv_screen_active());
        lv_obj_t * obj2 = lv_slider_create(lv_screen_active());

        lv_subject_init_int(&subject, 30);
        lv_slider_bind_value(obj1, &subject);
        lv_slider_bind_value(obj2, &subject);
        lv_subject_add_observer(&subject, observer_int, NULL);
        lv_obj_delete(obj1);
        lv_subject_deinit(&subject);
        lv_obj_delete(obj2);
    }

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 32);
}

#endif
