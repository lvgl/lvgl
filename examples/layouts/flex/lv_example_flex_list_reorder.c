/**
 * @file lv_example_flex_list_reorder.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_USE_BUTTON && LV_USE_LABEL && LV_BUILD_EXAMPLES

/*Both lists are plain flex columns: reordering a row is `lv_obj_move_to_index` on a
 *flex child, no list widget involved.*/

static lv_obj_t * list1;
static lv_obj_t * list2;

static lv_obj_t * currentButton = NULL;

/**
 * Create a list: a flex container that stacks its children in a column.
 */
static lv_obj_t * flex_list_create(lv_obj_t * parent)
{
    lv_obj_t * list = lv_obj_create(parent);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    return list;
}

/**
 * Add a full-width button holding an optional icon and a text label.
 */
static lv_obj_t * flex_list_add_button(lv_obj_t * list, const void * icon, const char * txt)
{
    lv_obj_t * btn = lv_button_create(list);
    lv_obj_set_size(btn, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);

#if LV_USE_IMAGE == 1
    if(icon) {
        lv_obj_t * img = lv_image_create(btn);
        lv_image_set_src(img, icon);
    }
#endif

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, txt);
    lv_obj_set_flex_grow(label, 1);

    return btn;
}

#if LV_USE_LOG
/**
 * Find the text of a list button by looking for its child label.
 */
static const char * flex_list_get_button_text(lv_obj_t * btn)
{
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(btn); i++) {
        lv_obj_t * child = lv_obj_get_child(btn, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            return lv_label_get_text(child);
        }
    }
    return "";
}
#endif /*LV_USE_LOG*/

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(code == LV_EVENT_CLICKED) {
#if LV_USE_LOG
        LV_LOG_USER("Clicked: %s", flex_list_get_button_text(obj));
#endif

        if(currentButton == obj) {
            currentButton = NULL;
        }
        else {
            currentButton = obj;
        }
        lv_obj_t * parent = lv_obj_get_parent(obj);
        int32_t i;
        for(i = 0; i < (int32_t)lv_obj_get_child_count(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == currentButton) {
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else {
                lv_obj_remove_state(child, LV_STATE_CHECKED);
            }
        }
    }
}

static void event_handler_top(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        if(currentButton == NULL) return;
        lv_obj_move_to_index(currentButton, 0);
        lv_obj_scroll_to_view(currentButton, LV_ANIM_ON);
    }
}

static void event_handler_up(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        if(currentButton == NULL) return;
        int32_t index = lv_obj_get_index(currentButton);
        if(index <= 0) return;
        lv_obj_move_to_index(currentButton, index - 1);
        lv_obj_scroll_to_view(currentButton, LV_ANIM_ON);
    }
}

static void event_handler_center(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        if(currentButton == NULL) return;

        lv_obj_t * parent = lv_obj_get_parent(currentButton);
        const int32_t pos = (int32_t)lv_obj_get_child_count(parent) / 2;

        lv_obj_move_to_index(currentButton, pos);

        lv_obj_scroll_to_view(currentButton, LV_ANIM_ON);
    }
}

static void event_handler_dn(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        if(currentButton == NULL) return;
        const int32_t index = lv_obj_get_index(currentButton);

        lv_obj_move_to_index(currentButton, index + 1);
        lv_obj_scroll_to_view(currentButton, LV_ANIM_ON);
    }
}

static void event_handler_bottom(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        if(currentButton == NULL) return;
        lv_obj_move_to_index(currentButton, -1);
        lv_obj_scroll_to_view(currentButton, LV_ANIM_ON);
    }
}

static void event_handler_swap(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        uint32_t cnt = lv_obj_get_child_count(list1);
        for(int i = 0; i < 100; i++) {
            lv_obj_t * obj = lv_obj_get_child(list1, (int32_t)lv_rand(0, cnt - 1));
            lv_obj_move_to_index(obj, (int32_t)lv_rand(0, cnt - 1));
            if(currentButton != NULL) {
                lv_obj_scroll_to_view(currentButton, LV_ANIM_ON);
            }
        }
    }
}

/**
 * @title Reorderable list with control panel
 * @brief Select a row in one flex column and move it with Top, Up, Center, Down, Bottom, or Shuffle.
 *
 * A 60% wide `LV_FLEX_FLOW_COLUMN` container on the left holds 15 `lv_button`
 * rows labeled `Item 0` through `Item 14`; clicking a row marks it as the
 * current selection with `LV_STATE_CHECKED` and clears the state on siblings.
 * A 40% wide column on the right pins Top, Up, Center, Down, Bottom, and
 * Shuffle buttons wired to `LV_EVENT_ALL` handlers that call
 * `lv_obj_move_to_index` and `lv_obj_scroll_to_view` to reposition the selected
 * row, with the Up, Center, Down, and Shuffle handlers also firing on
 * `LV_EVENT_LONG_PRESSED_REPEAT`. Reordering is a flex-child operation, so it
 * needs no list widget.
 */
void lv_example_flex_list_reorder(void)
{
    /*Create a list from a flex column*/
    list1 = flex_list_create(lv_screen_active());
    lv_obj_set_size(list1, lv_pct(60), lv_pct(100));
    lv_obj_set_style_pad_row(list1, 5, 0);

    /*Add buttons to the list*/
    lv_obj_t * btn;
    int i;
    for(i = 0; i < 15; i++) {
        btn = lv_button_create(list1);
        lv_obj_set_width(btn, lv_pct(50));
        lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

        lv_obj_t * lab = lv_label_create(btn);
        lv_label_set_text_fmt(lab, "Item %d", i);
    }

    /*Select the first button by default*/
    currentButton = lv_obj_get_child(list1, 0);
    lv_obj_add_state(currentButton, LV_STATE_CHECKED);

    /*Create a second list with up and down buttons*/
    list2 = flex_list_create(lv_screen_active());
    lv_obj_set_size(list2, lv_pct(40), lv_pct(100));
    lv_obj_align(list2, LV_ALIGN_TOP_RIGHT, 0, 0);

    btn = flex_list_add_button(list2, NULL, "Top");
    lv_obj_add_event_cb(btn, event_handler_top, LV_EVENT_ALL, NULL);
    lv_group_remove_obj(btn);

    btn = flex_list_add_button(list2, LV_SYMBOL_UP, "Up");
    lv_obj_add_event_cb(btn, event_handler_up, LV_EVENT_ALL, NULL);
    lv_group_remove_obj(btn);

    btn = flex_list_add_button(list2, LV_SYMBOL_LEFT, "Center");
    lv_obj_add_event_cb(btn, event_handler_center, LV_EVENT_ALL, NULL);
    lv_group_remove_obj(btn);

    btn = flex_list_add_button(list2, LV_SYMBOL_DOWN, "Down");
    lv_obj_add_event_cb(btn, event_handler_dn, LV_EVENT_ALL, NULL);
    lv_group_remove_obj(btn);

    btn = flex_list_add_button(list2, NULL, "Bottom");
    lv_obj_add_event_cb(btn, event_handler_bottom, LV_EVENT_ALL, NULL);
    lv_group_remove_obj(btn);

    btn = flex_list_add_button(list2, LV_SYMBOL_SHUFFLE, "Shuffle");
    lv_obj_add_event_cb(btn, event_handler_swap, LV_EVENT_ALL, NULL);
    lv_group_remove_obj(btn);
}

#endif
