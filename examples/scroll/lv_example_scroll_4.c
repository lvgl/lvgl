#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_LIST

/**
 * Styling the scrollbars
 */
void lv_example_scroll_4(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 100);
    lv_obj_center(obj);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label,
                      "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
                      "Etiam dictum, tortor vestibulum lacinia laoreet, mi neque consectetur neque, vel mattis odio dolor egestas ligula. \n"
                      "Sed vestibulum sapien nulla, id convallis ex porttitor nec. \n"
                      "Duis et massa eu libero accumsan faucibus a in arcu. \n"
                      "Ut pulvinar odio lorem, vel tempus turpis condimentum quis. Nam consectetur condimentum sem in auctor. \n"
                      "Sed nisl augue, venenatis in blandit et, gravida ac tortor. \n"
                      "Etiam dapibus elementum suscipit. \n"
                      "Proin mollis sollicitudin convallis. \n"
                      "Integer dapibus tempus arcu nec viverra. \n"
                      "Donec molestie nulla enim, eu interdum velit placerat quis. \n"
                      "Donec id efficitur risus, at molestie turpis. \n"
                      "Suspendisse vestibulum consectetur nunc ut commodo. \n"
                      "Fusce molestie rhoncus nisi sit amet tincidunt. \n"
                      "Suspendisse a nunc ut magna ornare volutpat.");

    /*Remove the style of scrollbar to have clean start*/
    lv_obj_remove_style(obj, NULL, LV_PART_SCROLLBAR | LV_STATE_ANY);

    /*Create a transition the animate the some properties on state change*/
    static const lv_style_prop_t props[] = {LV_STYLE_BG_OPA, LV_STYLE_WIDTH, 0};
    static lv_style_transition_dsc_t trans;
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 200, 0, NULL);

    /*Create a style for the scrollbars*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_width(&style, 4);      /*Width of the scrollbar*/
    lv_style_set_length(&style, 20);    /*Length of the scrollbar*/
    lv_style_set_pad_right(&style, 5);  /*Space from the parallel side*/
    lv_style_set_pad_top(&style, 5);    /*Space from the perpendicular side*/

    lv_style_set_radius(&style, 2);
    lv_style_set_bg_opa(&style, LV_OPA_70);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_border_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 3));
    lv_style_set_border_width(&style, 2);
    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_spread(&style, 2);
    lv_style_set_shadow_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 1));

    lv_style_set_transition(&style, &trans);

    /*Make the scrollbars wider and use 100% opacity when scrolled*/
    static lv_style_t style_scrolled;
    lv_style_init(&style_scrolled);
    lv_style_set_width(&style_scrolled, 8);
    lv_style_set_bg_opa(&style_scrolled, LV_OPA_COVER);

    lv_obj_add_style(obj, &style, LV_PART_SCROLLBAR);
    lv_obj_add_style(obj, &style_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
}

#endif
