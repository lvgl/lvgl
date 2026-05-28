/**
 * @file lv_example_grid_cell_span.c
 */

#include "../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * @title Grid cell span and alignment
 * @brief Position cells inside their track and span multiple tracks.
 *
 * The first row uses different style_grid_cell_x_align / y_align values to show start,
 * center, and end placement within a cell. Two further items use column_span and row_span
 * to occupy multiple tracks at once, demonstrating how a single child can cover several
 * cells of the grid.
 */
void lv_example_grid_cell_span(void)
{
    static lv_style_t style_card;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_card);
        lv_style_set_bg_color(&style_card, lv_color_hex(0xf6f6f6));
        lv_style_set_bg_opa(&style_card, (255 * 100 / 100));
        lv_style_set_pad_all(&style_card, 6);
        lv_style_set_radius(&style_card, 6);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Main grid container for alignment and span cases */
    lv_obj_t * container_1 = lv_obj_create(screen);
    static const int32_t container_1_style_grid_column_dsc_array_0[] = {70, 70, 70, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(container_1, container_1_style_grid_column_dsc_array_0, 0);
    static const int32_t container_1_style_grid_row_dsc_array_1[] = {44, 44, 44, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(container_1, container_1_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_layout(container_1, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(container_1, lv_pct(100), LV_SIZE_CONTENT);
    /* Cell aligned to start/start */
    lv_obj_t * container_2 = lv_obj_create(container_1);
    lv_obj_set_style_grid_cell_x_align(container_2, LV_GRID_ALIGN_START, 0);
    lv_obj_set_style_grid_cell_column_pos(container_2, 0, 0);
    lv_obj_set_style_grid_cell_y_align(container_2, LV_GRID_ALIGN_START, 0);
    lv_obj_set_style_grid_cell_row_pos(container_2, 0, 0);
    lv_obj_set_size(container_2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(container_2, &style_card, 0);
    lv_obj_t * label_1 = lv_label_create(container_2);
    lv_label_set_text(label_1, "start/start");

    /* Cell aligned to center/center */
    lv_obj_t * container_3 = lv_obj_create(container_1);
    lv_obj_set_style_grid_cell_x_align(container_3, LV_GRID_ALIGN_CENTER, 0);
    lv_obj_set_style_grid_cell_column_pos(container_3, 1, 0);
    lv_obj_set_style_grid_cell_y_align(container_3, LV_GRID_ALIGN_CENTER, 0);
    lv_obj_set_style_grid_cell_row_pos(container_3, 0, 0);
    lv_obj_set_size(container_3, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(container_3, &style_card, 0);
    lv_obj_t * label_2 = lv_label_create(container_3);
    lv_label_set_text(label_2, "center");

    /* Cell aligned to end/end */
    lv_obj_t * container_4 = lv_obj_create(container_1);
    lv_obj_set_style_grid_cell_x_align(container_4, LV_GRID_ALIGN_END, 0);
    lv_obj_set_style_grid_cell_column_pos(container_4, 2, 0);
    lv_obj_set_style_grid_cell_y_align(container_4, LV_GRID_ALIGN_END, 0);
    lv_obj_set_style_grid_cell_row_pos(container_4, 0, 0);
    lv_obj_set_size(container_4, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(container_4, &style_card, 0);
    lv_obj_t * label_3 = lv_label_create(container_4);
    lv_label_set_text(label_3, "end/end");

    /* Item spanning two columns */
    lv_obj_t * container_5 = lv_obj_create(container_1);
    lv_obj_set_style_grid_cell_x_align(container_5, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(container_5, 1, 0);
    lv_obj_set_style_grid_cell_column_span(container_5, 2, 0);
    lv_obj_set_style_grid_cell_y_align(container_5, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(container_5, 1, 0);
    lv_obj_set_style_bg_color(container_5, lv_color_hex(0xd8ecff), 0);
    lv_obj_set_style_bg_opa(container_5, (255 * 100 / 100), 0);
    lv_obj_t * label_4 = lv_label_create(container_5);
    lv_label_set_text(label_4, "span 2 columns");
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);

    /* Item spanning two rows */
    lv_obj_t * container_6 = lv_obj_create(container_1);
    lv_obj_set_style_grid_cell_x_align(container_6, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(container_6, 0, 0);
    lv_obj_set_style_grid_cell_y_align(container_6, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(container_6, 1, 0);
    lv_obj_set_style_grid_cell_row_span(container_6, 2, 0);
    lv_obj_set_style_bg_color(container_6, lv_color_hex(0xffe8cc), 0);
    lv_obj_set_style_bg_opa(container_6, (255 * 100 / 100), 0);
    lv_obj_t * label_5 = lv_label_create(container_6);
    lv_label_set_text(label_5, "span\n2 rows");
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
}
#endif
