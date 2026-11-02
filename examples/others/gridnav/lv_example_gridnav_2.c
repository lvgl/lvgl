#include "../../lv_examples.h"
#if LV_USE_GRIDNAV && LV_USE_FLEX && LV_USE_BUTTON && LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * A list is a flex column of full-width buttons. Add one holding an icon and a
 * text label.
 */
static lv_obj_t * list_add_button(lv_obj_t * list, const void * icon, const char * txt)
{
    lv_obj_t * btn = lv_button_create(list);
    lv_obj_set_size(btn, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);

#if LV_USE_IMAGE == 1
    if(icon) {
        lv_obj_t * img = lv_image_create(btn);
        lv_image_set_src(img, icon);
    }
#endif

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, txt);
    lv_obj_set_flex_grow(label, 1);

    return btn;
}

/**
 * Create a list: a flex container that stacks its children in a column.
 */
static lv_obj_t * list_create(lv_obj_t * parent)
{
    lv_obj_t * list = lv_obj_create(parent);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    return list;
}

/**
 * @title Keypad navigation across two lists
 * @brief Side-by-side list widgets with distinct `lv_gridnav_ctrl_t` modes.
 *
 * Two lists, each a `LV_FLEX_FLOW_COLUMN` container built by the static `list_*`
 * helpers, sit at the left and right edges. The left list registers with
 * `LV_GRIDNAV_CTRL_NONE` and is populated with 15 `LV_SYMBOL_FILE` buttons; the
 * right list uses `LV_GRIDNAV_CTRL_ROLLOVER` and holds 15 `LV_SYMBOL_DIRECTORY`
 * buttons. Each list is added to the default group while every item is removed
 * with `lv_group_remove_obj` so gridnav drives focus inside the list.
 */
void lv_example_gridnav_2(void)
{
    /*The example requires that a default group is set and
     *there is a keyboard indev*/
    lv_group_t * group = lv_group_get_default();
    if(!group) {
        LV_LOG_WARN("Gridnav example requires a default group");
        return;
    }

    lv_obj_t * list1 = list_create(lv_screen_active());
    lv_gridnav_add(list1, LV_GRIDNAV_CTRL_NONE);
    lv_obj_set_size(list1, lv_pct(45), lv_pct(80));
    lv_obj_align(list1, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_bg_color(list1, lv_palette_lighten(LV_PALETTE_BLUE, 5), LV_STATE_FOCUSED);
    lv_group_add_obj(group, list1);

    char buf[32];
    uint32_t i;
    for(i = 0; i < 15; i++) {
        lv_snprintf(buf, sizeof(buf), "File %d", i + 1);
        lv_obj_t * item = list_add_button(list1, LV_SYMBOL_FILE, buf);
        lv_obj_set_style_bg_opa(item, 0, 0);
        lv_group_remove_obj(item);   /*Not needed, we use the gridnav instead*/
    }

    lv_obj_t * list2 = list_create(lv_screen_active());
    lv_gridnav_add(list2, LV_GRIDNAV_CTRL_ROLLOVER);
    lv_obj_set_size(list2, lv_pct(45), lv_pct(80));
    lv_obj_align(list2, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_bg_color(list2, lv_palette_lighten(LV_PALETTE_BLUE, 5), LV_STATE_FOCUSED);
    lv_group_add_obj(group, list2);

    for(i = 0; i < 15; i++) {
        lv_snprintf(buf, sizeof(buf), "Folder %d", i + 1);
        lv_obj_t * item = list_add_button(list2, LV_SYMBOL_DIRECTORY, buf);
        lv_obj_set_style_bg_opa(item, 0, 0);
        lv_group_remove_obj(item);
    }
}

#endif
