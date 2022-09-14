#include "../../lv_examples.h"
#if LV_USE_RADIOBTN && LV_BUILD_EXAMPLES

static void event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_radiobtn_t * rb = (lv_radiobtn_t *)obj;
    const char * txt = rb->checked_txt;

    LV_LOG_USER("Selected city: %s", txt);
}


void lv_example_radiobtn_1(void)
{
    /*Create a radiobtn*/
    lv_obj_t * radiobtn1 = lv_radiobtn_create(lv_scr_act());
    lv_obj_set_size(radiobtn1, 110, 220);
    lv_obj_center(radiobtn1);
    lv_obj_add_event_cb(radiobtn1, event_handler, LV_EVENT_CLICKED, NULL);

    /*Add buttons to the radiobtn*/
    lv_obj_t * btn;

    btn = lv_radiobtn_add_item(radiobtn1, "Beijing");
    /*manually check a button, please only check one for a radiobtn*/
    lv_obj_add_state(btn, LV_STATE_CHECKED);
    btn = lv_radiobtn_add_item(radiobtn1, "Tokyo");
    btn = lv_radiobtn_add_item(radiobtn1, "Paris");
    btn = lv_radiobtn_add_item(radiobtn1, "London");
    btn = lv_radiobtn_add_item(radiobtn1, "New York");
}

#endif
