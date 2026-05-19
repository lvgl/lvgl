/**
 * @file lv_example_grid_fr.c
 */

#include "../../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * @title Grid FR units
 * @brief Share remaining space across tracks with fr() weights.
 *
 * The first column has a fixed 60px size; the next two columns use fr(1) and fr(2) and
 * split the remaining width in a 1:2 ratio. The same idea applies to rows: the first row
 * is fixed at 44px and the second uses fr(1) to absorb the leftover height.
 */
void lv_example_grid_fr(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);

    /* 💡 Adjust fr() weights and fixed values to compare how remaining space is split across tracks. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Grid FR units: remaining space distribution");

    /* Main grid container with fixed and FR tracks */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    static const int32_t lv_obj_1_style_grid_column_dsc_array_0[] = {60, LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(lv_obj_1, lv_obj_1_style_grid_column_dsc_array_0, 0);
    static const int32_t lv_obj_1_style_grid_row_dsc_array_1[] = {44, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(lv_obj_1, lv_obj_1_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_layout(lv_obj_1, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(lv_obj_1, lv_pct(100), 190);
    /* Header row showing column proportions */
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_1, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_1, 0, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_1, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_1, 0, 0);
    lv_obj_set_style_bg_color(lv_label_1, lv_color_hex(0xb8d8f8), 0);
    lv_obj_set_style_bg_opa(lv_label_1, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_1, "60px");

    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_2, 1, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_2, 0, 0);
    lv_obj_set_style_bg_color(lv_label_2, lv_color_hex(0x7ec8ff), 0);
    lv_obj_set_style_bg_opa(lv_label_2, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_2, "1fr");

    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_label_3, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_label_3, 2, 0);
    lv_obj_set_style_grid_cell_y_align(lv_label_3, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_label_3, 0, 0);
    lv_obj_set_style_bg_color(lv_label_3, lv_color_hex(0x4daff7), 0);
    lv_obj_set_style_bg_opa(lv_label_3, (255 * 100 / 100), 0);
    lv_label_set_text(lv_label_3, "2fr");

    /* Second row with matching column tracks */
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_obj_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_obj_2, 0, 0);
    lv_obj_set_style_grid_cell_y_align(lv_obj_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_obj_2, 1, 0);
    lv_obj_set_style_bg_color(lv_obj_2, lv_color_hex(0xb8d8f8), 0);
    lv_obj_set_style_bg_opa(lv_obj_2, (255 * 100 / 100), 0);
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_2);
    lv_label_set_text(lv_label_4, "60px");
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);

    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_obj_3, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_obj_3, 1, 0);
    lv_obj_set_style_grid_cell_y_align(lv_obj_3, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_obj_3, 1, 0);
    lv_obj_set_style_bg_color(lv_obj_3, lv_color_hex(0x7ec8ff), 0);
    lv_obj_set_style_bg_opa(lv_obj_3, (255 * 100 / 100), 0);
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_3);
    lv_label_set_text(lv_label_5, "1fr row");
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);

    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_obj_4, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_obj_4, 2, 0);
    lv_obj_set_style_grid_cell_y_align(lv_obj_4, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_obj_4, 1, 0);
    lv_obj_set_style_bg_color(lv_obj_4, lv_color_hex(0x4daff7), 0);
    lv_obj_set_style_bg_opa(lv_obj_4, (255 * 100 / 100), 0);
    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_4);
    lv_label_set_text(lv_label_6, "2fr");
    lv_obj_set_align(lv_label_6, LV_ALIGN_CENTER);
}
#endif
