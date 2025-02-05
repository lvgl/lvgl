#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_FLEX


/*
 * Circular list implementation based on:
 * Reference: https://blog.csdn.net/TQW4321/article/details/145434819
 */

#define ITEM_HEIGHT     50

static int32_t get_content_height(lv_obj_t * cont)
{
    int32_t h = 0;
    int32_t pad_row = lv_obj_get_style_pad_row(cont, LV_PART_MAIN);
    int total = lv_obj_get_child_count(cont);

    for(int i = 0; i < total; i++) {
        h += lv_obj_get_height(lv_obj_get_child(cont, i));
        if(i < total - 1) h += pad_row;
    }
    return h + lv_obj_get_style_pad_top(cont, LV_PART_MAIN)
           + lv_obj_get_style_pad_bottom(cont, LV_PART_MAIN);
}

static void cont_scroll_event_cb(lv_event_t * e) 
{
    static bool is_adjusting = false;
    lv_obj_t * cont = lv_event_get_target(e);

    if(!is_adjusting) {
        is_adjusting = true;
        int32_t scroll_y = lv_obj_get_scroll_y(cont);
        int32_t cont_h = lv_obj_get_height(cont);
        int32_t content_h = get_content_height(cont);

        const int32_t item_height = ITEM_HEIGHT;
        const int32_t threshold = 0;

        if(scroll_y < -threshold) {
            lv_obj_t * last_child = lv_obj_get_child(cont, lv_obj_get_child_count(cont) - 1);
            lv_obj_move_to_index(last_child, 0);
            lv_obj_scroll_to_y(cont, scroll_y + item_height, LV_ANIM_OFF);
        }
        else if(scroll_y > content_h - cont_h + threshold) {
            lv_obj_t * first_child = lv_obj_get_child(cont, 0);
            lv_obj_move_to_index(first_child, lv_obj_get_child_count(cont) - 1);
            lv_obj_scroll_to_y(cont, scroll_y - item_height, LV_ANIM_OFF);
        }
        is_adjusting = false;
    }
}

static void item_click_event_cb(lv_event_t * e)
{
    int id = (int)lv_event_get_user_data(e);
    LV_LOG_USER("click id = %d", id);
}

void lv_example_scroll_8(void)
{
    // Create scroll container
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 200, 200);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);           // Vertical layout
    lv_obj_add_event_cb(cont, cont_scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_center(cont);

    // Hide scrollbar visuals
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);


    // Create list items
    for(int i = 0; i < 10; i++) {
        lv_obj_t * item = lv_button_create(cont);
        lv_obj_set_size(item, LV_PCT(100), ITEM_HEIGHT);      // Full width, fixed height
        lv_obj_add_event_cb(item, item_click_event_cb, LV_EVENT_SHORT_CLICKED, (void*)(i+1)); // Pass ID

        lv_obj_t * label = lv_label_create(item);
        lv_label_set_text_fmt(label, "Item %"LV_PRIu32, i + 1);
        lv_obj_center(label);
    }
}


#endif
