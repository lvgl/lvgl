#include "../../lv_examples.h"
#if LV_USE_MSGBOX && LV_BUILD_EXAMPLES

static void dropdown_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * dropdown = lv_event_get_target_obj(e);
    lv_obj_t * msgbox = (lv_obj_t *)lv_event_get_user_data(e);
    lv_obj_t * top_layer = lv_layer_top();
    uint32_t opt = lv_dropdown_get_selected(dropdown);

    /*Blur screen*/
    if(opt == 0) {
        lv_obj_set_style_blur_radius(msgbox, 0, 0);
        lv_obj_set_style_blur_radius(top_layer, 24, 0);
    }
    /*Blur Message box*/
    else {
        lv_obj_set_style_blur_radius(msgbox, 24, 0);
        lv_obj_set_style_blur_radius(top_layer, 0, 0);
    }
}

void lv_example_msgbox_3(void)
{

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Lorem ipsum dolor sit amet, consectetur adipiscing elit."
                      " Ut orci mauris, placerat et euismod eu, ullamcorper eget massa. "
                      "Suspendisse sodales vitae augue ut vestibulum. "
                      "Nunc fringilla leo ut tellus consectetur tincidunt. "
                      "Quisque eu tortor semper odio aliquet congue egestas ac massa. "
                      "Phasellus elit lectus, finibus tempor augue in, elementum lobortis nisl. "
                      "Donec tristique lorem et tincidunt faucibus.\n\n"
                      "Lorem ipsum dolor sit amet, consectetur adipiscing elit."
                      " Ut orci mauris, placerat et euismod eu, ullamcorper eget massa. "
                      "Suspendisse sodales vitae augue ut vestibulum. "
                      "Nunc fringilla leo ut tellus consectetur tincidunt. "
                      "Quisque eu tortor semper odio aliquet congue egestas ac massa. "
                      "Phasellus elit lectus, finibus tempor augue in, elementum lobortis nisl. "
                      "Donec tristique lorem et tincidunt faucibus.");

    lv_obj_set_width(label, lv_pct(60));
    lv_obj_center(label);
    lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), 0);

    lv_obj_t * msgbox1 = lv_msgbox_create(lv_layer_top());
    lv_msgbox_add_title(msgbox1, "Setting");
    lv_msgbox_add_text(msgbox1, "Hello!\n\n"
                       "Scroll the text in the background to see how it behaves.");

    /*Just a little styling on the message box*/
    lv_obj_set_style_bg_opa(msgbox1, LV_OPA_40, 0);
    lv_obj_set_style_bg_opa(lv_msgbox_get_header(msgbox1), LV_OPA_50, 0);
    lv_obj_set_style_bg_color(lv_msgbox_get_header(msgbox1), lv_color_black(), 0);
    lv_obj_set_style_bg_color(msgbox1, lv_color_black(), 0);
    lv_obj_set_style_text_color(msgbox1, lv_color_white(), 0);
    lv_obj_set_style_text_color(lv_msgbox_get_header(msgbox1), lv_color_white(), 0);
    lv_obj_set_style_blur_backdrop(msgbox1, true, 0);

    /*A dropdown to select what to blur*/
    lv_obj_t * dropdown = lv_dropdown_create(lv_layer_top());
    lv_dropdown_set_options(dropdown, "Blur screen\nBlur msgbox");
    lv_obj_set_pos(dropdown, 5, 5);
    lv_obj_add_event_cb(dropdown, dropdown_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, msgbox1);
    /*Also make the list blurred*/
    lv_obj_set_style_blur_radius(lv_dropdown_get_list(dropdown), 24, 0);
    lv_obj_set_style_blur_backdrop(lv_dropdown_get_list(dropdown), true, 0);
    lv_obj_set_style_bg_opa(lv_dropdown_get_list(dropdown), LV_OPA_50, 0);

    /*Send a value changed event to set the initial state*/
    lv_obj_send_event(dropdown, LV_EVENT_VALUE_CHANGED, NULL);


}
#endif
