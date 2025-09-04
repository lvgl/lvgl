#if LV_BUILD_TEST
#  include "../../lvgl_private.h"
#  include "../lvgl.h"

#  include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

static void simple_style(lv_obj_t * obj)
{
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(obj, 0, LV_PART_MAIN);
}

static lv_obj_t * create_row(lv_flex_align_t main_place, int n_children)
{
    lv_obj_t * row = lv_obj_create(lv_screen_active());
    lv_obj_set_size(row, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, main_place, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    simple_style(row);
    for(int i = 0; i < n_children; i++) {
        lv_obj_t * child = lv_obj_create(row);
        lv_obj_set_size(child, 40, 40);
        lv_obj_set_style_bg_color(child, lv_palette_main(LV_PALETTE_BLUE), 0);
        simple_style(child);
    }
    return row;
}

void test_align(void)
{
    create_row(LV_FLEX_ALIGN_CENTER, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_center1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_CENTER, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_center2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_EVENLY, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_evenly1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_EVENLY, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_evenly2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_AROUND, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_around1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_AROUND, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_around2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_BETWEEN, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_between1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_BETWEEN, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_between2.png");
    lv_obj_clean(active_screen);
}

/*See https://github.com/lvgl/lvgl/issues/7035*/
void test_wrap_grow_min_width(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    for(int i = 0; i < 14; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_set_style_min_width(obj, 100, 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 70 + (i % 3) * 20);

        uint8_t grow = (i % 3);
        lv_obj_set_flex_grow(obj, grow);

        if(i == 9) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "grow:%d%s", grow, i == 9 ? "\nnew track" : "");
        lv_obj_center(label);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_wrap_grow_min_width.png");
}

/**
 * When the container is set to LV_SIZE_CONTENT in flex direction and an item is set to grow,
 * the size of the grown item should be equal to the min size of the item
 */
void test_col_grow_size_content(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    lv_obj_t * header = lv_label_create(cont);
    lv_obj_set_name(header, "header");
    lv_label_set_text(header, "header");

    lv_obj_t * item = lv_obj_create(cont);
    lv_obj_set_name(item, "item");
    lv_obj_set_width(item, LV_PCT(100));
    lv_obj_set_flex_grow(item, 1);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);

    lv_obj_t * footer = lv_label_create(cont);
    lv_obj_set_name(footer, "footer");
    lv_label_set_text(footer, "footer");

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content.png");

    lv_obj_set_style_min_height(item, 200, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content_min_size.png");

    /* The min size of the cont should "override" the `LV_SIZE_CONTENT` height so item should be visible and grow to
     * fill space */
    lv_obj_set_style_min_height(item, 0, LV_PART_MAIN);
    lv_obj_set_style_min_height(cont, 500, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content_min_size_cont.png");

    /* The min size of the cont should "override" the `LV_SIZE_CONTENT` height so item should be visible and grow to
     * fill space up to the max size */
    lv_obj_set_style_max_height(item, 200, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content_max_size.png");

    /* item height should be 0 since min size is not set for item or cont, cont should not be max size since max size is
     * larger than content */
    lv_obj_set_style_min_height(cont, 0, LV_PART_MAIN);
    lv_obj_set_style_max_height(item, 0, LV_PART_MAIN);
    lv_obj_set_style_max_height(cont, 500, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content_max_size_cont.png");

    /* item height should be 0 since min size is not set for item or cont, cont should be max size since max size is
     * smaller than content */
    lv_obj_set_style_max_height(item, 0, LV_PART_MAIN);
    lv_obj_set_style_max_height(cont, 70, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content_max_size_cont2.png");

    /* item should be min height (50) and cont will be max height (40) so cont should be scrollable */
    lv_obj_set_style_min_height(item, 50, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content_max_size_cont3.png");

    /* max cont height should be ignored here */
    lv_obj_set_style_max_height(cont, LV_PCT(70), LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content_max_size_cont4.png");
}

/**
 * When the container is set to LV_SIZE_CONTENT in flex direction and an item is set to grow,
 * the size of the grown item should be equal to the min size of the item
 */
void test_row_grow_size_content(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    lv_obj_t * left = lv_label_create(cont);
    lv_obj_set_name(left, "left");
    lv_label_set_text(left, "left");

    lv_obj_t * item = lv_obj_create(cont);
    lv_obj_set_name(item, "item");
    lv_obj_set_height(item, LV_PCT(100));
    lv_obj_set_flex_grow(item, 1);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);

    lv_obj_t * right = lv_label_create(cont);
    lv_obj_set_name(right, "right");
    lv_label_set_text(right, "right");

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_grow_size_content.png");

    lv_obj_set_style_min_width(item, 200, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_grow_size_content_min_size.png");

    /* The min size of the cont should "override" the `LV_SIZE_CONTENT` width so item should be visible and grow to
     * fill space */
    lv_obj_set_style_min_width(item, 0, LV_PART_MAIN);
    lv_obj_set_style_min_width(cont, 500, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_grow_size_content_min_size_cont.png");

    /* The min size of the cont should "override" the `LV_SIZE_CONTENT` width so item should be visible and grow to
     * fill space up to the max size */
    lv_obj_set_style_max_width(item, 200, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_grow_size_content_max_size.png");

    /* item width should be 0 since min size is not set for item or cont, cont should not be max size since max size is
     * larger than content */
    lv_obj_set_style_min_width(cont, 0, LV_PART_MAIN);
    lv_obj_set_style_max_width(item, 0, LV_PART_MAIN);
    lv_obj_set_style_max_width(cont, 500, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_grow_size_content_max_size_cont.png");

    /* item width should be 0 since min size is not set for item or cont, cont should be max size since max size is
     * smaller than content */
    lv_obj_set_style_max_width(item, 0, LV_PART_MAIN);
    lv_obj_set_style_max_width(cont, 100, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_grow_size_content_max_size_cont2.png");

    /* item should be min width (90) and cont will be max width (70) so cont should be scrollable */
    lv_obj_set_style_min_width(item, 90, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_grow_size_content_max_size_cont3.png");

    /* max cont width should be ignored here */
    lv_obj_set_style_max_width(cont, LV_PCT(70), LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_grow_size_content_max_size_cont4.png");
}

/* Test that flex grow still works with min size = LV_SIZE_CONTENT */
void test_flex_grow_size_content_2(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_style_min_width(cont, LV_SIZE_CONTENT, LV_PART_MAIN);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    lv_obj_t * label = lv_label_create(cont);
    lv_obj_set_name(label, "label");
    lv_label_set_text(label, "label");

    lv_obj_t * sub_cont = lv_obj_create(cont);
    lv_obj_set_name(sub_cont, "sub_cont");
    lv_obj_set_height(sub_cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(sub_cont, 1);
    lv_obj_set_flex_flow(sub_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_bg_color(sub_cont, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(sub_cont, LV_OPA_COVER, 0);

    for(size_t i = 0; i < 5; i++) {
        lv_obj_t * item = lv_label_create(sub_cont);
        lv_obj_set_name(item, "item_#");
        lv_label_set_text_fmt(item, "item_%d", (int)i);
        lv_obj_set_style_text_color(item, lv_color_black(), 0);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_grow_size_content2.png");

    lv_obj_set_style_max_width(sub_cont, LV_SIZE_CONTENT, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_grow_size_content_max_size_content.png");

    for(size_t i = 5; i < 20; i++) {
        lv_obj_t * item = lv_label_create(sub_cont);
        lv_obj_set_name(item, "item_#");
        lv_label_set_text_fmt(item, "item_%d", (int)i);
        lv_obj_set_style_text_color(item, lv_color_black(), 0);
    }

    /** This interaction feels like UB but I don't think it was introduced by this PR.
     * Testing to see if behaviour changes
     */
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_grow_size_content_max_size_content_wrap.png");
    lv_obj_set_style_max_width(sub_cont, LV_COORD_MAX, LV_PART_MAIN);
    lv_obj_update_layout(sub_cont);
    lv_obj_set_style_max_width(sub_cont, LV_SIZE_CONTENT, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_grow_size_content_max_size_content_wrap2.png");
}

static lv_obj_t * cont_row_5_create(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(cont);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    for(int i = 0; i < 5; i++) {
        lv_obj_t * btn = lv_button_create(cont);
        lv_obj_set_size(btn, 40, 60);

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "%d", i);
        lv_obj_center(label);
    }

    return cont;
}

static void hide(lv_obj_t * cont, uint32_t idx)
{
    lv_obj_add_flag(lv_obj_get_child(cont, idx), LV_OBJ_FLAG_HIDDEN);
}

void test_flex_hide_items(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_t * cont;
    cont = cont_row_5_create();

    cont = cont_row_5_create();
    hide(cont, 0);

    cont = cont_row_5_create();
    hide(cont, 0);
    hide(cont, 1);

    cont = cont_row_5_create();
    hide(cont, 4);

    cont = cont_row_5_create();
    hide(cont, 3);
    hide(cont, 4);

    cont = cont_row_5_create();
    hide(cont, 0);
    hide(cont, 1);
    hide(cont, 3);
    hide(cont, 4);

    cont = cont_row_5_create();
    hide(cont, 1);
    hide(cont, 3);

    cont = cont_row_5_create();
    hide(cont, 0);
    hide(cont, 4);

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_hide_items.png");
}

#endif
