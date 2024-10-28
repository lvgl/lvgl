#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES
#include <stdio.h>  /* For sprintf() prototype. */

lv_obj_t  * scrolling_panel;

lv_obj_t  * x_func_label;
lv_obj_t  * y_func_label;
lv_obj_t  * top_func_label;
lv_obj_t  * bottom_func_label;
lv_obj_t  * left_func_label;
lv_obj_t  * right_func_label;
lv_obj_t  * event_count_label;

lv_obj_t  * x_value_label;
lv_obj_t  * y_value_label;
lv_obj_t  * top_value_label;
lv_obj_t  * bottom_value_label;
lv_obj_t  * left_value_label;
lv_obj_t  * right_value_label;
lv_obj_t  * event_count_value_label;

lv_obj_t  * save_button;
lv_obj_t  * restore_button;

lv_style_t  value_label_style;
char        x_value_buffer[16];
char        y_value_buffer[16];
char        top_value_buffer[16];
char        bottom_value_buffer[16];
char        left_value_buffer[16];
char        right_value_buffer[16];
char        event_count_buffer[16];
int         event_count;
int         saved_scroll_x;
int         saved_scroll_y;

static void scroll_update_cb(lv_event_t * e);
static void button_event_cb(lv_event_t * e);

static void scroll_update_cb(lv_event_t * e)
{
    LV_UNUSED(e);

    event_count++;

    sprintf(x_value_buffer, "%d", lv_obj_get_scroll_x(scrolling_panel));
    sprintf(y_value_buffer, "%d", lv_obj_get_scroll_y(scrolling_panel));
    sprintf(top_value_buffer, "%d", lv_obj_get_scroll_top(scrolling_panel));
    sprintf(bottom_value_buffer, "%d", lv_obj_get_scroll_bottom(scrolling_panel));
    sprintf(left_value_buffer, "%d", lv_obj_get_scroll_left(scrolling_panel));
    sprintf(right_value_buffer, "%d", lv_obj_get_scroll_right(scrolling_panel));
    sprintf(event_count_buffer, "%d", event_count);
    lv_label_set_text_static(x_value_label, x_value_buffer);
    lv_label_set_text_static(y_value_label, y_value_buffer);
    lv_label_set_text_static(top_value_label, top_value_buffer);
    lv_label_set_text_static(bottom_value_label, bottom_value_buffer);
    lv_label_set_text_static(left_value_label, left_value_buffer);
    lv_label_set_text_static(right_value_label, right_value_buffer);
    lv_label_set_text_static(event_count_value_label, event_count_buffer);
}

void button_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_user_data(e);

    if(obj == save_button) {
        saved_scroll_x = lv_obj_get_scroll_x(scrolling_panel);
        saved_scroll_y = lv_obj_get_scroll_y(scrolling_panel);
    }
    else {
        lv_obj_scroll_to(scrolling_panel, saved_scroll_x, saved_scroll_y, LV_ANIM_ON);
    }
}

/**
 * Demonstrate how scrolling appears automatically
 */
void lv_example_scroll_1(void)
{
    /*Create an object with the new style*/
    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_set_size(panel, 200, 200);
    lv_obj_center(panel);

    lv_obj_t * child;
    lv_obj_t * label;

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 0, 0);
    lv_obj_set_size(child, 70, 70);
    label = lv_label_create(child);
    lv_label_set_text(label, "Zero");
    lv_obj_center(label);

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 160, 80);
    lv_obj_set_size(child, 80, 80);

    lv_obj_t * child2 = lv_button_create(child);
    lv_obj_set_size(child2, 100, 50);

    label = lv_label_create(child2);
    lv_label_set_text(label, "Right");
    lv_obj_center(label);

    child = lv_obj_create(panel);
    lv_obj_set_pos(child, 40, 160);
    lv_obj_set_size(child, 100, 70);
    label = lv_label_create(child);
    lv_label_set_text(label, "Bottom");
    lv_obj_center(label);

    /* Set up labels that show scroll positions and event count. */
    int32_t value_label_width = 32;
    int32_t line_height = 18;
    lv_style_init(&value_label_style);
    lv_style_set_text_align(&value_label_style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_size(&value_label_style, value_label_width, line_height - 1);

    lv_obj_t * scr;
    scr = lv_screen_active();
    x_value_label = lv_label_create(scr);
    y_value_label = lv_label_create(scr);
    top_value_label = lv_label_create(scr);
    bottom_value_label = lv_label_create(scr);
    left_value_label = lv_label_create(scr);
    right_value_label = lv_label_create(scr);
    event_count_value_label = lv_label_create(scr);

    lv_obj_add_style(x_value_label, &value_label_style, 0);
    lv_obj_add_style(y_value_label, &value_label_style, 0);
    lv_obj_add_style(top_value_label, &value_label_style, 0);
    lv_obj_add_style(bottom_value_label, &value_label_style, 0);
    lv_obj_add_style(left_value_label, &value_label_style, 0);
    lv_obj_add_style(right_value_label, &value_label_style, 0);
    lv_obj_add_style(event_count_value_label, &value_label_style, 0);

    x_func_label = lv_label_create(scr);
    y_func_label = lv_label_create(scr);
    top_func_label = lv_label_create(scr);
    bottom_func_label = lv_label_create(scr);
    left_func_label = lv_label_create(scr);
    right_func_label = lv_label_create(scr);
    event_count_label = lv_label_create(scr);

    lv_label_set_text_static(x_func_label, "<-- lv_obj_get_scroll_x(panel)");
    lv_label_set_text_static(y_func_label, "<-- lv_obj_get_scroll_y(panel)");
    lv_label_set_text_static(top_func_label, "<-- lv_obj_get_scroll_top(panel)");
    lv_label_set_text_static(bottom_func_label, "<-- lv_obj_get_scroll_bottom(panel)");
    lv_label_set_text_static(left_func_label, "<-- lv_obj_get_scroll_left(panel)");
    lv_label_set_text_static(right_func_label, "<-- lv_obj_get_scroll_right(panel)");
    lv_label_set_text_static(event_count_label, "<-- event_count");

    int32_t x = 20;
    int32_t y = 4;
    int32_t offset = value_label_width + 8;
    /* When LV_OBJ_FLAG_SCROLL_ELASTIC is cleared, scrolling does not go past edge bounaries. */
    /* lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLL_ELASTIC); */
    lv_obj_set_pos(x_value_label, x, y);
    lv_obj_set_pos(x_func_label, x + offset, y);
    y += line_height;
    lv_obj_set_pos(y_value_label, x, y);
    lv_obj_set_pos(y_func_label, x + offset, y);
    y += line_height;
    lv_obj_set_pos(top_value_label, x, y);
    lv_obj_set_pos(top_func_label, x + offset, y);
    y += line_height;
    lv_obj_set_pos(bottom_value_label, x, y);
    lv_obj_set_pos(bottom_func_label, x + offset, y);
    y += line_height;
    lv_obj_set_pos(left_value_label, x, y);
    lv_obj_set_pos(left_func_label, x + offset, y);
    y += line_height;
    lv_obj_set_pos(right_value_label, x, y);
    lv_obj_set_pos(right_func_label, x + offset, y);
    y += line_height;
    lv_obj_set_pos(event_count_value_label, x, y);
    lv_obj_set_pos(event_count_label, x + offset, y);

    scrolling_panel = panel;  /* For access to panel in `scroll_update_cb()`. */
    scroll_update_cb(NULL);   /* Set initial text in value labels. */
    /* Call `scroll_update_cb` while panel is being scrolled. */
    lv_obj_add_event_cb(panel, scroll_update_cb, LV_EVENT_SCROLL, NULL);

    /* Set up buttons that save and restore scroll position. */
    save_button = lv_button_create(scr);
    restore_button = lv_button_create(scr);
    lv_obj_t * lbl;
    lbl = lv_label_create(save_button);
    lv_label_set_text_static(lbl, "Save");
    lbl = lv_label_create(restore_button);
    lv_label_set_text_static(lbl, "Restore");
    lv_obj_align(save_button, LV_ALIGN_CENTER, 140, -30);
    lv_obj_align(restore_button, LV_ALIGN_CENTER, 150, 30);
    lv_obj_add_event_cb(save_button, button_event_cb, LV_EVENT_CLICKED, save_button);
    lv_obj_add_event_cb(restore_button, button_event_cb, LV_EVENT_CLICKED, restore_button);
}

#endif
