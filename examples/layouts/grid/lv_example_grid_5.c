#include "../../../lvgl.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

static void row_gap_anim(lv_obj_t * obj, lv_anim_value_t v)
{
    lv_obj_set_style_pad_row(obj, LV_PART_MAIN, LV_STATE_DEFAULT, v);
}

static void column_gap_anim(lv_obj_t * obj, lv_anim_value_t v)
{
    lv_obj_set_style_pad_column(obj, LV_PART_MAIN, LV_STATE_DEFAULT, v);
}
/**
 * Demonstrate column and row gap
 */
void lv_example_grid_5(void)
{

    /*60x60 cells*/
    static lv_coord_t col_dsc[3] = {60, 60, 60};
    static lv_coord_t row_dsc[3] = {40, 40, 40};

    static lv_grid_t grid;
    lv_grid_init(&grid);
    lv_grid_set_template(&grid, col_dsc, 3, row_dsc, 3);

    /*Create a container with grid*/
    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_layout(cont, &grid);

    lv_obj_t * label;
    lv_obj_t * obj;
    uint32_t i;
    for(i = 0; i < 9; i++) {
        uint8_t col = i % 3;
        uint8_t row = i / 3;

        obj = lv_obj_create(cont, NULL);
        lv_obj_set_grid_cell(obj, LV_GRID_STRETCH, col, 1,
                                 LV_GRID_STRETCH, row, 1);
        label = lv_label_create(obj, NULL);
        lv_label_set_text_fmt(label, "%d,%d", col, row);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, cont);
    lv_anim_set_values(&a, 0, 10);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) row_gap_anim);
    lv_anim_set_time(&a, 500);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) column_gap_anim);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_start(&a);
}

#endif

