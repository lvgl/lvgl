/**
 * @file lv_example_checkbox_bind_checked.c
 */

#include "../../lv_examples.h"
#if LV_USE_CHECKBOX && LV_BUILD_EXAMPLES

/**
 * @title Checkbox bind checked + bind flag
 * @brief A checkbox drives an int subject; a sibling label hides when the subject is zero.
 *
 * `bind_checked` connects the checkbox's checked state to `subject_flag` (0 or 1).
 * The note label uses `bind_flag_if_eq` to enable the `hidden` flag while
 * `subject_flag` equals 0 — so the label only shows once the box is ticked.
 */
void lv_example_checkbox_bind_checked(void)
{
    static lv_subject_t subject_flag;

    static bool inited = false;

    if(!inited) {
        lv_subject_init_int(&subject_flag, 0);
        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();

    /* 💡 Toggle the checkbox; the note appears/disappears as `subject_flag` flips between 0 and 1. */
    lv_obj_t * checkbox = lv_checkbox_create(screen);
    lv_checkbox_set_text(checkbox, "Show extra details");
    lv_obj_set_align(checkbox, LV_ALIGN_CENTER);
    lv_obj_bind_checked(checkbox, &subject_flag);

    lv_obj_t * label = lv_label_create(screen);
    lv_label_set_text(label, "Extra details only visible while the box is ticked.");
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_obj_set_y(label, 30);
    lv_obj_bind_flag_if_eq(label, &subject_flag, LV_OBJ_FLAG_HIDDEN, 0);
}
#endif
