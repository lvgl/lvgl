#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_IMAGE

static lv_style_t style1;

static void event_cb(lv_event_t * e)
{
    LV_LOG_USER("Clicked");

    static uint8_t change = 0;
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * obj = lv_obj_get_parent(btn);
    /*Change style properties*/
    if(change) {
        lv_style_set_border_side(&style1, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT);
    }
    else {
        lv_style_set_border_side(&style1, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT);
    }
    change = ~change;
    /*Notify the object, that a property has been changed*/
    lv_obj_invalidate(obj);
}
/**
 * Using style change reports
 */
void lv_example_style_19(void)
{

    lv_style_init(&style1);

    /*Set a background color and a radius*/
    lv_style_set_radius(&style1, 10);
    lv_style_set_bg_opa(&style1, LV_OPA_COVER);
    lv_style_set_bg_color(&style1, lv_palette_lighten(LV_PALETTE_GREY, 1));

    /*Add border to the bottom+right*/
    lv_style_set_border_color(&style1, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_border_width(&style1, 5);
    lv_style_set_border_opa(&style1, LV_OPA_50);
    lv_style_set_border_side(&style1, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style1, 0);
    lv_obj_center(obj);

    /*Create a button, and click event for it to change style properties on demand*/
    lv_obj_t * btn = lv_btn_create(obj);
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    /*Create a label for user friendliness*/
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Click me!");
    lv_obj_center(label);

}

#endif
