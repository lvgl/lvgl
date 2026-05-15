/**
 * @file lv_example_grid_cell_span.c
 */

#include "../../../../lvgl.h"

/**
 * @title Grid cell span and alignment
 * @brief Position cells inside their track and span multiple tracks.
 *
 * The first row uses different style_grid_cell_x_align / y_align values to show start,
 * center, and end placement within a cell. Two further items use column_span and row_span
 * to occupy multiple tracks at once, demonstrating how a single child can cover several
 * cells of the grid.
 */
void lv_example_grid_cell_span_create(void)
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

    /* 💡 Adjust style_grid_cell_*_span and *_align values to test spanning and per-cell alignment behaviors. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Grid cells: alignment and span");

    /* Main grid container for alignment and span cases */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    static const int32_t lv_obj_1_style_grid_column_dsc_array_0[] = {70, 70, 70, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(lv_obj_1, lv_obj_1_style_grid_column_dsc_array_0, 0);
    static const int32_t lv_obj_1_style_grid_row_dsc_array_1[] = {44, 44, 44, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(lv_obj_1, lv_obj_1_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_layout(lv_obj_1, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(lv_obj_1, lv_pct(100), LV_SIZE_CONTENT);
    /* Cell aligned to start/start */
    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_obj_2, LV_GRID_ALIGN_START, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_obj_2, 0, 0);
    lv_obj_set_style_grid_cell_y_align(lv_obj_2, LV_GRID_ALIGN_START, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_obj_2, 0, 0);
    lv_obj_set_size(lv_obj_2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_2, &style_card, 0);
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_2);
    lv_label_set_text(lv_label_1, "start/start");

    /* Cell aligned to center/center */
    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_obj_3, LV_GRID_ALIGN_CENTER, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_obj_3, 1, 0);
    lv_obj_set_style_grid_cell_y_align(lv_obj_3, LV_GRID_ALIGN_CENTER, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_obj_3, 0, 0);
    lv_obj_set_size(lv_obj_3, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_3, &style_card, 0);
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_3);
    lv_label_set_text(lv_label_2, "center");

    /* Cell aligned to end/end */
    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_obj_4, LV_GRID_ALIGN_END, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_obj_4, 2, 0);
    lv_obj_set_style_grid_cell_y_align(lv_obj_4, LV_GRID_ALIGN_END, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_obj_4, 0, 0);
    lv_obj_set_size(lv_obj_4, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(lv_obj_4, &style_card, 0);
    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_4);
    lv_label_set_text(lv_label_3, "end/end");

    /* Item spanning two columns */
    lv_obj_t * lv_obj_5 = lv_obj_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_obj_5, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_obj_5, 1, 0);
    lv_obj_set_style_grid_cell_column_span(lv_obj_5, 2, 0);
    lv_obj_set_style_grid_cell_y_align(lv_obj_5, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_obj_5, 1, 0);
    lv_obj_set_style_bg_color(lv_obj_5, lv_color_hex(0xd8ecff), 0);
    lv_obj_set_style_bg_opa(lv_obj_5, (255 * 100 / 100), 0);
    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_5);
    lv_label_set_text(lv_label_4, "span 2 columns");
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);

    /* Item spanning two rows */
    lv_obj_t * lv_obj_6 = lv_obj_create(lv_obj_1);
    lv_obj_set_style_grid_cell_x_align(lv_obj_6, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_obj_6, 0, 0);
    lv_obj_set_style_grid_cell_y_align(lv_obj_6, LV_GRID_ALIGN_STRETCH, 0);
    lv_obj_set_style_grid_cell_row_pos(lv_obj_6, 1, 0);
    lv_obj_set_style_grid_cell_row_span(lv_obj_6, 2, 0);
    lv_obj_set_style_bg_color(lv_obj_6, lv_color_hex(0xffe8cc), 0);
    lv_obj_set_style_bg_opa(lv_obj_6, (255 * 100 / 100), 0);
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_6);
    lv_label_set_text(lv_label_5, "span\n2 rows");
    lv_obj_set_align(lv_label_5, LV_ALIGN_CENTER);
}

