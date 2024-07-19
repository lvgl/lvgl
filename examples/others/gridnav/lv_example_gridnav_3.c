#include "../../lv_examples.h"
#if LV_USE_GRIDNAV && LV_USE_FLEX && LV_BUILD_EXAMPLES

static void cont_sub_event_cb(lv_event_t * e)
{
    uint32_t k = lv_event_get_key(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(k == LV_KEY_ENTER) {
        lv_group_focus_obj(obj);
    }
    else if(k == LV_KEY_ESC) {
        lv_group_focus_next(lv_obj_get_group(obj));
    }

}

/**
 * Nested grid navigations
 */
void lv_example_gridnav_3(void)
{
    /*It's assumed that the default group is set and
     *there is a keyboard indev*/

    lv_obj_t * cont_main = lv_obj_create(lv_screen_active());
    lv_gridnav_add(cont_main, LV_GRIDNAV_CTRL_ROLLOVER | LV_GRIDNAV_CTRL_SCROLL_FIRST);

    /*Only the container needs to be in a group*/
    lv_group_add_obj(lv_group_get_default(), cont_main);

    /*Use flex here, but works with grid or manually placed objects as well*/
    lv_obj_set_flex_flow(cont_main, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_bg_color(cont_main, lv_palette_lighten(LV_PALETTE_BLUE, 5), LV_STATE_FOCUSED);
    lv_obj_set_size(cont_main, lv_pct(80), LV_SIZE_CONTENT);

    lv_obj_t * btn;
    lv_obj_t * label;

    btn = lv_button_create(cont_main);
    lv_group_remove_obj(btn);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Button 1");

    btn = lv_button_create(cont_main);
    lv_group_remove_obj(btn);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Button 2");

    /*Create another container with long text to show how LV_GRIDNAV_CTRL_SCROLL_FIRST works*/
    lv_obj_t * cont_sub1 = lv_obj_create(cont_main);
    lv_obj_set_size(cont_sub1, lv_pct(100), 100);

    label = lv_label_create(cont_sub1);
    lv_obj_set_style_bg_color(cont_sub1, lv_palette_lighten(LV_PALETTE_RED, 5), LV_STATE_FOCUSED);
    lv_obj_set_width(label, lv_pct(100));
    lv_label_set_text(label,
                      "I'm a very long text which is makes my container scrollable. "
                      "As LV_GRIDNAV_FLAG_SCROLL_FIRST is enabled arrow will scroll me first "
                      "and a new objects will be focused only when an edge is reached with the scrolling.\n\n"
                      "This is only some placeholder text to be sure the parent will be scrollable. \n\n"
                      "Hello world!\n"
                      "Hello world!\n"
                      "Hello world!\n"
                      "Hello world!\n"
                      "Hello world!\n"
                      "Hello world!");

    /*Create a third container that can be focused with ENTER and contains another grid nav*/
    lv_obj_t * cont_sub2 = lv_obj_create(cont_main);
    lv_gridnav_add(cont_sub2, LV_GRIDNAV_CTRL_ROLLOVER);
    /*Only the container needs to be in a group*/
    lv_group_add_obj(lv_group_get_default(), cont_sub2);

    lv_obj_add_event_cb(cont_sub2, cont_sub_event_cb, LV_EVENT_KEY, NULL);

    /*Use flex here, but works with grid or manually placed objects as well*/
    lv_obj_set_flex_flow(cont_sub2, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_bg_color(cont_sub2, lv_palette_lighten(LV_PALETTE_RED, 5), LV_STATE_FOCUSED);
    lv_obj_set_size(cont_sub2, lv_pct(100), LV_SIZE_CONTENT);

    label = lv_label_create(cont_sub2);
    lv_label_set_text(label, "Use ENTER/ESC to focus/defocus this container");
    lv_obj_set_width(label, lv_pct(100));

    btn = lv_button_create(cont_sub2);
    lv_group_remove_obj(btn);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Button 3");

    btn = lv_button_create(cont_sub2);
    lv_group_remove_obj(btn);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Button 4");

}

#endif
