#include "../../lv_examples.h"
#if LV_USE_GRIDNAV && LV_USE_FLEX && LV_BUILD_EXAMPLES

static void event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_obj_t * list = lv_obj_get_parent(obj);
    LV_UNUSED(list); /*If logging is disabled*/
    LV_LOG_USER("Clicked: %s", lv_list_get_button_text(list, obj));
}

/**
 * @title List with section separators
 * @brief Twenty list buttons grouped by `lv_list_add_text` separators under gridnav rollover.
 *
 * A list on the left is registered with `LV_GRIDNAV_CTRL_ROLLOVER`. Every fifth
 * iteration adds a non-focusable separator via `lv_list_add_text`, and each
 * focusable entry is added with `lv_list_add_button` using `LV_SYMBOL_FILE`.
 * Items are removed from the default group so gridnav handles movement, and
 * each item's `LV_EVENT_CLICKED` callback logs its text through
 * `lv_list_get_button_text`. A separate `Button` sits at the right edge.
 */
void lv_example_gridnav_4(void)
{
    /*It's assumed that the default group is set and
     *there is a keyboard indev*/

    lv_obj_t * list = lv_list_create(lv_screen_active());
    lv_gridnav_add(list, LV_GRIDNAV_CTRL_ROLLOVER);
    lv_obj_align(list, LV_ALIGN_LEFT_MID, 10, 0);
    lv_group_add_obj(lv_group_get_default(), list);

    uint32_t i;
    for(i = 0; i < 20; i++) {
        char buf[32];

        /*Add some separators too, they are not focusable by gridnav*/
        if((i % 5) == 0) {
            lv_snprintf(buf, sizeof(buf), "Section %d", i / 5 + 1);
            lv_list_add_text(list, buf);
        }

        lv_snprintf(buf, sizeof(buf), "File %d", i + 1);
        lv_obj_t * item = lv_list_add_button(list, LV_SYMBOL_FILE, buf);
        lv_obj_add_event_cb(item, event_handler, LV_EVENT_CLICKED, NULL);
        lv_group_remove_obj(item);  /*The default group adds it automatically*/
    }

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
}

#endif
