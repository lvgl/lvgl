#include "../../../lvgl.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * Use a built in flex layout and demonstrate flex grow.
 */
void lv_example_flex_3(void)
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_layout(cont, &lv_flex_row_nowrap);

    lv_obj_t * obj;
    obj = lv_obj_create(cont, NULL);
    lv_obj_set_size(obj, 20, 20);           /*Fix size*/

    obj = lv_obj_create(cont, NULL);
    lv_obj_set_height(obj, 30);
    lv_obj_set_flex_grow(obj, 1);           /*1 portion from the free space*/

    obj = lv_obj_create(cont, NULL);
    lv_obj_set_height(obj, 40);
    lv_obj_set_flex_grow(obj, 2);           /*2 portion from the free space*/

    obj = lv_obj_create(cont, NULL);
    lv_obj_set_size(obj, 20, 20);           /*Fix size. It is flushed to the right by the "grow" items*/
}

#endif
