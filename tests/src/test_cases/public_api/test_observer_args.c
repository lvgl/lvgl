#if LV_BUILD_TEST == 1
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t observer_called = 0;

/* Subjects are owned by LVGL, so every case registers the ones it creates here
 * and tearDown() deletes them. */
static lv_subject_t * subjects[16] = {NULL};

static lv_subject_t * subject_create(lv_subject_type_t type)
{
    for(size_t i = 0; i < LV_ARRAYLEN(subjects); ++i) {
        if(subjects[i] == NULL) {
            subjects[i] = lv_subject_create(type);
            TEST_ASSERT_NOT_NULL(subjects[i]);
            return subjects[i];
        }
    }
    TEST_FAIL_MESSAGE("subject pool exhausted");
    return NULL;
}

static void observer_int(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);
    observer_called++;
}

void setUp(void)
{
    observer_called = 0;
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());

    /* Group Subjects first: deleting a group dereferences its elements, so the
     * elements have to outlive it. */
    for(size_t i = 0; i < LV_ARRAYLEN(subjects); ++i) {
        if(subjects[i] && subjects[i]->type == LV_SUBJECT_TYPE_GROUP) {
            lv_subject_delete(subjects[i]);
            subjects[i] = NULL;
        }
    }
    for(size_t i = 0; i < LV_ARRAYLEN(subjects); ++i) {
        lv_subject_delete(subjects[i]);
        subjects[i] = NULL;
    }
}

/*A setter for the wrong type leaves the value and the observers alone*/
void test_observer_args_int_setters_of_other_types_are_ignored(void)
{
    lv_subject_t * subject = subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_int(subject, 15);
    lv_subject_add_observer(subject, observer_int, NULL);
    observer_called = 0;

    lv_subject_set_pointer(subject, NULL);
    lv_subject_set_color(subject, lv_color_black());
    lv_subject_set_string(subject, "hello");

    TEST_ASSERT_EQUAL(15, lv_subject_get_int(subject));
    TEST_ASSERT_EQUAL(0, observer_called);
}

void test_observer_args_float_setters_of_other_types_are_ignored(void)
{
    lv_subject_t * subject = subject_create(LV_SUBJECT_TYPE_FLOAT);
    lv_subject_set_float(subject, 15.75f);
    lv_subject_add_observer(subject, observer_int, NULL);
    observer_called = 0;

    lv_subject_set_pointer(subject, NULL);
    lv_subject_set_color(subject, lv_color_black());
    lv_subject_set_string(subject, "hello");

    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_float(subject));
    TEST_ASSERT_EQUAL(0, observer_called);
}

void test_observer_args_string_setters_of_other_types_are_ignored(void)
{
    static char buf_current[32];
    static char buf_previous[32];
    lv_subject_t * subject = subject_create(LV_SUBJECT_TYPE_STRING);
    lv_subject_set_string_buffer_static(subject, buf_current, buf_previous, sizeof(buf_current));
    lv_subject_set_string(subject, "a");
    lv_subject_add_observer(subject, observer_int, NULL);
    observer_called = 0;

    lv_subject_set_pointer(subject, NULL);
    lv_subject_set_color(subject, lv_color_black());
    lv_subject_set_int(subject, 10);

    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(subject));
    TEST_ASSERT_EQUAL(0, observer_called);
}

void test_observer_args_color_setters_of_other_types_are_ignored(void)
{
    lv_subject_t * subject = subject_create(LV_SUBJECT_TYPE_COLOR);
    lv_subject_set_color(subject, lv_color_hex3(0xabc));
    lv_subject_add_observer(subject, observer_int, NULL);
    observer_called = 0;

    lv_subject_set_pointer(subject, NULL);
    lv_subject_set_int(subject, 10);
    lv_subject_set_string(subject, "hello");

    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc), lv_subject_get_color(subject));
    TEST_ASSERT_EQUAL(0, observer_called);
}

void test_observer_args_pointer_setters_of_other_types_are_ignored(void)
{
    static const int a[] = {1, 2, 3};
    lv_subject_t * subject = subject_create(LV_SUBJECT_TYPE_POINTER);
    lv_subject_set_pointer(subject, (void *)&a[2]);
    lv_subject_add_observer(subject, observer_int, NULL);
    observer_called = 0;

    lv_subject_set_int(subject, 10);
    lv_subject_set_color(subject, lv_color_black());
    lv_subject_set_string(subject, "hello");

    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(subject));
    TEST_ASSERT_EQUAL(0, observer_called);
}

/*An index past the end of the group reports no element*/
void test_observer_args_group_element_out_of_range(void)
{
    static lv_subject_t * list[2];
    for(uint32_t i = 0; i < LV_ARRAYLEN(list); i++) {
        list[i] = subject_create(LV_SUBJECT_TYPE_INT);
        lv_subject_set_int(list[i], (int32_t)i + 1);
    }

    lv_subject_t * group_subject = subject_create(LV_SUBJECT_TYPE_GROUP);
    lv_subject_set_group_list_static(group_subject, list, 2);

    TEST_ASSERT_EQUAL_PTR(NULL, lv_subject_get_group_element(group_subject, 2));
    TEST_ASSERT_EQUAL_PTR(NULL, lv_subject_get_group_element(group_subject, 1000));
    TEST_ASSERT_EQUAL_PTR(NULL, lv_subject_get_group_element(group_subject, -1));
}

/*A subject that was never initialised has no type to observe*/
void test_observer_args_add_observer_to_uninitialized_subject(void)
{
    static lv_subject_t uninitialized_subject;
    TEST_ASSERT_EQUAL_PTR(NULL, lv_subject_add_observer(&uninitialized_subject, observer_int, NULL));
}

/*lv_obj_bind_checked() only accepts an int subject*/
void test_observer_args_bind_checked_to_non_int_subject(void)
{
    lv_obj_t * obj = lv_button_create(lv_screen_active());
    lv_obj_set_checkable(obj, true);

    lv_subject_t * subject_wrong = subject_create(LV_SUBJECT_TYPE_POINTER);
    TEST_ASSERT_EQUAL_PTR(NULL, lv_obj_bind_checked(obj, subject_wrong));
}

/*A string setter on a pointer subject changes nothing, so a bound label stays put*/
void test_observer_args_set_string_to_pointer_subject_is_ignored(void)
{
    lv_obj_t * obj = lv_label_create(lv_screen_active());
    lv_subject_t * subject_pointer = subject_create(LV_SUBJECT_TYPE_POINTER);
    lv_subject_set_pointer(subject_pointer, "HELLO");
    lv_label_bind_text(obj, subject_pointer, NULL);
    TEST_ASSERT_EQUAL_STRING("HELLO", lv_label_get_text(obj));

    lv_subject_set_string(subject_pointer, "NOTHING");
    TEST_ASSERT_EQUAL_STRING("HELLO", lv_label_get_text(obj));
}

void test_observer_obj_bind_bool_invalid(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    lv_subject_t * subject = subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_int(subject, 0);

    TEST_ASSERT_EQUAL_PTR(NULL, lv_obj_bind_bool(NULL, subject, lv_obj_set_hidden));
    TEST_ASSERT_EQUAL_PTR(NULL, lv_obj_bind_bool(obj, NULL, lv_obj_set_hidden));
    TEST_ASSERT_EQUAL_PTR(NULL, lv_obj_bind_bool(obj, subject, NULL));

    lv_obj_delete(obj);
}

void test_observer_obj_flag_invalid_subject(void)
{
    LV_DEPRECATIONS_IGNORE_BEGIN
    typedef lv_observer_t * (*lv_obj_bind_flag_fn)(
        lv_obj_t *, lv_subject_t *, lv_obj_flag_t, int32_t);
    static const lv_obj_bind_flag_fn fns[] = {
        lv_obj_bind_flag_if_eq, lv_obj_bind_flag_if_not_eq,
        lv_obj_bind_flag_if_ge, lv_obj_bind_flag_if_gt,
        lv_obj_bind_flag_if_lt, lv_obj_bind_flag_if_le,
    };
    LV_DEPRECATIONS_IGNORE_END

    lv_obj_t * obj = lv_obj_create(lv_screen_active());


    /* Can only bind to int */
    lv_subject_t * invalid[4];
    invalid[0] = subject_create(LV_SUBJECT_TYPE_POINTER);
    invalid[1] = subject_create(LV_SUBJECT_TYPE_STRING);
    invalid[2] = subject_create(LV_SUBJECT_TYPE_COLOR);
    invalid[3] = subject_create(LV_SUBJECT_TYPE_GROUP);

    for(size_t i = 0; i < LV_ARRAYLEN(fns); ++i) {
        for(size_t j = 0; j < LV_ARRAYLEN(invalid); ++j) {
            TEST_ASSERT_EQUAL_PTR(NULL,
                                  fns[i](obj, invalid[j],
                                         LV_OBJ_FLAG_HIDDEN, 5));
        }
    }
}

void test_observer_obj_state_invalid_subject(void)
{
    LV_DEPRECATIONS_IGNORE_BEGIN
    typedef lv_observer_t * (*lv_obj_bind_state_fn)(
        lv_obj_t *, lv_subject_t *, lv_state_t, int32_t);

    static const lv_obj_bind_state_fn fns[] = {
        lv_obj_bind_state_if_eq, lv_obj_bind_state_if_not_eq,
        lv_obj_bind_state_if_ge, lv_obj_bind_state_if_gt,
        lv_obj_bind_state_if_lt, lv_obj_bind_state_if_le,
    };
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    /* Can only bind to int */
    lv_subject_t * invalid[4];
    invalid[0] = subject_create(LV_SUBJECT_TYPE_POINTER);
    invalid[1] = subject_create(LV_SUBJECT_TYPE_STRING);
    invalid[2] = subject_create(LV_SUBJECT_TYPE_COLOR);
    invalid[3] = subject_create(LV_SUBJECT_TYPE_GROUP);

    for(size_t i = 0; i < LV_ARRAYLEN(fns); ++i) {
        for(size_t j = 0; j < LV_ARRAYLEN(invalid); ++j) {
            TEST_ASSERT_EQUAL_PTR(
                NULL, fns[i](obj, invalid[j], 0, 5));
        }
    }
    LV_DEPRECATIONS_IGNORE_END
}

#endif /*LV_BUILD_TEST*/
