/**
 * @file lv_example_grid_ignore_layout.c
 */

#include "../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * @title Grid ignore layout
 * @brief Detach an item from grid placement.
 *
 * Four normal grid items occupy their cells while a fifth item with ignore_layout="true"
 * is skipped by the grid. The detached item keeps its manual x/y position and does not
 * displace any of the placed cells, useful for floating badges or overlays.
 */
void lv_example_grid_ignore_layout(void)
{
    static lv_style_t style_cell;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_cell);
        lv_style_set_bg_opa(&style_cell, (255 * 100 / 100));
        lv_style_set_text_color(&style_cell, lv_color_hex(0xffffff));
        lv_style_set_pad_all(&style_cell, 6);
        lv_style_set_radius(&style_cell, 4);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 8, 0);

    /* Main grid container */
    lv_obj_t * container = lv_obj_create(screen);
    static const int32_t container_style_grid_column_dsc_array_0[] = {54, 54, 54, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(container, container_style_grid_column_dsc_array_0, 0);
    static const int32_t container_style_grid_row_dsc_array_1[] = {42, 42, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(container, container_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_layout(container, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(container, lv_pct(100), 120);
    lv_obj_t * label_1 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_1, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_1, 0, 0);
    lv_obj_set_style_bg_color(label_1, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_1, "A");
    lv_obj_add_style(label_1, &style_cell, 0);

    lv_obj_t * label_2 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_2, 1, 0);
    lv_obj_set_style_grid_cell_row_pos(label_2, 0, 0);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0x2ecc71), 0);
    lv_label_set_text(label_2, "B");
    lv_obj_add_style(label_2, &style_cell, 0);

    lv_obj_t * label_3 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_3, 2, 0);
    lv_obj_set_style_grid_cell_row_pos(label_3, 0, 0);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0xe67e22), 0);
    lv_label_set_text(label_3, "C");
    lv_obj_add_style(label_3, &style_cell, 0);

    lv_obj_t * label_4 = lv_label_create(container);
    lv_obj_set_style_grid_cell_column_pos(label_4, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_4, 1, 0);
    lv_obj_set_style_bg_color(label_4, lv_color_hex(0x8e44ad), 0);
    lv_label_set_text(label_4, "D");
    lv_obj_add_style(label_4, &style_cell, 0);

    lv_obj_t * label_5 = lv_label_create(container);
    lv_obj_set_flag(label_5, LV_OBJ_FLAG_IGNORE_LAYOUT, true);
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(label_5, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(label_5, "FREE");
    lv_obj_add_style(label_5, &style_cell, 0);
}
#endif
