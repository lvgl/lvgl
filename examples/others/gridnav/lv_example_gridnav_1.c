#include "../../lv_examples.h"
#if LV_USE_GRIDNAV && LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * Demonstrate a a basic grid navigation
 */
void lv_example_gridnav_1(void)
{
    /*It's assumed that the default group is set and
     *there is a keyboard indev*/

    lv_obj_t * cont1 = lv_obj_create(lv_screen_active());
    lv_gridnav_add(cont1, LV_GRIDNAV_CTRL_NONE);

    /*Use flex here, but works with grid or manually placed objects as well*/
    lv_obj_set_flex_flow(cont1, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_bg_color(cont1, lv_palette_lighten(LV_PALETTE_BLUE, 5), LV_STATE_FOCUSED);
    lv_obj_set_size(cont1, lv_pct(50), lv_pct(100));

    /*Only the container needs to be in a group*/
    lv_group_add_obj(lv_group_get_default(), cont1);

    lv_obj_t * label = lv_label_create(cont1);
    lv_label_set_text_fmt(label, "No rollover");

    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_obj_t * obj = lv_button_create(cont1);
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
        lv_group_remove_obj(obj);   /*Not needed, we use the gridnav instead*/

        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%"LV_PRIu32"", i);
        lv_obj_center(label);
    }

    /* Create a second container with rollover grid nav mode.*/

    lv_obj_t * cont2 = lv_obj_create(lv_screen_active());
    lv_gridnav_add(cont2, LV_GRIDNAV_CTRL_ROLLOVER);
    lv_obj_set_style_bg_color(cont2, lv_palette_lighten(LV_PALETTE_BLUE, 5), LV_STATE_FOCUSED);
    lv_obj_set_size(cont2, lv_pct(50), lv_pct(100));
    lv_obj_align(cont2, LV_ALIGN_RIGHT_MID, 0, 0);

    label = lv_label_create(cont2);
    lv_obj_set_width(label, lv_pct(100));
    lv_label_set_text_fmt(label, "Rollover\nUse tab to focus the other container");

    /*Only the container needs to be in a group*/
    lv_group_add_obj(lv_group_get_default(), cont2);

    /*Add and place some children manually*/
    lv_obj_t * ta = lv_textarea_create(cont2);
    lv_obj_set_size(ta, lv_pct(100), 80);
    lv_obj_set_pos(ta, 0, 80);
    lv_group_remove_obj(ta);   /*Not needed, we use the gridnav instead*/

    lv_obj_t * cb = lv_checkbox_create(cont2);
    lv_obj_set_pos(cb, 0, 170);
    lv_group_remove_obj(cb);   /*Not needed, we use the gridnav instead*/

    lv_obj_t * sw1 = lv_switch_create(cont2);
    lv_obj_set_pos(sw1, 0, 200);
    lv_group_remove_obj(sw1);   /*Not needed, we use the gridnav instead*/

    lv_obj_t * sw2 = lv_switch_create(cont2);
    lv_obj_set_pos(sw2, lv_pct(50), 200);
    lv_group_remove_obj(sw2);   /*Not needed, we use the gridnav instead*/
}

#endif
