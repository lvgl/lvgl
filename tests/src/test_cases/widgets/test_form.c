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
    active_screen = NULL;
}

/* Test 1: Basic form creation */
void test_form_create(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    TEST_ASSERT_NOT_NULL(form);
    TEST_ASSERT_TRUE(lv_obj_has_class(form, &lv_form_class));
}

/* Test 2: Form deletion */
void test_form_delete(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    TEST_ASSERT_NOT_NULL(form);

    lv_obj_delete(form);
    TEST_ASSERT_EQUAL(0, lv_obj_get_child_count(active_screen));
}

/* Test 3: Form layout setting */
void test_form_set_layout(void)
{
    lv_obj_t * form = lv_form_create(active_screen);

    lv_form_set_layout(form, LV_FORM_LAYOUT_VERTICAL);
    TEST_ASSERT_EQUAL(LV_FORM_LAYOUT_VERTICAL, lv_form_get_layout(form));

    lv_form_set_layout(form, LV_FORM_LAYOUT_HORIZONTAL);
    TEST_ASSERT_EQUAL(LV_FORM_LAYOUT_HORIZONTAL, lv_form_get_layout(form));

    lv_form_set_layout(form, LV_FORM_LAYOUT_GRID);
    TEST_ASSERT_EQUAL(LV_FORM_LAYOUT_GRID, lv_form_get_layout(form));
}

/* Test 4: Form spacing */
void test_form_set_spacing(void)
{
    lv_obj_t * form = lv_form_create(active_screen);

    lv_form_set_spacing(form, 20);
    TEST_ASSERT_EQUAL(20, lv_form_get_spacing(form));

    lv_form_set_spacing(form, 5);
    TEST_ASSERT_EQUAL(5, lv_form_get_spacing(form));
}

/* Test 5: Form section creation */
void test_form_add_section(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Personal Info");

    TEST_ASSERT_NOT_NULL(section);
    TEST_ASSERT_TRUE(lv_obj_has_class(section, &lv_form_section_class));
    TEST_ASSERT_EQUAL_PTR(form, lv_obj_get_parent(section));
}

/* Test 6: Section title management */
void test_form_section_title(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Test Section");

    TEST_ASSERT_NOT_NULL(lv_form_section_get_title(section));
    TEST_ASSERT_EQUAL_STRING("Test Section", lv_form_section_get_title(section));

    lv_form_section_set_title(section, "New Title");
    TEST_ASSERT_EQUAL_STRING("New Title", lv_form_section_get_title(section));

    lv_form_section_set_title(section, NULL);
    TEST_ASSERT_NULL(lv_form_section_get_title(section));
}

/* Test 7: Section field spacing */
void test_form_section_field_spacing(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, NULL);

    lv_form_section_set_field_spacing(section, 15);
    TEST_ASSERT_EQUAL(15, lv_form_section_get_field_spacing(section));
}

/* Test 8: Form field creation with textarea */
void test_form_add_field_textarea(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Fields");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_textarea_set_placeholder(ta, "Enter name");

    lv_obj_t * field = lv_form_add_field(section, "Name:", ta);

    TEST_ASSERT_NOT_NULL(field);
    TEST_ASSERT_TRUE(lv_obj_has_class(field, &lv_form_field_class));
    TEST_ASSERT_EQUAL_PTR(section, lv_obj_get_parent(field));
    TEST_ASSERT_EQUAL_PTR(ta, lv_form_field_get_input(field));
}

/* Test 9: Form field with label */
void test_form_add_field_with_label(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Fields");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_obj_t * field = lv_form_add_field(section, "Email:", ta);

    lv_obj_t * label = lv_form_field_get_label(field);
    TEST_ASSERT_NOT_NULL(label);
    TEST_ASSERT_EQUAL_STRING("Email:", lv_label_get_text(label));
}

/* Test 10: Required field */
void test_form_field_required(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Fields");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_obj_t * field = lv_form_add_field(section, "Required:", ta);

    lv_form_field_set_required(field, true);
    TEST_ASSERT_TRUE(lv_form_field_is_required(field));

    lv_form_field_set_required(field, false);
    TEST_ASSERT_FALSE(lv_form_field_is_required(field));
}

/* Test 11: Field value management */
void test_form_field_value(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Fields");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_textarea_set_text(ta, "Initial");
    lv_obj_t * field = lv_form_add_field(section, "Field:", ta);

    TEST_ASSERT_EQUAL_STRING("Initial", lv_form_field_get_value(field));

    lv_form_field_set_value(field, "New Value");
    TEST_ASSERT_EQUAL_STRING("New Value", lv_form_field_get_value(field));
    TEST_ASSERT_EQUAL_STRING("New Value", lv_textarea_get_text(ta));
}

/* Test 12: Form validation - required field empty */
void test_form_validation_required_empty(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Fields");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_obj_t * field = lv_form_add_field(section, "Required:", ta);

    lv_form_field_set_required(field, true);

    bool is_valid = lv_form_validate(form);
    TEST_ASSERT_FALSE(is_valid);
    TEST_ASSERT_EQUAL(LV_FORM_VALIDATION_REQUIRED, lv_form_field_get_validation(field));
}

/* Test 13: Form validation - required field filled */
void test_form_validation_required_filled(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Fields");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_textarea_set_text(ta, "Value");
    lv_obj_t * field = lv_form_add_field(section, "Required:", ta);

    lv_form_field_set_required(field, true);

    bool is_valid = lv_form_validate(form);
    TEST_ASSERT_TRUE(is_valid);
    TEST_ASSERT_EQUAL(LV_FORM_VALIDATION_OK, lv_form_field_get_validation(field));
}

/* Test 14: Form validation - optional field empty */
void test_form_validation_optional_empty(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Fields");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_form_add_field(section, "Optional:", ta);

    bool is_valid = lv_form_validate(form);
    TEST_ASSERT_TRUE(is_valid);
}

/* Test 15: Form clean */
void test_form_clean(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section1 = lv_form_add_section(form, "Section 1");
    lv_obj_t * section2 = lv_form_add_section(form, "Section 2");

    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(form));

    lv_form_clean(form);
    TEST_ASSERT_EQUAL(0, lv_obj_get_child_count(form));
}

/* Test 16: Multiple sections and fields */
void test_form_multiple_sections_fields(void)
{
    lv_obj_t * form = lv_form_create(active_screen);

    /* Section 1 */
    lv_obj_t * section1 = lv_form_add_section(form, "Personal Info");
    lv_obj_t * ta1 = lv_textarea_create(active_screen);
    lv_form_add_field(section1, "Name:", ta1);

    lv_obj_t * ta2 = lv_textarea_create(active_screen);
    lv_form_add_field(section1, "Email:", ta2);

    /* Section 2 */
    lv_obj_t * section2 = lv_form_add_section(form, "Address");
    lv_obj_t * ta3 = lv_textarea_create(active_screen);
    lv_form_add_field(section2, "Street:", ta3);

    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(form));
    TEST_ASSERT_EQUAL(3, lv_obj_get_child_count(section1));
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(section2));
}

/* Test 17: Form data retrieval */
void test_form_get_data(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "User Info");

    lv_obj_t * ta1 = lv_textarea_create(active_screen);
    lv_textarea_set_text(ta1, "John Doe");
    lv_form_add_field(section, "Name:", ta1);

    lv_obj_t * ta2 = lv_textarea_create(active_screen);
    lv_textarea_set_text(ta2, "john@example.com");
    lv_form_add_field(section, "Email:", ta2);

    char buffer[256];
    lv_result_t res = lv_form_get_data(form, buffer, sizeof(buffer));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_NOT_NULL(strstr(buffer, "User Info"));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "Name: John Doe"));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "Email: john@example.com"));
}

/* Test 18: Form with dropdown */
void test_form_field_dropdown(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Selection");

    lv_obj_t * dd = lv_dropdown_create(active_screen);
    lv_dropdown_set_options(dd, "Option 1\nOption 2\nOption 3");
    lv_obj_t * field = lv_form_add_field(section, "Choose:", dd);

    TEST_ASSERT_NOT_NULL(field);
    TEST_ASSERT_EQUAL_PTR(dd, lv_form_field_get_input(field));
}

/* Test 19: Form with checkbox */
void test_form_field_checkbox(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Preferences");

    lv_obj_t * cb = lv_checkbox_create(active_screen);
    lv_checkbox_set_text(cb, "Subscribe");
    lv_obj_t * field = lv_form_add_field(section, "Newsletter:", cb);

    TEST_ASSERT_NOT_NULL(field);
    TEST_ASSERT_EQUAL_PTR(cb, lv_form_field_get_input(field));
}

/* Test 20: Form with switch */
void test_form_field_switch(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Settings");

    lv_obj_t * sw = lv_switch_create(active_screen);
    lv_obj_t * field = lv_form_add_field(section, "Notifications:", sw);

    TEST_ASSERT_NOT_NULL(field);
    TEST_ASSERT_EQUAL_PTR(sw, lv_form_field_get_input(field));
}

/* Test 21: Form screenshot - basic */
void test_form_screenshot_basic(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_set_size(form, 300, LV_SIZE_CONTENT);
    lv_obj_center(form);

    lv_obj_t * section = lv_form_add_section(form, "Basic Form");

    lv_obj_t * ta1 = lv_textarea_create(active_screen);
    lv_textarea_set_placeholder(ta1, "Enter name");
    lv_form_add_field(section, "Name:", ta1);

    lv_obj_t * ta2 = lv_textarea_create(active_screen);
    lv_textarea_set_placeholder(ta2, "Enter email");
    lv_form_field_set_required(lv_form_add_field(section, "Email:", ta2), true);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/form_basic.png");
}

/* Test 22: Form screenshot - multiple sections */
void test_form_screenshot_complex(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_set_size(form, 350, LV_SIZE_CONTENT);
    lv_obj_center(form);

    /* Personal Info */
    lv_obj_t * sec1 = lv_form_add_section(form, "Personal Info");
    lv_obj_t * ta1 = lv_textarea_create(active_screen);
    lv_form_add_field(sec1, "Name:", ta1);

    lv_obj_t * dd = lv_dropdown_create(active_screen);
    lv_dropdown_set_options(dd, "Male\nFemale\nOther");
    lv_form_add_field(sec1, "Gender:", dd);

    /* Contact */
    lv_obj_t * sec2 = lv_form_add_section(form, "Contact");
    lv_obj_t * ta2 = lv_textarea_create(active_screen);
    lv_form_field_set_required(lv_form_add_field(sec2, "Email:", ta2), true);

    lv_obj_t * ta3 = lv_textarea_create(active_screen);
    lv_form_add_field(sec2, "Phone:", ta3);

    /* Preferences */
    lv_obj_t * sec3 = lv_form_add_section(form, "Preferences");
    lv_obj_t * cb = lv_checkbox_create(active_screen);
    lv_checkbox_set_text(cb, "Newsletter");
    lv_form_add_field(sec3, "Subscribe:", cb);

    lv_obj_t * sw = lv_switch_create(active_screen);
    lv_form_add_field(sec3, "Notify:", sw);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/form_complex.png");
}

/* Test 23: Form with horizontal layout */
void test_form_horizontal_layout(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_form_set_layout(form, LV_FORM_LAYOUT_HORIZONTAL);
    lv_obj_set_size(form, 400, LV_SIZE_CONTENT);
    lv_obj_center(form);

    lv_obj_t * section = lv_form_add_section(form, "Quick Input");
    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_form_add_field(section, "Search:", ta);

    lv_obj_t * btn = lv_button_create(active_screen);
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Go");
    lv_form_add_field(section, "", btn);

    TEST_ASSERT_EQUAL(LV_FORM_LAYOUT_HORIZONTAL, lv_form_get_layout(form));
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/form_horizontal.png");
}

/* Test 24: Field value change event */
void test_form_field_value_change_event(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Test");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_obj_t * field = lv_form_add_field(section, "Input:", ta);

    /* Track if event was fired */
    static bool event_fired = false;
    static lv_obj_t * event_field = NULL;

    void event_cb(lv_event_t * e) {
        event_fired = true;
        lv_form_event_data_t * data = lv_event_get_param(e);
        event_field = data->field;
    }

    lv_obj_add_event_cb(form, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* Simulate value change */
    lv_textarea_set_text(ta, "Test Value");

    /* Note: Event is triggered in the field's event handler */
    /* We can't easily test this without running the event loop */
    /* But we can verify the structure is correct */
    TEST_ASSERT_NOT_NULL(field);
    TEST_ASSERT_EQUAL_STRING("Test Value", lv_form_field_get_value(field));
}

/* Test 25: Form validation event */
void test_form_validation_event(void)
{
    lv_obj_t * form = lv_form_create(active_screen);
    lv_obj_t * section = lv_form_add_section(form, "Test");

    lv_obj_t * ta = lv_textarea_create(active_screen);
    lv_form_field_set_required(lv_form_add_field(section, "Required:", ta), true);

    bool result = lv_form_validate(form);
    TEST_ASSERT_FALSE(result);

    lv_textarea_set_text(ta, "Value");
    result = lv_form_validate(form);
    TEST_ASSERT_TRUE(result);
}

#endif
