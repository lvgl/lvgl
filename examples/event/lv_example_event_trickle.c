#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_FLEX

/**
 * @title Event trickle to children
 * @brief Forward pressed state from a flex container down to its children.
 *
 * A 290x200 container with `LV_FLEX_FLOW_ROW_WRAP` holds nine small
 * sub-containers, each with a numbered label. The container is flagged with
 * `LV_OBJ_FLAG_EVENT_TRICKLE` so its input events reach the children; a
 * single white-on-black style is added to the container for
 * `LV_STATE_PRESSED` and to each sub-container for `LV_STATE_FOCUSED`, so
 * pressing the container flips the whole group to the dark style.
 */
void lv_example_event_trickle(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 290, 200);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    static lv_style_t style_black;
    lv_style_init(&style_black);
    lv_style_set_text_color(&style_black, lv_color_white());
    lv_style_set_bg_color(&style_black, lv_color_black());

    /*Enable event trickle-down on the container*/
    lv_obj_add_flag(cont, LV_OBJ_FLAG_EVENT_TRICKLE);

    lv_obj_add_style(cont, &style_black, LV_STATE_PRESSED);

    uint32_t i;
    for(i = 0; i < 9; i++) {
        lv_obj_t * subcont = lv_obj_create(cont);
        lv_obj_set_size(subcont, 70, 50);
        lv_obj_t * label = lv_label_create(subcont);
        lv_label_set_text_fmt(label, "%" LV_PRIu32, i);

        /*Add style to the label when clicked*/
        lv_obj_add_style(subcont, &style_black, LV_STATE_FOCUSED);
    }
}

#endif
