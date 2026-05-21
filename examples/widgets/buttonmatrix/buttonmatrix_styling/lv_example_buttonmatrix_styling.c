/**
 * @file lv_example_buttonmatrix_styling.c
 */

#include "../../../lv_examples.h"
#if LV_USE_BUTTONMATRIX && LV_BUILD_EXAMPLES

/**
 * @title Button matrix styling
 * @brief Style the MAIN container, the ITEMS, and the checked-state items separately.
 *
 * Button matrix has two parts: MAIN (the container) and ITEMS (each virtual button).
 * The named styles target ITEMS in the default state and in the `checked` state via
 * `selector="items|checked"`, so the highlighted day picks up an accent fill. The
 * container gets a flat, padded look via local `style_*` props on the matrix tag.
 */
void lv_example_buttonmatrix_styling(void)
{
    static lv_style_t style_bm_item;
    static lv_style_t style_bm_item_checked;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_bm_item);
        lv_style_set_bg_opa(&style_bm_item, (255 * 100 / 100));
        lv_style_set_bg_color(&style_bm_item, lv_color_hex(0xf3f4f6));
        lv_style_set_radius(&style_bm_item, 8);
        lv_style_set_border_color(&style_bm_item, lv_color_hex(0xd1d5db));
        lv_style_set_border_width(&style_bm_item, 1);
        lv_style_set_text_color(&style_bm_item, lv_color_hex(0x111827));

        lv_style_init(&style_bm_item_checked);
        lv_style_set_bg_color(&style_bm_item_checked, lv_color_hex(0x6366f1));
        lv_style_set_text_color(&style_bm_item_checked, lv_color_hex(0xffffff));
        lv_style_set_outline_color(&style_bm_item_checked, lv_color_hex(0x6366f1));
        lv_style_set_outline_width(&style_bm_item_checked, 3);
        lv_style_set_outline_pad(&style_bm_item_checked, 2);
        lv_style_set_outline_opa(&style_bm_item_checked, 80);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Tap a day to switch the checked tile and see the items|checked style apply. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Button matrix: styling parts and states");

    /* Weekday matrix with one-checked semantics + styled items */
    lv_obj_t * lv_buttonmatrix_0 = lv_buttonmatrix_create(screen);
    lv_obj_set_size(lv_buttonmatrix_0, lv_pct(90), 60);
    static const char * lv_buttonmatrix_0_map_0[] = {"M", "T", "W", "T", "F", "S", "S", NULL};
    lv_buttonmatrix_set_map(lv_buttonmatrix_0, lv_buttonmatrix_0_map_0);
    lv_buttonmatrix_set_one_checked(lv_buttonmatrix_0, true);
    static const lv_buttonmatrix_ctrl_t lv_buttonmatrix_0_ctrl_map_1[] = {LV_BUTTONMATRIX_CTRL_CHECKABLE, LV_BUTTONMATRIX_CTRL_CHECKABLE, (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED), LV_BUTTONMATRIX_CTRL_CHECKABLE, LV_BUTTONMATRIX_CTRL_CHECKABLE, LV_BUTTONMATRIX_CTRL_CHECKABLE, LV_BUTTONMATRIX_CTRL_CHECKABLE};
    lv_buttonmatrix_set_ctrl_map(lv_buttonmatrix_0, lv_buttonmatrix_0_ctrl_map_1);
    lv_obj_set_style_bg_opa(lv_buttonmatrix_0, (255 * 100 / 100), 0);
    lv_obj_set_style_bg_color(lv_buttonmatrix_0, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_color(lv_buttonmatrix_0, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_border_width(lv_buttonmatrix_0, 1, 0);
    lv_obj_set_style_radius(lv_buttonmatrix_0, 10, 0);
    lv_obj_set_style_pad_all(lv_buttonmatrix_0, 6, 0);
    lv_obj_set_style_pad_row(lv_buttonmatrix_0, 6, 0);
    lv_obj_set_style_pad_column(lv_buttonmatrix_0, 6, 0);
    lv_obj_add_style(lv_buttonmatrix_0, &style_bm_item, LV_PART_ITEMS);
    lv_obj_add_style(lv_buttonmatrix_0, &style_bm_item_checked, LV_PART_ITEMS | LV_STATE_CHECKED);
}
#endif
