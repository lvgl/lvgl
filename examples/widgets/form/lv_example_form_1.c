#include "../../lv_examples.h"
#if LV_USE_FORM && LV_BUILD_EXAMPLES

/**
 * Basic form with text fields
 */
void lv_example_form_1(void)
{
    /* Create a form */
    lv_obj_t * form = lv_form_create(lv_screen_active());
    lv_obj_set_size(form, 300, LV_SIZE_CONTENT);
    lv_obj_center(form);

    /* Add a section */
    lv_obj_t * section = lv_form_add_section(form, "Personal Information");

    /* Add name field */
    lv_obj_t * ta_name = lv_textarea_create(form);
    lv_textarea_set_placeholder(ta_name, "Enter your name");
    lv_form_add_field(section, "Name:", ta_name);

    /* Add email field (required) */
    lv_obj_t * ta_email = lv_textarea_create(form);
    lv_textarea_set_placeholder(ta_email, "Enter your email");
    lv_obj_t * email_field = lv_form_add_field(section, "Email:", ta_email);
    lv_form_field_set_required(email_field, true);

    /* Add phone field */
    lv_obj_t * ta_phone = lv_textarea_create(form);
    lv_textarea_set_placeholder(ta_phone, "Enter your phone");
    lv_form_add_field(section, "Phone:", ta_phone);
}

#endif
