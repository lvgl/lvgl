#include "../../../lvgl.h"
#if LV_USE_ROLLER && LV_BUILD_EXAMPLES

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Selected value: %s\n", buf);
    }
}

/**
 * Roller with various alignments and larger text in the selected area
 */
void lv_example_roller_2(void)
{
    /*A style to make the selected option larger*/
    static lv_style_t style_sel;
    lv_style_init(&style_sel);
    lv_style_set_text_font(&style_sel, &lv_font_montserrat_22);

    const char * opts = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10";
    lv_obj_t *roller;

    /*A roller on the left with left aligned text, and custom width*/
    roller = lv_roller_create(lv_scr_act(), NULL);
    lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller, 2);
    lv_obj_set_width(roller, 100);
    lv_obj_add_style(roller, LV_PART_SELECTED, LV_STATE_DEFAULT, &style_sel);
    lv_obj_set_style_text_align(roller, LV_PART_MAIN, LV_STATE_DEFAULT, LV_TEXT_ALIGN_LEFT);
    lv_obj_align(roller, NULL, LV_ALIGN_IN_LEFT_MID, 10, 0);
    lv_obj_add_event_cb(roller, event_handler, NULL);
    lv_roller_set_selected(roller, 2, LV_ANIM_OFF);

    /*A roller on the middle with center aligned text, and auto (default) width*/
    roller = lv_roller_create(lv_scr_act(), NULL);
    lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_add_style(roller, LV_PART_SELECTED, LV_STATE_DEFAULT, &style_sel);
    lv_obj_align(roller, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(roller, event_handler, NULL);
    lv_roller_set_selected(roller, 5, LV_ANIM_OFF);

    /*A roller on the right with right aligned text, and custom width*/
    roller = lv_roller_create(lv_scr_act(), NULL);
    lv_roller_set_options(roller, opts, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller, 4);
    lv_obj_set_width(roller, 80);
    lv_obj_add_style(roller, LV_PART_SELECTED, LV_STATE_DEFAULT, &style_sel);
    lv_obj_set_style_text_align(roller, LV_PART_MAIN, LV_STATE_DEFAULT, LV_TEXT_ALIGN_RIGHT);
    lv_obj_align(roller, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 0);
    lv_obj_add_event_cb(roller, event_handler, NULL);
    lv_roller_set_selected(roller, 8, LV_ANIM_OFF);
}

#endif
