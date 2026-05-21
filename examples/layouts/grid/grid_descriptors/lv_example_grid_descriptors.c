/**
 * @file lv_example_grid_descriptors.c
 */

#include "../../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * @title Grid descriptors
 * @brief Define fixed-size column and row tracks.
 *
 * style_grid_column_dsc_array and style_grid_row_dsc_array list the size of each track in
 * pixels. The example uses three 70px columns and two 44px rows, and each child positions
 * itself with style_grid_cell_column_pos and style_grid_cell_row_pos.
 */
void lv_example_grid_descriptors(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Main grid container with fixed descriptors */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    static const int32_t lv_obj_1_style_grid_column_dsc_array_0[] = {70, 70, 70, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(lv_obj_1, lv_obj_1_style_grid_column_dsc_array_0, 0);
    static const int32_t lv_obj_1_style_grid_row_dsc_array_1[] = {44, 44, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(lv_obj_1, lv_obj_1_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_layout(lv_obj_1, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(lv_obj_1, lv_pct(100), LV_SIZE_CONTENT);
    /* Row 0 cells */
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_0, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_0, 0, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_0, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_0, 0, 0);
    lv_obj_set_style_bg_color(lv_label_0, lv_color_hex(0x4a90d9), 0);
    lv_obj_set_style_bg_opa(lv_label_0, (255 * 100 / 100), 0);
    lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xffffff), 0);
    lv_label_set_text(lv_label_0, "0,0");

    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_1, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_1, 1, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_1, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_1, 0, 0);
    lv_obj_set_style_bg_color(lv_label_1, lv_color_hex(0x4a90d9), 0);
    lv_obj_set_style_bg_opa(lv_label_1, (255 * 100 / 100), 0);
    lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0xffffff), 0);
    lv_label_set_text(lv_label_1, "1,0");

    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_2, 2, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_2, 0, 0);
    lv_obj_set_style_bg_color(lv_label_2, lv_color_hex(0x4a90d9), 0);
    lv_obj_set_style_bg_opa(lv_label_2, (255 * 100 / 100), 0);
    lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0xffffff), 0);
    lv_label_set_text(lv_label_2, "2,0");

    /* Row 1 cells */
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_3, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_3, 0, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_3, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_3, 1, 0);
    lv_obj_set_style_bg_color(lv_label_3, lv_color_hex(0x27ae60), 0);
    lv_obj_set_style_bg_opa(lv_label_3, (255 * 100 / 100), 0);
    lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0xffffff), 0);
    lv_label_set_text(lv_label_3, "0,1");

    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_4, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_4, 1, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_4, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_4, 1, 0);
    lv_obj_set_style_bg_color(lv_label_4, lv_color_hex(0x27ae60), 0);
    lv_obj_set_style_bg_opa(lv_label_4, (255 * 100 / 100), 0);
    lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xffffff), 0);
    lv_label_set_text(lv_label_4, "1,1");

    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_5, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_5, 2, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_5, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_5, 1, 0);
    lv_obj_set_style_bg_color(lv_label_5, lv_color_hex(0x27ae60), 0);
    lv_obj_set_style_bg_opa(lv_label_5, (255 * 100 / 100), 0);
    lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0xffffff), 0);
    lv_label_set_text(lv_label_5, "2,1");
}
#endif
