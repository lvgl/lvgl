/**
 * @file lv_example_grid_align.c
 */

#include "../../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * @title Grid alignment of tracks
 * @brief Place column and row tracks within free space.
 *
 * Track sizes are smaller than the container, so leftover space is distributed by
 * style_grid_column_align and style_grid_row_align. Here columns use space_between to
 * push them to the edges, while rows align to end and stick to the bottom.
 */
void lv_example_grid_align(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(screen, 8, 0);

    /* 💡 Adjust style_grid_column_align and style_grid_row_align to compare how tracks are placed in free space. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Grid align: tracks in free space");

    /* Grid container with explicit track alignment settings */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    static const int32_t lv_obj_1_style_grid_column_dsc_array_0[] = {56, 56, 56, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(lv_obj_1, lv_obj_1_style_grid_column_dsc_array_0, 0);
    static const int32_t lv_obj_1_style_grid_row_dsc_array_1[] = {36, 36, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(lv_obj_1, lv_obj_1_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_grid_column_align(lv_obj_1, LV_GRID_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_grid_row_align(lv_obj_1, LV_GRID_ALIGN_END, 0);
    lv_obj_set_style_layout(lv_obj_1, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(lv_obj_1, lv_pct(100), 180);
    /* First row */
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_1, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_1, 0, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_1, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_1, 0, 0);
    lv_obj_set_style_bg_color(lv_label_1, lv_color_hex(0xd6eaf8), 0);
    lv_obj_set_style_bg_opa(lv_label_1, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_1, "0,0");

    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_2, 1, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_2, 0, 0);
    lv_obj_set_style_bg_color(lv_label_2, lv_color_hex(0xd6eaf8), 0);
    lv_obj_set_style_bg_opa(lv_label_2, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_2, "1,0");

    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_3, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_3, 2, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_3, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_3, 0, 0);
    lv_obj_set_style_bg_color(lv_label_3, lv_color_hex(0xd6eaf8), 0);
    lv_obj_set_style_bg_opa(lv_label_3, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_3, "2,0");

    /* Second row */
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_4, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_4, 0, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_4, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_4, 1, 0);
    lv_obj_set_style_bg_color(lv_label_4, lv_color_hex(0xd5f5e3), 0);
    lv_obj_set_style_bg_opa(lv_label_4, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_4, "0,1");

    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_5, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_5, 1, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_5, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_5, 1, 0);
    lv_obj_set_style_bg_color(lv_label_5, lv_color_hex(0xd5f5e3), 0);
    lv_obj_set_style_bg_opa(lv_label_5, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_5, "1,1");

    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_6, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_6, 2, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_6, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_6, 1, 0);
    lv_obj_set_style_bg_color(lv_label_6, lv_color_hex(0xd5f5e3), 0);
    lv_obj_set_style_bg_opa(lv_label_6, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_6, "2,1");
}
#endif
