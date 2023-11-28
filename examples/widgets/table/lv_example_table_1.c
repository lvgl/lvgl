#include "../../lv_examples.h"
#if LV_USE_TABLE && LV_BUILD_EXAMPLES

static void draw_event_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = draw_task->draw_dsc;
    /*If the cells are drawn...*/
    if(base_dsc->part == LV_PART_ITEMS) {
        uint32_t row = base_dsc->id1;
        uint32_t col = base_dsc->id2;

        /*Make the texts in the first cell center aligned*/
        if(row == 0) {
            if(draw_task->type == LV_DRAW_TASK_TYPE_LABEL) {
                lv_draw_label_dsc_t * label_draw_dsc = draw_task->draw_dsc;
                label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
            }
            if(draw_task->type == LV_DRAW_TASK_TYPE_FILL) {
                lv_draw_rect_dsc_t * rect_draw_dsc = draw_task->draw_dsc;
                rect_draw_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), rect_draw_dsc->bg_color, LV_OPA_20);
                rect_draw_dsc->bg_opa = LV_OPA_COVER;
            }
        }
        /*In the first column align the texts to the right*/
        else if(col == 0) {
            if(draw_task->type == LV_DRAW_TASK_TYPE_LABEL) {
                lv_draw_label_dsc_t * label_draw_dsc = draw_task->draw_dsc;
                label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
            }
        }

        /*Make every 2nd row grayish*/
        if((row != 0 && row % 2) == 0) {
            if(draw_task->type == LV_DRAW_TASK_TYPE_FILL) {
                lv_draw_rect_dsc_t * rect_draw_dsc = draw_task->draw_dsc;
                rect_draw_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), rect_draw_dsc->bg_color, LV_OPA_10);
                rect_draw_dsc->bg_opa = LV_OPA_COVER;
            }
        }
    }
}

void lv_example_table_1(void)
{
    lv_obj_t * table = lv_table_create(lv_screen_active());

    /*Fill the first column*/
    lv_table_set_cell_value(table, 0, 0, "Name");
    lv_table_set_cell_value(table, 1, 0, "Apple");
    lv_table_set_cell_value(table, 2, 0, "Banana");
    lv_table_set_cell_value(table, 3, 0, "Lemon");
    lv_table_set_cell_value(table, 4, 0, "Grape");
    lv_table_set_cell_value(table, 5, 0, "Melon");
    lv_table_set_cell_value(table, 6, 0, "Peach");
    lv_table_set_cell_value(table, 7, 0, "Nuts");

    /*Fill the second column*/
    lv_table_set_cell_value(table, 0, 1, "Price");
    lv_table_set_cell_value(table, 1, 1, "$7");
    lv_table_set_cell_value(table, 2, 1, "$4");
    lv_table_set_cell_value(table, 3, 1, "$6");
    lv_table_set_cell_value(table, 4, 1, "$2");
    lv_table_set_cell_value(table, 5, 1, "$5");
    lv_table_set_cell_value(table, 6, 1, "$1");
    lv_table_set_cell_value(table, 7, 1, "$9");

    /*Set a smaller height to the table. It'll make it scrollable*/
    lv_obj_set_height(table, 200);
    lv_obj_center(table);

    /*Add an event callback to to apply some custom drawing*/
    lv_obj_add_event_cb(table, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
}

#endif
