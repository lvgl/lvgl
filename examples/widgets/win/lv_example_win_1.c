#include "../../lv_examples.h"
#if LV_USE_WIN && LV_BUILD_EXAMPLES


static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Button %d clicked", lv_obj_get_child_id(obj));
    }
}

void lv_example_win_1(void)
{
    lv_obj_t * win = lv_win_create(lv_scr_act(), 60);
    lv_win_add_btn(win, LV_SYMBOL_LEFT, 40, event_handler);
    lv_win_add_title(win, "A title");
    lv_win_add_btn(win, LV_SYMBOL_RIGHT, 40, event_handler);
    lv_win_add_btn(win, LV_SYMBOL_CLOSE, 60, event_handler);

    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be aded here*/
    lv_obj_t * label =  lv_label_create(cont);
    lv_label_set_text(label, "This is\n"
                             "a pretty\n"
                             "long text\n"
                             "to see how\n"
                             "the window\n"
                             "becomes\n"
                             "scrollable.\n"
                             "\n"
                             "\n"
                             "Some more\n"
                             "text to be\n"
                             "sure it\n"
                             "overflows. :)");


}

#endif
