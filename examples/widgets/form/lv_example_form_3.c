#include "../../lv_examples.h"
#if LV_USE_FORM && LV_BUILD_EXAMPLES

static void submit_event_cb(lv_event_t * e)
{
    lv_obj_t * form = lv_event_get_current_target(e);
    lv_form_event_data_t * data = lv_event_get_param(e);

    char buffer[256];
    lv_form_get_data(form, buffer, sizeof(buffer));

    LV_LOG_USER("Form submitted. Valid: %s", data->is_valid ? "Yes" : "No");
    LV_LOG_USER("Data:\n%s", buffer);
}

/**
 * Form with validation and submit event
 */
void lv_example_form_3(void)
{
    /* Create a form */
    lv_obj_t * form = lv_form_create(lv_screen_active());
    lv_obj_set_size(form, 320, LV_SIZE_CONTENT);
    lv_obj_center(form);

    /* Add section */
    lv_obj_t * section = lv_form_add_section(form, "Registration");

    /* Required fields */
    lv_obj_t * ta_username = lv_textarea_create(form);
    lv_textarea_set_placeholder(ta_username, "Username");
    lv_obj_t * field1 = lv_form_add_field(section, "Username:", ta_username);
    lv_form_field_set_required(field1, true);

    lv_obj_t * ta_password = lv_textarea_create(form);
    lv_textarea_set_placeholder(ta_password, "Password");
    lv_textarea_set_password_mode(ta_password, true);
    lv_obj_t * field2 = lv_form_add_field(section, "Password:", ta_password);
    lv_form_field_set_required(field2, true);

    /* Optional field */
    lv_obj_t * ta_email = lv_textarea_create(form);
    lv_textarea_set_placeholder(ta_email, "Optional");
    lv_form_add_field(section, "Email:", ta_email);

    /* Add submit button */
    lv_obj_t * btn_section = lv_form_add_section(form, NULL);
    lv_obj_t * btn = lv_button_create(btn_section);
    lv_obj_set_width(btn, LV_PCT(100));
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Submit");
    lv_obj_center(btn_label);

    /* Add submit event to button */
    lv_obj_add_event_cb(btn, submit_event_cb, LV_EVENT_CLICKED, form);

    /* Also add validation event to form */
    lv_obj_add_event_cb(form, submit_event_cb, LV_EVENT_SUBMIT, NULL);
}

#endif
