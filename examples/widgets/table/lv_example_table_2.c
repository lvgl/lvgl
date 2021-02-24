#include "../../../lvgl.h"
#if LV_USE_TABLE && LV_BUILD_EXAMPLES

#define ITEM_CNT 200

static void event_cb(lv_obj_t * obj, lv_event_t e)
{
    if(e == LV_EVENT_DRAW_PART_END) {
        lv_obj_draw_hook_dsc_t * hook_dsc = lv_event_get_param();
        /*If the cells are drawn...*/
        if(hook_dsc->part == LV_PART_ITEMS) {
            uint32_t row = hook_dsc->id;

            /*Make the texts in the first cell center aligned*/
            if(row == 0) {
                hook_dsc->label_dsc->align = LV_TEXT_ALIGN_CENTER;
                hook_dsc->rect_dsc->bg_color = lv_color_mix(lv_color_blue(), hook_dsc->rect_dsc->bg_color, LV_OPA_20);
                hook_dsc->rect_dsc->bg_opa = LV_OPA_COVER;
            }

            /*Make every 2nd row grayish*/
            if((row != 0 && row % 2) == 0) {
                hook_dsc->rect_dsc->bg_color = lv_color_mix(lv_color_grey(), hook_dsc->rect_dsc->bg_color, LV_OPA_10);
                hook_dsc->rect_dsc->bg_opa = LV_OPA_COVER;
            }
        }
    }
}


/**
 * A very light-weighted list created from table
 */
void lv_example_table_2(void)
{
    /*Measure memory usage*/
    lv_mem_monitor_t mon1;
    lv_mem_monitor(&mon1);

    uint32_t t = lv_tick_get();

    lv_obj_t * table = lv_table_create(lv_scr_act(), NULL);
    lv_table_set_row_cnt(table, ITEM_CNT); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value */
    lv_table_set_col_cnt(table, 1);

    uint32_t i;
    for(i = 0; i < ITEM_CNT; i++) {
        lv_table_set_cell_value_fmt(table, i, 0, "Item %d", i + 1);
    }

    /*Set a smaller height to the table. It'll make it scrollable*/
    lv_obj_set_height(table, 200);
    lv_obj_align(table, NULL, LV_ALIGN_CENTER, 0, -20);

    /*Add an event callback to to apply some custom drawing*/
//    lv_obj_add_event_cb(table, event_cb, NULL);

    lv_mem_monitor_t mon2;
    lv_mem_monitor(&mon2);

    uint32_t mem_used = mon1.free_size - mon2.free_size;

    uint32_t elaps = lv_tick_elaps(t);

    lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_fmt(label, "%d bytes are used by the table\n"
                                  "and %d items were added in %d ms",
                                  mem_used, ITEM_CNT, elaps);

    lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

}

#endif
