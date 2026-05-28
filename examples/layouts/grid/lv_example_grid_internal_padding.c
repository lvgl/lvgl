/**
 * @file lv_example_grid_internal_padding.c
 */

#include "../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * @title Grid internal padding
 * @brief Set horizontal and vertical gaps between grid cells.
 *
 * style_pad_column inserts space between columns and style_pad_row inserts space between
 * rows. The two are independent, so column gaps and row gaps can differ — here columns
 * are flush together while rows have a large vertical gap.
 */
void lv_example_grid_internal_padding(void)
{
    static lv_style_t style_chip;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_chip);
        lv_style_set_bg_opa(&style_chip, (255 * 100 / 100));
        lv_style_set_text_color(&style_chip, lv_color_hex(0xffffff));
        lv_style_set_pad_all(&style_chip, 6);
        lv_style_set_radius(&style_chip, 4);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 8, 0);

    /* Grid container with explicit row and column gaps */
    lv_obj_t * container = lv_obj_create(screen);
    static const int32_t container_style_grid_column_dsc_array_0[] = {50, 50, 50, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(container, container_style_grid_column_dsc_array_0, 0);
    static const int32_t container_style_grid_row_dsc_array_1[] = {40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(container, container_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_pad_column(container, 0, 0);
    lv_obj_set_style_pad_row(container, 66, 0);
    lv_obj_set_style_layout(container, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(container, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_t * label_1 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_1, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_1, 0, 0);
    lv_obj_set_style_bg_color(label_1, lv_color_hex(0x1f77b4), 0);
    lv_label_set_text(label_1, "One");
    lv_obj_add_style(label_1, &style_chip, 0);

    lv_obj_t * label_2 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_2, 1, 0);
    lv_obj_set_style_grid_cell_row_pos(label_2, 0, 0);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0xff7f0e), 0);
    lv_label_set_text(label_2, "Two");
    lv_obj_add_style(label_2, &style_chip, 0);

    lv_obj_t * label_3 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_3, 2, 0);
    lv_obj_set_style_grid_cell_row_pos(label_3, 0, 0);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0x2ca02c), 0);
    lv_label_set_text(label_3, "Three");
    lv_obj_add_style(label_3, &style_chip, 0);

    lv_obj_t * label_4 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_4, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_4, 1, 0);
    lv_obj_set_style_bg_color(label_4, lv_color_hex(0xd62728), 0);
    lv_label_set_text(label_4, "Four");
    lv_obj_add_style(label_4, &style_chip, 0);

    lv_obj_t * label_5 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_5, 1, 0);
    lv_obj_set_style_grid_cell_row_pos(label_5, 1, 0);
    lv_obj_set_style_bg_color(label_5, lv_color_hex(0x9467bd), 0);
    lv_label_set_text(label_5, "Five");
    lv_obj_add_style(label_5, &style_chip, 0);

    lv_obj_t * label_6 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_6, 2, 0);
    lv_obj_set_style_grid_cell_row_pos(label_6, 1, 0);
    lv_obj_set_style_bg_color(label_6, lv_color_hex(0x17becf), 0);
    lv_label_set_text(label_6, "Six");
    lv_obj_add_style(label_6, &style_chip, 0);
}
#endif
