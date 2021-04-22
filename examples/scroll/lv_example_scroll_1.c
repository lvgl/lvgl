#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * Demonstrate how scrolling appears automatically
 */
void lv_example_scroll_1(void)
{
    /*Create an object with the new style*/
    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, 200, 200);
    lv_obj_center(panel);

    lv_obj_t * child;
    lv_obj_t * label;

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 0, 0);
    label = lv_label_create(child);
    lv_label_set_text(label, "Zero");
    lv_obj_center(label);

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, -40, 100);
    label = lv_label_create(child);
    lv_label_set_text(label, "Left");
    lv_obj_center(label);

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 90, -30);
    label = lv_label_create(child);
    lv_label_set_text(label, "Top");
    lv_obj_center(label);

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 150, 80);
    label = lv_label_create(child);
    lv_label_set_text(label, "Right");
    lv_obj_center(label);

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 60, 170);
    label = lv_label_create(child);
    lv_label_set_text(label, "Bottom");
    lv_obj_center(label);
}

#endif
