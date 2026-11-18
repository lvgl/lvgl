#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

/**
 * See https://github.com/lvgl/lvgl/issues/6837
 */
void test_content_parent_pct_child_pos_1(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(parent, 20, 20);
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(parent, 10, 0);

    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_set_pos(child1, 100, 50);
    lv_obj_set_size(child1, 100, 100);

    lv_obj_t * child2 = lv_obj_create(parent);

    /*Simple case*/
    lv_obj_set_size(child2, 50, 50);
    lv_obj_set_pos(child2, 0, 0);
    lv_obj_update_layout(child2);
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_x(child2));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_y(child2));

    /*Simple case*/
    lv_obj_set_pos(child2, 30, 200);
    lv_obj_update_layout(child2);
    TEST_ASSERT_EQUAL_INT32(30, lv_obj_get_x(child2));
    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_y(child2));

    /*x and y should be 0 to avoid circural dependency*/
    lv_obj_set_pos(child2, LV_PCT(10), LV_PCT(50));
    lv_obj_update_layout(child2);
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_x(child2));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_y(child2));
}

void test_style_min_size(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_style_pad_all(parent, 10, 0);

    lv_obj_t * child = lv_button_create(parent);
    lv_obj_t * label = lv_label_create(child);
    lv_label_set_text(label, "Button");
    lv_obj_set_size(child, LV_PCT(100), LV_PCT(100)); // will evaluate to 0
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_no_min_size.png");
    TEST_ASSERT_TRUE(lv_obj_is_width_min(child));
    TEST_ASSERT_TRUE(lv_obj_is_height_min(child));
    TEST_ASSERT_EQUAL(0, lv_obj_get_style_clamped_width(child));
    TEST_ASSERT_EQUAL(0, lv_obj_get_style_clamped_height(child));

    lv_obj_set_style_min_width(child, 200, 0);
    lv_obj_set_style_min_height(child, 300, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_fixed_min_size.png");
    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_width(child));
    TEST_ASSERT_EQUAL_INT32(300, lv_obj_get_height(child));
    TEST_ASSERT_TRUE(lv_obj_is_width_min(child));
    TEST_ASSERT_TRUE(lv_obj_is_height_min(child));
    TEST_ASSERT_EQUAL(200, lv_obj_get_style_clamped_width(child));
    TEST_ASSERT_EQUAL(300, lv_obj_get_style_clamped_height(child));

    lv_obj_set_style_min_width(child, LV_SIZE_CONTENT, 0);
    lv_obj_set_style_min_height(child, LV_SIZE_CONTENT, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_content_min_size.png");
}

void test_chaining_invalidation_layout(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, 500, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_t * label = lv_label_create(cont);
    lv_obj_set_name(label, "label");
    lv_label_set_text(label, "Dropdown with size content:");

    lv_obj_t * sub_cont = lv_obj_create(cont);
    lv_obj_set_name(sub_cont, "sub_cont");
    lv_obj_set_style_bg_color(sub_cont, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(sub_cont, LV_OPA_COVER, 0);
    lv_obj_set_height(sub_cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(sub_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_grow(sub_cont, 1);
    lv_obj_set_style_min_width(sub_cont, LV_SIZE_CONTENT, LV_PART_MAIN);

    lv_obj_t * dd = lv_dropdown_create(sub_cont);
    lv_obj_set_name(dd, "dropdown");
    lv_dropdown_set_options(dd, "Short\nA bit longer option\nThe longest option in the list");
    lv_obj_set_width(dd, 0);
    lv_obj_set_style_min_width(dd, LV_SIZE_CONTENT, 0);
    lv_obj_set_flex_grow(dd, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_chained_layout_invalidation_pre.png");
    lv_dropdown_set_selected(dd, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_pos_chained_layout_invalidation_post.png");
}

void test_circular_height_dependency(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    lv_obj_t * item1 = lv_obj_create(cont);
    lv_obj_set_name(item1, "item1");
    lv_obj_set_style_bg_color(item1, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(item1, LV_OPA_COVER, 0);
    /**
     * Because parent is size content this will evaluate after all the fixed/clamped children are sized
     * This means item1 should size to 100 because of item2
     */
    lv_obj_set_height(item1, LV_PCT(100));
    lv_obj_set_flex_grow(item1, 1);

    lv_obj_t * item2 = lv_obj_create(cont);
    lv_obj_set_name(item2, "item2");
    lv_obj_set_style_bg_color(item2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(item2, LV_OPA_COVER, 0);
    lv_obj_set_height(item2, 100); // fixed size
    lv_obj_set_flex_grow(item2, 1);

    lv_obj_t * item3 = lv_obj_create(cont);
    lv_obj_set_name(item3, "item3");
    lv_obj_set_style_bg_color(item3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(item3, LV_OPA_COVER, 0);
    lv_obj_set_height(item3, LV_PCT(100)); // same as item1 but checking if child order matters
    lv_obj_set_flex_grow(item3, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_circular_height.png");
    TEST_ASSERT_EQUAL_INT32(100, lv_obj_get_height(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item3));

    /**
     * Decreasing item2 height should also decrease item1 and item3 height
     */
    lv_obj_set_height(item2, 40);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(40, lv_obj_get_height(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item3));

    lv_obj_set_height(item2, LV_PCT(100));
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_height(item1));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_height(item2));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_height(item3));

    lv_obj_set_style_min_height(item2, 50, 0);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(50, lv_obj_get_height(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_height(item2), lv_obj_get_height(item3));
}

void test_circular_width_dependency(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * item1 = lv_obj_create(cont);
    lv_obj_set_name(item1, "item1");
    lv_obj_set_style_bg_color(item1, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(item1, LV_OPA_COVER, 0);
    /**
     * Because parent is size content this will evaluate after all the fixed/clamped children are sized
     * This means item1 should size to 100 because of item2
     */
    lv_obj_set_width(item1, LV_PCT(100));
    lv_obj_set_flex_grow(item1, 1);

    lv_obj_t * item2 = lv_obj_create(cont);
    lv_obj_set_name(item2, "item2");
    lv_obj_set_style_bg_color(item2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(item2, LV_OPA_COVER, 0);
    lv_obj_set_width(item2, 100); // fixed size
    lv_obj_set_flex_grow(item2, 1);

    lv_obj_t * item3 = lv_obj_create(cont);
    lv_obj_set_name(item3, "item3");
    lv_obj_set_style_bg_color(item3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(item3, LV_OPA_COVER, 0);
    lv_obj_set_width(item3, LV_PCT(100)); // same as item1 but checking if child order matters
    lv_obj_set_flex_grow(item3, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_circular_width.png");
    TEST_ASSERT_EQUAL_INT32(100, lv_obj_get_width(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item3));

    /**
     * Decreasing item2 width should also decrease item1 and item3 width
     */
    lv_obj_set_width(item2, 40);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(40, lv_obj_get_width(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item3));

    lv_obj_set_width(item2, LV_PCT(100));
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_width(item1));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_width(item2));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_width(item3));

    lv_obj_set_style_min_width(item2, 50, 0);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT32(50, lv_obj_get_width(item2));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item1));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(item2), lv_obj_get_width(item3));
}

static lv_obj_t * cont_create(lv_obj_t * parent, const char * text)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style(cont, NULL, LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 1, LV_PART_MAIN);
    lv_obj_set_size(cont, 150, 30);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, text);

    lv_obj_t * rect_top_right = lv_obj_create(cont);
    lv_obj_remove_style(rect_top_right, NULL, LV_PART_MAIN);
    lv_obj_set_size(rect_top_right, 10, 10);
    lv_obj_set_align(rect_top_right, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_bg_color(rect_top_right, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rect_top_right, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t * rect_bottom_left = lv_obj_create(cont);
    lv_obj_remove_style(rect_bottom_left, NULL, LV_PART_MAIN);
    lv_obj_set_size(rect_bottom_left, 10, 10);
    lv_obj_set_align(rect_bottom_left, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_bg_color(rect_bottom_left, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rect_bottom_left, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t * rect_bottom_right = lv_obj_create(cont);
    lv_obj_remove_style(rect_bottom_right, NULL, LV_PART_MAIN);
    lv_obj_set_size(rect_bottom_right, 10, 10);
    lv_obj_set_align(rect_bottom_right, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_style_bg_color(rect_bottom_right, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rect_bottom_right, LV_OPA_COVER, LV_PART_MAIN);

    return cont;
}

static void cont_create_x_y(lv_obj_t * parent, const char * text, int32_t x, int32_t y)
{
    lv_obj_t * cont = cont_create(parent, text);
    lv_obj_set_pos(cont, x, y);
}

void test_rtl_pos_x_y(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());

    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, LV_PART_MAIN);

    cont_create_x_y(cont, "(0,0)", 0, 0);
    cont_create_x_y(cont, "(50,50)", 50, 50);
    cont_create_x_y(cont, "(100,100)", 100, 100);
    cont_create_x_y(cont, "(150,150)", 150, 150);
    cont_create_x_y(cont, "(100,200)", 100, 200);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/rtl_obj_pos_x_y.png");
}

static void cont_create_align(lv_obj_t * parent, const char * text, lv_align_t align)
{
    lv_obj_t * cont = cont_create(parent, text);
    lv_obj_set_align(cont, align);
}

static void cont_create_align_offset(lv_obj_t * parent, const char * text, lv_align_t align, int32_t x_ofs,
                                     int32_t y_ofs)
{
    lv_obj_t * cont = cont_create(parent, text);
    lv_obj_align(cont, align, x_ofs, y_ofs);
}

void test_align_left(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_LTR, LV_PART_MAIN);

    cont_create_align(cont, "TOP_LEFT", LV_ALIGN_TOP_LEFT);
    cont_create_align(cont, "LEFT_MID", LV_ALIGN_LEFT_MID);
    cont_create_align(cont, "BOTTOM_LEFT", LV_ALIGN_BOTTOM_LEFT);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_align_left.png");
}

void test_align_right(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_LTR, LV_PART_MAIN);

    cont_create_align(cont, "TOP_RIGHT", LV_ALIGN_TOP_RIGHT);
    cont_create_align(cont, "RIGHT_MID", LV_ALIGN_RIGHT_MID);
    cont_create_align(cont, "BOTTOM_RIGHT", LV_ALIGN_BOTTOM_RIGHT);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_align_right.png");
}

void test_align_center(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_LTR, LV_PART_MAIN);

    cont_create_align_offset(cont, "TOP_MID (150,0)", LV_ALIGN_TOP_MID, 150, 0);
    cont_create_align_offset(cont, "CENTER (150,0)", LV_ALIGN_CENTER, 150, 0);
    cont_create_align_offset(cont, "BOTTOM_MID (150,0)", LV_ALIGN_BOTTOM_MID, 150, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_align_center.png");
}

void test_rtl_align_left(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, LV_PART_MAIN);

    cont_create_align(cont, "TOP_LEFT", LV_ALIGN_TOP_LEFT);
    cont_create_align(cont, "LEFT_MID", LV_ALIGN_LEFT_MID);
    cont_create_align(cont, "BOTTOM_LEFT", LV_ALIGN_BOTTOM_LEFT);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/rtl_obj_align_left.png");
}

void test_rtl_align_right(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, LV_PART_MAIN);

    cont_create_align(cont, "TOP_RIGHT", LV_ALIGN_TOP_RIGHT);
    cont_create_align(cont, "RIGHT_MID", LV_ALIGN_RIGHT_MID);
    cont_create_align(cont, "BOTTOM_RIGHT", LV_ALIGN_BOTTOM_RIGHT);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/rtl_obj_align_right.png");
}

void test_rtl_align_center(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, LV_PART_MAIN);

    cont_create_align_offset(cont, "TOP_MID (150,0)", LV_ALIGN_TOP_MID, 150, 0);
    cont_create_align_offset(cont, "CENTER (150,0)", LV_ALIGN_CENTER, 150, 0);
    cont_create_align_offset(cont, "BOTTOM_MID (150,0)", LV_ALIGN_BOTTOM_MID, 150, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/rtl_obj_align_center.png");
}

#define REORDER_MAX_INV_AREAS 64
static lv_area_t reorder_inv_areas[REORDER_MAX_INV_AREAS];
static uint32_t reorder_inv_cnt;

static void reorder_require_direct_mode(void)
{
    if(lv_display_get_render_mode(lv_display_get_default()) != LV_DISPLAY_RENDER_MODE_DIRECT) {
        TEST_IGNORE_MESSAGE("Needs LV_DISPLAY_RENDER_MODE_DIRECT to keep the previous frame");
    }
}

static lv_obj_t * reorder_box(lv_obj_t * parent, int32_t x, int32_t y, int32_t w, int32_t h, lv_palette_t color)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_palette_main(color), 0);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    return obj;
}

static lv_obj_t * reorder_flex_row(void)
{
    lv_obj_t * cont = reorder_box(lv_screen_active(), 20, 20, 400, 100, LV_PALETTE_GREY);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    return cont;
}

/*Two grid items spanning overlapping cells: a reorder changes only the stacking*/
static void reorder_grid_overlapping(lv_obj_t ** a, lv_obj_t ** b)
{
    static const int32_t cols[] = {60, 100, 140, LV_GRID_TEMPLATE_LAST};
    static const int32_t rows[] = {80, LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = reorder_box(lv_screen_active(), 20, 20, 400, 100, LV_PALETTE_GREY);
    lv_obj_set_grid_dsc_array(cont, cols, rows);

    *a = reorder_box(cont, 0, 0, 50, 50, LV_PALETTE_RED);
    *b = reorder_box(cont, 0, 0, 50, 50, LV_PALETTE_BLUE);
    lv_obj_set_grid_cell(*a, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_grid_cell(*b, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
}

/*Redraw what the reorder invalidated, then compare it with a full redraw*/
static void reorder_assert_no_stale_pixels(void)
{
    lv_display_t * disp = lv_display_get_default();
    lv_refr_now(disp);  /*Partial redraw; the layout runs here too*/

    lv_draw_buf_t * buf = lv_display_get_buf_active(disp);
    uint32_t size = buf->header.stride * buf->header.h;
    uint8_t * partial = lv_malloc(size);
    TEST_ASSERT_NOT_NULL(partial);
    lv_memcpy(partial, buf->data, size);

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(disp);  /*Reference: everything redrawn*/

    uint32_t px_size = lv_color_format_get_size(buf->header.cf);
    uint32_t stale = 0;
    lv_area_t bbox = {INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN};
    for(int32_t y = 0; y < (int32_t)buf->header.h; y++) {
        for(int32_t x = 0; x < (int32_t)buf->header.w; x++) {
            uint32_t ofs = y * buf->header.stride + x * px_size;
            if(lv_memcmp(&partial[ofs], &buf->data[ofs], px_size) != 0) {
                stale++;
                bbox.x1 = LV_MIN(bbox.x1, x);
                bbox.y1 = LV_MIN(bbox.y1, y);
                bbox.x2 = LV_MAX(bbox.x2, x);
                bbox.y2 = LV_MAX(bbox.y2, y);
            }
        }
    }
    lv_free(partial);

    char msg[128];
    lv_snprintf(msg, sizeof(msg), "%" LV_PRIu32 " stale pixels in (%" LV_PRId32 ",%" LV_PRId32 ")-(%" LV_PRId32 ",%"
                LV_PRId32 ")", stale, bbox.x1, bbox.y1, bbox.x2, bbox.y2);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, stale, msg);
}

static void reorder_inv_area_cb(lv_event_t * e)
{
    TEST_ASSERT_LESS_THAN_UINT32(REORDER_MAX_INV_AREAS, reorder_inv_cnt);
    reorder_inv_areas[reorder_inv_cnt++] = *(lv_area_t *)lv_event_get_param(e);
}

static void reorder_capture_start(void)
{
    reorder_inv_cnt = 0;
    lv_display_add_event_cb(lv_display_get_default(), reorder_inv_area_cb, LV_EVENT_INVALIDATE_AREA, NULL);
}

static void reorder_capture_stop(void)
{
    lv_display_remove_event_cb_with_user_data(lv_display_get_default(), reorder_inv_area_cb, NULL);
}

static bool reorder_was_invalidated(const lv_obj_t * obj)
{
    lv_area_t common;
    for(uint32_t i = 0; i < reorder_inv_cnt; i++) {
        if(lv_area_intersect(&common, &obj->coords, &reorder_inv_areas[i])) return true;
    }
    return false;
}

/*lv_obj_move_to_index()*/

void test_move_to_index_redraw_overlapping(void)
{
    reorder_require_direct_mode();

    lv_obj_t * a = reorder_box(lv_screen_active(), 50, 50, 100, 100, LV_PALETTE_RED);
    reorder_box(lv_screen_active(), 100, 100, 100, 100, LV_PALETTE_BLUE);
    lv_refr_now(NULL);

    lv_obj_move_to_index(a, -1);
    reorder_assert_no_stale_pixels();
}

/*Moving the first item to the end shifts every sibling*/
void test_move_to_index_redraw_flex(void)
{
    reorder_require_direct_mode();

    lv_obj_t * cont = reorder_flex_row();
    lv_obj_t * a = reorder_box(cont, 0, 0, 40, 60, LV_PALETTE_RED);
    lv_obj_t * b = reorder_box(cont, 0, 0, 80, 80, LV_PALETTE_GREEN);
    reorder_box(cont, 0, 0, 120, 40, LV_PALETTE_BLUE);
    lv_refr_now(NULL);

    lv_obj_move_to_index(a, -1);
    reorder_assert_no_stale_pixels();

    TEST_ASSERT_EQUAL_INT32(cont->coords.x1, b->coords.x1);
    TEST_ASSERT_EQUAL_INT32(cont->coords.x1 + 80 + 120, a->coords.x1);
}

/*The grow item ends up in a different slot*/
void test_move_to_index_redraw_flex_grow(void)
{
    reorder_require_direct_mode();

    lv_obj_t * cont = reorder_flex_row();
    lv_obj_t * a = reorder_box(cont, 0, 0, 60, 60, LV_PALETTE_RED);
    lv_obj_t * grow = reorder_box(cont, 0, 0, 10, 30, LV_PALETTE_GREEN);
    lv_obj_set_flex_grow(grow, 1);
    reorder_box(cont, 0, 0, 150, 80, LV_PALETTE_BLUE);
    lv_refr_now(NULL);

    lv_obj_move_to_index(a, -1);
    reorder_assert_no_stale_pixels();

    TEST_ASSERT_EQUAL_INT32(cont->coords.x1, grow->coords.x1);
    TEST_ASSERT_EQUAL_INT32(cont->coords.x2 - 60 + 1, a->coords.x1);
}

/*Nothing moves, only the stacking changes*/
void test_move_to_index_redraw_grid(void)
{
    reorder_require_direct_mode();

    lv_obj_t * a;
    lv_obj_t * b;
    reorder_grid_overlapping(&a, &b);
    lv_refr_now(NULL);

    lv_obj_move_to_index(a, -1);
    reorder_assert_no_stale_pixels();
}

/*Only the moved object can look different, not the whole parent*/
void test_move_to_index_invalidates_only_the_moved_object(void)
{
    reorder_require_direct_mode();

    lv_obj_t * a = reorder_box(lv_screen_active(), 10, 10, 50, 50, LV_PALETTE_RED);
    reorder_box(lv_screen_active(), 30, 30, 50, 50, LV_PALETTE_BLUE);
    lv_obj_t * far = reorder_box(lv_screen_active(), 300, 300, 50, 50, LV_PALETTE_GREEN);
    lv_refr_now(NULL);

    reorder_capture_start();
    lv_obj_move_to_index(a, -1);
    reorder_capture_stop();

    TEST_ASSERT_TRUE(reorder_was_invalidated(a));
    TEST_ASSERT_FALSE(reorder_was_invalidated(far));
}

/*lv_obj_swap()*/

void test_swap_redraw_overlapping(void)
{
    reorder_require_direct_mode();

    lv_obj_t * a = reorder_box(lv_screen_active(), 50, 50, 100, 100, LV_PALETTE_RED);
    lv_obj_t * b = reorder_box(lv_screen_active(), 100, 100, 100, 100, LV_PALETTE_BLUE);
    lv_refr_now(NULL);

    lv_obj_swap(a, b);
    reorder_assert_no_stale_pixels();
}

/*Swapping the first and last items moves all three*/
void test_swap_redraw_flex(void)
{
    reorder_require_direct_mode();

    lv_obj_t * cont = reorder_flex_row();
    lv_obj_t * a = reorder_box(cont, 0, 0, 40, 60, LV_PALETTE_RED);
    lv_obj_t * b = reorder_box(cont, 0, 0, 80, 80, LV_PALETTE_GREEN);
    lv_obj_t * c = reorder_box(cont, 0, 0, 120, 40, LV_PALETTE_BLUE);
    lv_refr_now(NULL);

    lv_obj_swap(a, c);
    reorder_assert_no_stale_pixels();

    TEST_ASSERT_EQUAL_INT32(cont->coords.x1, c->coords.x1);
    TEST_ASSERT_EQUAL_INT32(cont->coords.x1 + 120, b->coords.x1);
    TEST_ASSERT_EQUAL_INT32(cont->coords.x1 + 120 + 80, a->coords.x1);
}

/*Nothing moves, only the stacking changes*/
void test_swap_redraw_grid(void)
{
    reorder_require_direct_mode();

    lv_obj_t * a;
    lv_obj_t * b;
    reorder_grid_overlapping(&a, &b);
    lv_refr_now(NULL);

    lv_obj_swap(a, b);
    reorder_assert_no_stale_pixels();
}

/*Children of different parents: each one moves into the other parent*/
void test_swap_redraw_different_parents(void)
{
    reorder_require_direct_mode();

    lv_obj_t * p1 = reorder_box(lv_screen_active(), 20, 20, 200, 200, LV_PALETTE_GREY);
    lv_obj_t * p2 = reorder_box(lv_screen_active(), 300, 20, 200, 200, LV_PALETTE_BROWN);
    lv_obj_t * a = reorder_box(p1, 10, 10, 50, 50, LV_PALETTE_RED);
    lv_obj_t * b = reorder_box(p2, 100, 100, 80, 80, LV_PALETTE_BLUE);
    lv_refr_now(NULL);

    lv_obj_swap(a, b);
    reorder_assert_no_stale_pixels();

    TEST_ASSERT_EQUAL_INT32(p2->coords.x1 + 10, a->coords.x1);
    TEST_ASSERT_EQUAL_INT32(p1->coords.x1 + 100, b->coords.x1);
}

/*Items of two flex containers trade places*/
void test_swap_redraw_different_flex_parents(void)
{
    reorder_require_direct_mode();

    lv_obj_t * p1 = reorder_flex_row();
    lv_obj_t * p2 = reorder_flex_row();
    lv_obj_set_y(p2, 200);
    lv_obj_t * a = reorder_box(p1, 0, 0, 40, 60, LV_PALETTE_RED);
    lv_obj_t * a2 = reorder_box(p1, 0, 0, 80, 80, LV_PALETTE_GREEN);
    lv_obj_t * b = reorder_box(p2, 0, 0, 120, 40, LV_PALETTE_BLUE);
    reorder_box(p2, 0, 0, 60, 60, LV_PALETTE_ORANGE);
    lv_refr_now(NULL);

    lv_obj_swap(a, b);
    reorder_assert_no_stale_pixels();

    TEST_ASSERT_EQUAL_INT32(p1->coords.x1, b->coords.x1);
    TEST_ASSERT_EQUAL_INT32(p1->coords.y1, b->coords.y1);
    TEST_ASSERT_EQUAL_INT32(p1->coords.x1 + 120, a2->coords.x1);
    TEST_ASSERT_EQUAL_INT32(p2->coords.x1, a->coords.x1);
    TEST_ASSERT_EQUAL_INT32(p2->coords.y1, a->coords.y1);
}

/*Only the swapped objects can look different, not the whole parent*/
void test_swap_invalidates_only_the_swapped_objects(void)
{
    reorder_require_direct_mode();

    lv_obj_t * a = reorder_box(lv_screen_active(), 10, 10, 50, 50, LV_PALETTE_RED);
    lv_obj_t * b = reorder_box(lv_screen_active(), 30, 30, 50, 50, LV_PALETTE_BLUE);
    lv_obj_t * far = reorder_box(lv_screen_active(), 300, 300, 50, 50, LV_PALETTE_GREEN);
    lv_refr_now(NULL);

    reorder_capture_start();
    lv_obj_swap(a, b);
    reorder_capture_stop();

    TEST_ASSERT_TRUE(reorder_was_invalidated(a));
    TEST_ASSERT_TRUE(reorder_was_invalidated(b));
    TEST_ASSERT_FALSE(reorder_was_invalidated(far));
}

static void reorder_ext_draw_40_cb(lv_event_t * e)
{
    lv_event_set_ext_draw_size(e, 40);
}

/*Overflow-visible children are clipped to the parent's coords + ext draw size, the area lv_obj_invalidate() covers*/
static void reorder_overflow_visible_scene(lv_obj_t ** a, lv_obj_t ** b)
{
    *a = reorder_box(lv_screen_active(), 50, 50, 50, 50, LV_PALETTE_RED);
    lv_obj_set_overflow_visible(*a, true);
    lv_obj_add_event_cb(*a, reorder_ext_draw_40_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);
    lv_obj_refresh_ext_draw_size(*a);
    reorder_box(*a, 20, 20, 100, 100, LV_PALETTE_ORANGE);    /*Up to (169,169), clipped at (139,139)*/
    *b = reorder_box(lv_screen_active(), 110, 110, 100, 100, LV_PALETTE_BLUE);
    lv_refr_now(NULL);
}

/*The child of `a` overlaps `b` outside `a`*/
void test_move_to_index_redraw_overflow_visible(void)
{
    reorder_require_direct_mode();

    lv_obj_t * a;
    lv_obj_t * b;
    reorder_overflow_visible_scene(&a, &b);

    lv_obj_move_to_index(a, -1);
    reorder_assert_no_stale_pixels();
}

/*The child of `a` overlaps `b` outside `a`*/
void test_swap_redraw_overflow_visible(void)
{
    reorder_require_direct_mode();

    lv_obj_t * a;
    lv_obj_t * b;
    reorder_overflow_visible_scene(&a, &b);

    lv_obj_swap(a, b);
    reorder_assert_no_stale_pixels();
}

void test_obj_is_rendered_after_the_first_refresh(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    TEST_ASSERT_FALSE(lv_obj_is_rendered(obj));

    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_obj_is_rendered(obj));

    /*It stays rendered, also when it is hidden later*/
    lv_obj_set_hidden(obj, true);
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_obj_is_rendered(obj));
}

void test_obj_is_not_rendered_while_hidden(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_hidden(obj, true);

    lv_refr_now(NULL);
    TEST_ASSERT_FALSE(lv_obj_is_rendered(obj));

    lv_obj_set_hidden(obj, false);
    lv_refr_now(NULL);
    TEST_ASSERT_TRUE(lv_obj_is_rendered(obj));
}

#endif
