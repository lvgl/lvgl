/**
 * @file lv_example_grid_subgrid.c
 */

#include "../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * @title Grid sub-grid
 * @brief Nest a grid inside a cell of a parent grid.
 *
 * The outer grid defines 3x2 tracks, and an inner grid occupies a 2x2 area within it. The
 * inner grid intentionally omits its own column/row descriptors so it inherits the parent
 * tracks, keeping its cells aligned with the outer grid's columns and rows.
 */
void lv_example_grid_subgrid(void)
{
    static lv_style_t style_tile;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_tile);
        lv_style_set_bg_opa(&style_tile, (255 * 100 / 100));
        lv_style_set_text_color(&style_tile, lv_color_hex(0xffffff));
        lv_style_set_pad_all(&style_tile, 6);
        lv_style_set_radius(&style_tile, 4);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 8, 0);

    /* Parent grid container */
    lv_obj_t * container_1 = lv_obj_create(screen);
    static const int32_t container_1_style_grid_column_dsc_array_0[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(container_1, container_1_style_grid_column_dsc_array_0, 0);
    static const int32_t container_1_style_grid_row_dsc_array_1[] = {40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(container_1, container_1_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_layout(container_1, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(container_1, lv_pct(100), LV_SIZE_CONTENT);
    /* Label in the parent grid: left tile */
    lv_obj_t * label_1 = lv_label_create(container_1);
    lv_obj_set_style_grid_cell_column_pos(label_1, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_1, 0, 0);
    lv_obj_set_style_bg_color(label_1, lv_color_hex(0x1f77b4), 0);
    lv_label_set_text(label_1, "P0,0");
    lv_obj_add_style(label_1, &style_tile, 0);

    /* Sub-grid container spanning 2 columns and 2 rows
       Do not set the grid descriptor to let them inherit the parent tracks */
    lv_obj_t * container_2 = lv_obj_create(container_1);
    lv_obj_set_style_grid_cell_x_align(container_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(container_2, 1, 0);
    lv_obj_set_style_grid_cell_column_span(container_2, 2, 0);
    lv_obj_set_style_grid_cell_y_align(container_2, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(container_2, 0, 0);
    lv_obj_set_style_grid_cell_row_span(container_2, 2, 0);
    lv_obj_set_style_layout(container_2, LV_LAYOUT_GRID, 0);
    lv_obj_set_style_pad_all(container_2, 4, 0);
    /* Sub-grid label at local cell (0,0) */
    lv_obj_t * label_2 = lv_label_create(container_2);
    lv_obj_set_style_grid_cell_column_pos(label_2, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_2, 0, 0);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0x2ca02c), 0);
    lv_label_set_text(label_2, "S0,0");
    lv_obj_add_style(label_2, &style_tile, 0);

    /* Sub-grid label at local cell (1,0) */
    lv_obj_t * label_3 = lv_label_create(container_2);
    lv_obj_set_style_grid_cell_column_pos(label_3, 1, 0);
    lv_obj_set_style_grid_cell_row_pos(label_3, 0, 0);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0xff7f0e), 0);
    lv_label_set_text(label_3, "S1,0");
    lv_obj_add_style(label_3, &style_tile, 0);

    /* Sub-grid label at local cell (0,1) */
    lv_obj_t * label_4 = lv_label_create(container_2);
    lv_obj_set_style_grid_cell_column_pos(label_4, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_4, 1, 0);
    lv_obj_set_style_bg_color(label_4, lv_color_hex(0xd62728), 0);
    lv_label_set_text(label_4, "S0,1");
    lv_obj_add_style(label_4, &style_tile, 0);

    /* Sub-grid label at local cell (1,1) */
    lv_obj_t * label_5 = lv_label_create(container_2);
    lv_obj_set_style_grid_cell_column_pos(label_5, 1, 0);
    lv_obj_set_style_grid_cell_row_pos(label_5, 1, 0);
    lv_obj_set_style_bg_color(label_5, lv_color_hex(0x9467bd), 0);
    lv_label_set_text(label_5, "S1,1");
    lv_obj_add_style(label_5, &style_tile, 0);
}
#endif
