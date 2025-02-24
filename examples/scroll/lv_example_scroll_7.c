#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_FLEX

static lv_obj_t * high_label;
static lv_obj_t * low_label;
static int32_t top_num;
static int32_t bottom_num;
static bool update_scroll_running = false;

static lv_obj_t * load_item(lv_obj_t * parent, int32_t num)
{
    lv_obj_t * new = lv_obj_create(parent);
    lv_obj_set_size(new, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_t * label = lv_label_create(new);
    lv_label_set_text_fmt(label, "%"PRId32, num);
    return new;
}

static void update_scroll(lv_obj_t * obj)
{
    /* do not re-enter this function when `lv_obj_scroll_by`
     * triggers this callback again.
     */
    if(update_scroll_running) return;
    update_scroll_running = true;

    int32_t top_num_original = top_num;
    int32_t bottom_num_original = bottom_num;

    /* load items we're getting close to */
    while(bottom_num > -30 && lv_obj_get_scroll_bottom(obj) < 200) {
        bottom_num -= 1;
        load_item(obj, bottom_num);
        lv_obj_update_layout(obj);
        LV_LOG_USER("loaded bottom num: %"PRId32, bottom_num);
    }
    while(top_num < 30 && lv_obj_get_scroll_top(obj) < 200) {
        top_num += 1;
        int32_t bottom_before = lv_obj_get_scroll_bottom(obj);
        lv_obj_t * new_item = load_item(obj, top_num);
        lv_obj_move_to_index(new_item, 0);
        lv_obj_update_layout(obj);
        int32_t bottom_after = lv_obj_get_scroll_bottom(obj);
        lv_obj_scroll_by(obj, 0, bottom_before - bottom_after, LV_ANIM_OFF);
        LV_LOG_USER("loaded top num: %"PRId32, top_num);
    }

    /* delete far-away items */
    while(lv_obj_get_scroll_bottom(obj) > 600) {
        bottom_num += 1;
        lv_obj_t * child = lv_obj_get_child(obj, -1);
        lv_obj_delete(child);
        lv_obj_update_layout(obj);
        LV_LOG_USER("deleted bottom num: %"PRId32, bottom_num);
    }
    while(lv_obj_get_scroll_top(obj) > 600) {
        top_num -= 1;
        int32_t bottom_before = lv_obj_get_scroll_bottom(obj);
        lv_obj_t * child = lv_obj_get_child(obj, 0);
        lv_obj_delete(child);
        lv_obj_update_layout(obj);
        int32_t bottom_after = lv_obj_get_scroll_bottom(obj);
        lv_obj_scroll_by(obj, 0, bottom_before - bottom_after, LV_ANIM_OFF);
        LV_LOG_USER("deleted top num: %"PRId32, top_num);
    }

    if(top_num != top_num_original) {
        lv_label_set_text_fmt(high_label, "current largest\nloaded value:\n%"PRId32, top_num);
    }
    if(bottom_num != bottom_num_original) {
        lv_label_set_text_fmt(low_label, "current smallest\nloaded value:\n%"PRId32, bottom_num);
    }

    update_scroll_running = false;
}

static void scroll_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    update_scroll(obj);
}

static void checkbox_cb(lv_event_t * e)
{
    lv_obj_t * checkbox = lv_event_get_target_obj(e);
    lv_obj_t * obj = lv_event_get_user_data(e);
    bool checked = lv_obj_has_state(checkbox, LV_STATE_CHECKED);
    lv_obj_set_style_opa(obj, checked ? LV_OPA_COVER : LV_OPA_TRANSP, LV_PART_SCROLLBAR);
}

/**
 * Dynamically load widgets while scrolling
 */
void lv_example_scroll_7(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_t * obj = lv_obj_create(scr);
    lv_obj_set_size(obj, 160, 220);
    lv_obj_align(obj, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_opa(obj, LV_OPA_TRANSP, LV_PART_SCROLLBAR);

    high_label = lv_label_create(scr);
    lv_label_set_text_static(high_label, "current largest\nloaded value:");
    lv_obj_align(high_label, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_obj_t * checkbox = lv_checkbox_create(scr);
    lv_checkbox_set_text_static(checkbox, "show\nscrollbar");
    lv_obj_align(checkbox, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_event_cb(checkbox, checkbox_cb, LV_EVENT_VALUE_CHANGED, obj);

    low_label = lv_label_create(scr);
    lv_label_set_text_static(low_label, "current smallest\nloaded value:");
    lv_obj_align(low_label, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    load_item(obj, 3);
    /* These counters hold the the highest/lowest number currently loaded. */
    top_num = 3;
    bottom_num = 3;

    lv_obj_update_layout(obj);
    update_scroll(obj);
    lv_obj_add_event_cb(obj, scroll_cb, LV_EVENT_SCROLL, NULL);
}

#endif
