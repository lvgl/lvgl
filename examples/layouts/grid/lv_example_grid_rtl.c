/**
 * @file lv_example_grid_rtl.c
 */

#include "../../lv_examples.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * @title Grid RTL
 * @brief Reverse column order with style_base_dir.
 *
 * Both grids declare the same three columns and the same children. Setting style_base_dir
 * to rtl on the second grid flips the column order, so cell 0 appears on the right
 * without changing how children are referenced or ordered in XML.
 */
void lv_example_grid_rtl(void)
{
    static lv_style_t style_badge;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_badge);
        lv_style_set_bg_opa(&style_badge, (255 * 100 / 100));
        lv_style_set_text_color(&style_badge, lv_color_hex(0xffffff));
        lv_style_set_pad_all(&style_badge, 6);
        lv_style_set_radius(&style_badge, 4);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 8, 0);

    /* LTR reference block */
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_set_text(label_1, "LTR (default):");

    lv_obj_t * container_1 = lv_obj_create(screen);
    static const int32_t container_1_style_grid_column_dsc_array_0[] = {40, 40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(container_1, container_1_style_grid_column_dsc_array_0, 0);
    static const int32_t container_1_style_grid_row_dsc_array_1[] = {38, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(container_1, container_1_style_grid_row_dsc_array_1, 0);
    lv_obj_set_style_layout(container_1, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(container_1, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_t * label_2 = lv_label_create(container_1);
    lv_obj_set_style_grid_cell_column_pos(label_2, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_2, 0, 0);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_2, "0");
    lv_obj_add_style(label_2, &style_badge, 0);

    lv_obj_t * label_3 = lv_label_create(container_1);
    lv_obj_set_style_grid_cell_column_pos(label_3, 1, 0);
    lv_obj_set_style_grid_cell_row_pos(label_3, 0, 0);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0x2ecc71), 0);
    lv_label_set_text(label_3, "1");
    lv_obj_add_style(label_3, &style_badge, 0);

    lv_obj_t * label_4 = lv_label_create(container_1);
    lv_obj_set_style_grid_cell_column_pos(label_4, 2, 0);
    lv_obj_set_style_grid_cell_row_pos(label_4, 0, 0);
    lv_obj_set_style_bg_color(label_4, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(label_4, "2");
    lv_obj_add_style(label_4, &style_badge, 0);

    /* RTL block with identical children */
    lv_obj_t * label_5 = lv_label_create(screen);
    lv_label_set_text(label_5, "RTL:");

    lv_obj_t * container_2 = lv_obj_create(screen);
    lv_obj_set_style_base_dir(container_2, LV_BASE_DIR_RTL, 0);
    static const int32_t container_2_style_grid_column_dsc_array_2[] = {40, 40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(container_2, container_2_style_grid_column_dsc_array_2, 0);
    static const int32_t container_2_style_grid_row_dsc_array_3[] = {38, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_row_dsc_array(container_2, container_2_style_grid_row_dsc_array_3, 0);
    lv_obj_set_style_layout(container_2, LV_LAYOUT_GRID, 0);
    lv_obj_set_size(container_2, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_t * label_6 = lv_label_create(container_2);
    lv_obj_set_style_grid_cell_column_pos(label_6, 0, 0);
    lv_obj_set_style_grid_cell_row_pos(label_6, 0, 0);
    lv_obj_set_style_bg_color(label_6, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_6, "0");
    lv_obj_add_style(label_6, &style_badge, 0);

    lv_obj_t * label_7 = lv_label_create(container_2);
    lv_obj_set_style_grid_cell_column_pos(label_7, 1, 0);
    lv_obj_set_style_grid_cell_row_pos(label_7, 0, 0);
    lv_obj_set_style_bg_color(label_7, lv_color_hex(0x2ecc71), 0);
    lv_label_set_text(label_7, "1");
    lv_obj_add_style(label_7, &style_badge, 0);

    lv_obj_t * label_8 = lv_label_create(container_2);
    lv_obj_set_style_grid_cell_column_pos(label_8, 2, 0);
    lv_obj_set_style_grid_cell_row_pos(label_8, 0, 0);
    lv_obj_set_style_bg_color(label_8, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(label_8, "2");
    lv_obj_add_style(label_8, &style_badge, 0);
}
#endif
