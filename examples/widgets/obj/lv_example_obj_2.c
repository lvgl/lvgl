#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * Make an object draggable.
 */
void lv_example_obj_2(void)
{
    lv_obj_t * obj;
    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 150, 100);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label, "Drag me");
    lv_obj_center(label);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_DRAGABLE);
}
#endif
