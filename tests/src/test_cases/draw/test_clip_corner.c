#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

static lv_obj_t * create_panel(int32_t radius, bool transform)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_style_pad_all(parent, 3, 0);
    lv_obj_set_style_radius(parent, radius, 0);
    lv_obj_set_style_clip_corner(parent, true, 0);
    if(transform) lv_obj_set_style_transform_rotation(parent, 300, 0);

    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_width(label, lv_pct(200));
    lv_label_set_text(label,
                      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus dignissim quam id eros iaculis dapibus. Mauris nisl orci, vulputate sed eleifend a, consectetur et nulla.");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_bg_color(label, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_20, 0);

    lv_obj_update_layout(parent);
    lv_obj_scroll_by(parent, -15, -15, LV_ANIM_OFF);

    return parent;
}

void test_func_1(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_column(lv_screen_active(), 40, 0);

    create_panel(0, false);
    create_panel(10, false);
    create_panel(30, false);
    create_panel(100, false);

    lv_obj_t * cont = create_panel(0, true);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    create_panel(10, true);
    create_panel(30, true);
    create_panel(100, true);

    TEST_ASSERT_EQUAL_SCREENSHOT("clip_corner_1.png");

}

#endif
