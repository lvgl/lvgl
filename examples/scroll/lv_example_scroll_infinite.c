#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_FLEX

static lv_obj_t * high_label;
static lv_obj_t * low_label;
static int32_t top_num;
static int32_t bottom_num;
static bool update_running = false;

static lv_obj_t * load_item(lv_obj_t * parent, int32_t num)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text_fmt(label, "%" LV_PRId32, num);
    return obj;
}

static void update_scroll(lv_obj_t * obj)
{
    /* Re-entrancy guard: lv_obj_scroll_by below re-triggers LV_EVENT_SCROLL. */
    if(update_running) return;
    update_running = true;

    int32_t top_before = top_num;
    int32_t bottom_before_num = bottom_num;

    /* Append items as the bottom edge gets close. */
    while(bottom_num > -30 && lv_obj_get_scroll_bottom(obj) < 200) {
        bottom_num -= 1;
        load_item(obj, bottom_num);
        lv_obj_update_layout(obj);
    }
    /* Prepend items as the top edge gets close, compensating the scroll. */
    while(top_num < 30 && lv_obj_get_scroll_top(obj) < 200) {
        top_num += 1;
        int32_t bottom_before = lv_obj_get_scroll_bottom(obj);
        lv_obj_t * item = load_item(obj, top_num);
        lv_obj_move_to_index(item, 0);
        lv_obj_update_layout(obj);
        lv_obj_scroll_by(obj, 0, bottom_before - lv_obj_get_scroll_bottom(obj), LV_ANIM_OFF);
    }

    /* Drop items that scrolled far out of view. */
    while(lv_obj_get_scroll_bottom(obj) > 600) {
        bottom_num += 1;
        lv_obj_delete(lv_obj_get_child(obj, -1));
        lv_obj_update_layout(obj);
    }
    while(lv_obj_get_scroll_top(obj) > 600) {
        top_num -= 1;
        int32_t bottom_before = lv_obj_get_scroll_bottom(obj);
        lv_obj_delete(lv_obj_get_child(obj, 0));
        lv_obj_update_layout(obj);
        lv_obj_scroll_by(obj, 0, bottom_before - lv_obj_get_scroll_bottom(obj), LV_ANIM_OFF);
    }

    if(top_num != top_before) lv_label_set_text_fmt(high_label, "largest:\n%" LV_PRId32, top_num);
    if(bottom_num != bottom_before_num) lv_label_set_text_fmt(low_label, "smallest:\n%" LV_PRId32, bottom_num);

    update_running = false;
}

static void scroll_cb(lv_event_t * e)
{
    update_scroll(lv_event_get_target_obj(e));
}

/**
 * @title Infinite (virtualized) scrolling
 * @brief Create rows on demand and delete far-off ones while a column scrolls.
 *
 * A column tracks the highest and lowest loaded numbers. The
 * `LV_EVENT_SCROLL` callback appends or prepends rows while either edge
 * is within 200 px of content end, and deletes rows once they are more
 * than 600 px away, compensating each change with `lv_obj_scroll_by` so
 * the view stays steady. This keeps the element count bounded no matter
 * how far the user scrolls.
 */
void lv_example_scroll_infinite(void)
{
    lv_obj_t * scr = lv_screen_active();

    high_label = lv_label_create(scr);
    lv_label_set_text(high_label, "largest:");
    lv_obj_align(high_label, LV_ALIGN_TOP_LEFT, 10, 10);

    low_label = lv_label_create(scr);
    lv_label_set_text(low_label, "smallest:");
    lv_obj_align(low_label, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    lv_obj_t * obj = lv_obj_create(scr);
    lv_obj_set_size(obj, 160, 220);
    lv_obj_align(obj, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

    load_item(obj, 3);
    top_num = 3;
    bottom_num = 3;

    lv_obj_update_layout(obj);
    update_scroll(obj);
    lv_obj_add_event_cb(obj, scroll_cb, LV_EVENT_SCROLL, NULL);
}

#endif