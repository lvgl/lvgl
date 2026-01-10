#include "../../lv_examples.h"
#if LV_USE_FORM && LV_BUILD_EXAMPLES

/**
 * Form with various input types
 */
void lv_example_form_2(void)
{
    /* Create a form */
    lv_obj_t * form = lv_form_create(lv_screen_active());
    lv_obj_set_size(form, 350, LV_SIZE_CONTENT);
    lv_obj_center(form);

    /* Section 1: Basic Info */
    lv_obj_t * sec1 = lv_form_add_section(form, "Basic Information");

    lv_obj_t * ta_name = lv_textarea_create(form);
    lv_form_add_field(sec1, "Name:", ta_name);

    lv_obj_t * dd_gender = lv_dropdown_create(form);
    lv_dropdown_set_options(dd_gender, "Male\nFemale\nOther");
    lv_form_add_field(sec1, "Gender:", dd_gender);

    /* Section 2: Preferences */
    lv_obj_t * sec2 = lv_form_add_section(form, "Preferences");

    lv_obj_t * cb_news = lv_checkbox_create(form);
    lv_checkbox_set_text(cb_news, "Subscribe to newsletter");
    lv_form_add_field(sec2, "Newsletter:", cb_news);

    lv_obj_t * sw_notify = lv_switch_create(form);
    lv_form_add_field(sec2, "Notifications:", sw_notify);

    /* Section 3: Comments */
    lv_obj_t * sec3 = lv_form_add_section(form, "Comments");

    lv_obj_t * ta_comments = lv_textarea_create(form);
    lv_textarea_set_one_line(ta_comments, false);
    lv_textarea_set_placeholder(ta_comments, "Enter your comments");
    lv_obj_set_height(ta_comments, 80);
    lv_form_add_field(sec3, "Message:", ta_comments);
}

#endif
