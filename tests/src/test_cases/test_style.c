#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include <unistd.h>

static void obj_set_height_helper(void * obj, int32_t height)
{
    lv_obj_set_height((lv_obj_t *)obj, (int32_t)height);
}

void test_gradient_vertical_misalignment(void)
{
    /* Tests gradient caching as the height of widget changes.*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x00ff00), 0);

    lv_obj_set_size(obj, 300, 100);

    lv_refr_now(NULL);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffff00), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x00ffff), 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, obj_set_height_helper);
    lv_anim_set_duration(&a, 1000);
    lv_anim_set_reverse_duration(&a, 1000);
    lv_anim_set_repeat_count(&a, 100);
    lv_anim_set_values(&a, 0, 300);
    lv_anim_start(&a);

    uint32_t i;
    for(i = 0; i < 100; i++) {
        lv_timer_handler();
        lv_tick_inc(73); /*Use a not round number to cover more anim states */
        usleep(1000);
    }
}

void test_custom_prop_ids(void)
{
    uint8_t fake_flag = 0;
    uint32_t initial_custom_props = lv_style_get_num_custom_props();
    uint32_t max_props_to_register = 64;
    for(uint32_t i = 0; i < max_props_to_register; i++) {
        lv_style_prop_t prop = lv_style_register_prop(fake_flag);
        /* Should have a higher index than the last built-in prop */
        TEST_ASSERT_GREATER_THAN(LV_STYLE_LAST_BUILT_IN_PROP, prop);
        if(i == 0) {
            /* Should be equal to the first expected index of a custom prop */
            TEST_ASSERT_EQUAL(LV_STYLE_NUM_BUILT_IN_PROPS + initial_custom_props, prop);
        }
        /*We should find our flags*/
        TEST_ASSERT_EQUAL(fake_flag, lv_style_prop_lookup_flags(prop));
        if(fake_flag == 0xff)
            fake_flag = 0;
        else
            fake_flag++;
    }
    TEST_ASSERT_EQUAL(initial_custom_props + max_props_to_register, lv_style_get_num_custom_props());
    /*
     * Check that the resizing algorithm works correctly, given that 64 props
     * were registered + whatever's built-in. A failure here may just indicate
     * that LVGL registers more built-in properties now and this needs adjustment.
     */
    TEST_ASSERT_EQUAL(LV_GLOBAL_DEFAULT()->style_custom_table_size, 64);
}

const lv_style_const_prop_t const_style_props[] = {
    LV_STYLE_CONST_WIDTH(51),
    LV_STYLE_CONST_HEIGHT(50),
    LV_STYLE_CONST_PROPS_END
};

LV_STYLE_CONST_INIT(const_style, const_style_props);

void test_const_style(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &const_style, LV_PART_MAIN);
    TEST_ASSERT_EQUAL(51, lv_obj_get_style_width(obj, LV_PART_MAIN));
    TEST_ASSERT_EQUAL(50, lv_obj_get_style_height(obj, LV_PART_MAIN));
}

void test_style_replacement(void)
{
    /*Define styles*/
    lv_style_t style_red;
    lv_style_t style_blue;

    lv_style_init(&style_red);
    lv_style_set_bg_color(&style_red, lv_color_hex(0xff0000));

    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0x0000ff));

    /*Create object with style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style_red, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex(0xff0000), lv_obj_get_style_bg_color(obj, LV_PART_MAIN));

    /*Replace style successfully*/
    bool replaced = lv_obj_replace_style(obj, &style_red, &style_blue, LV_PART_MAIN);
    TEST_ASSERT_EQUAL(true, replaced);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex(0x0000ff), lv_obj_get_style_bg_color(obj, LV_PART_MAIN));

    /*Failed replacement (already replaced)*/
    replaced = lv_obj_replace_style(obj, &style_red, &style_blue, LV_PART_MAIN);
    TEST_ASSERT_EQUAL(false, replaced);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex(0x0000ff), lv_obj_get_style_bg_color(obj, LV_PART_MAIN));

    lv_style_reset(&style_red);
    lv_style_reset(&style_blue);
}

void test_style_copy(void)
{
    lv_style_t style1;
    lv_style_t style2;
    lv_style_t copied_style;

    const lv_color_t copied_bg_color = lv_color_hex(0x0000ff);
    const lv_color_t copied_border_color = lv_color_hex(0x00ff00);
    const lv_color_t copied_outline_color = lv_color_hex(0xffff00);

    lv_style_init(&style1);
    lv_style_set_bg_color(&style1, lv_color_hex(0xff0000)); // this should get overwritten
    lv_style_set_outline_color(&style1, copied_outline_color);

    lv_style_init(&style2);
    lv_style_set_bg_color(&style2, copied_bg_color);
    lv_style_set_border_color(&style2, copied_border_color);

    lv_style_init(&copied_style);
    lv_style_copy(&copied_style, &style1);
    lv_style_copy(&copied_style, &style2); // This should reset `copied_style` then duplicate the properties of `style2`

    lv_style_value_t value;

    TEST_ASSERT_TRUE(lv_style_get_prop(&copied_style, LV_STYLE_BG_COLOR, &value) == LV_STYLE_RES_FOUND);
    TEST_ASSERT_EQUAL_COLOR(copied_bg_color, value.color);
    TEST_ASSERT_TRUE(lv_style_get_prop(&copied_style, LV_STYLE_BORDER_COLOR, &value) == LV_STYLE_RES_FOUND);
    TEST_ASSERT_EQUAL_COLOR(copied_border_color, value.color);
    TEST_ASSERT_TRUE(lv_style_get_prop(&copied_style, LV_STYLE_OUTLINE_COLOR, &value) == LV_STYLE_RES_NOT_FOUND);

    /* Changing the original style should not impact the style that copied it */
    lv_style_set_bg_color(&style2, lv_color_hex(0x00ff00));
    TEST_ASSERT_TRUE(lv_style_get_prop(&copied_style, LV_STYLE_BG_COLOR, &value) == LV_STYLE_RES_FOUND);
    TEST_ASSERT_EQUAL_COLOR(copied_bg_color, value.color);

    lv_style_reset(&style1);
    lv_style_reset(&style2);
    lv_style_reset(&copied_style);
}

void test_style_merge(void)
{
    lv_style_t style1;
    lv_style_t style2;
    lv_style_t merged_style;

    const lv_color_t merged_bg_color = lv_color_hex(0x0000ff);
    const lv_color_t merged_border_color = lv_color_hex(0x00ff00);
    const lv_color_t merged_outline_color = lv_color_hex(0xffff00);

    lv_style_init(&style1);
    lv_style_set_bg_color(&style1, lv_color_hex(0xff0000)); // this should get overwritten
    lv_style_set_outline_color(&style1, merged_outline_color);

    lv_style_init(&style2);
    lv_style_set_bg_color(&style2, merged_bg_color);
    lv_style_set_border_color(&style2, merged_border_color);

    lv_style_init(&merged_style);
    lv_style_merge(&merged_style, &style1);
    lv_style_merge(&merged_style, &style2);

    lv_style_value_t value;

    TEST_ASSERT_TRUE(lv_style_get_prop(&merged_style, LV_STYLE_BG_COLOR, &value) == LV_STYLE_RES_FOUND);
    TEST_ASSERT_EQUAL_COLOR(merged_bg_color, value.color);
    TEST_ASSERT_TRUE(lv_style_get_prop(&merged_style, LV_STYLE_BORDER_COLOR, &value) == LV_STYLE_RES_FOUND);
    TEST_ASSERT_EQUAL_COLOR(merged_border_color, value.color);
    TEST_ASSERT_TRUE(lv_style_get_prop(&merged_style, LV_STYLE_OUTLINE_COLOR, &value) == LV_STYLE_RES_FOUND);
    TEST_ASSERT_EQUAL_COLOR(merged_outline_color, value.color);

    /* Changing the original style should not impact the style that copied it */
    lv_style_set_bg_color(&style2, lv_color_hex(0x00ff00));
    TEST_ASSERT_TRUE(lv_style_get_prop(&merged_style, LV_STYLE_BG_COLOR, &value) == LV_STYLE_RES_FOUND);
    TEST_ASSERT_EQUAL_COLOR(merged_bg_color, value.color);

    lv_style_reset(&style1);
    lv_style_reset(&style2);
    lv_style_reset(&merged_style);
}

void test_style_has_prop(void)
{
    lv_style_t style;
    lv_style_init(&style);
    lv_style_set_outline_color(&style, lv_color_white());

    /*Create object with style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    TEST_ASSERT_EQUAL(false, lv_obj_has_style_prop(obj, LV_PART_MAIN, LV_STYLE_OUTLINE_COLOR));
    TEST_ASSERT_EQUAL(false, lv_obj_has_style_prop(obj, LV_PART_MAIN, LV_STYLE_OUTLINE_WIDTH));
    TEST_ASSERT_EQUAL(false, lv_obj_has_style_prop(obj, LV_PART_INDICATOR, LV_STYLE_OUTLINE_COLOR));

    lv_obj_add_style(obj, &style, LV_PART_MAIN);
    lv_obj_set_style_outline_width(obj, 2, LV_PART_MAIN);

    TEST_ASSERT_EQUAL(true, lv_obj_has_style_prop(obj, LV_PART_MAIN, LV_STYLE_OUTLINE_COLOR));
    TEST_ASSERT_EQUAL(true, lv_obj_has_style_prop(obj, LV_PART_MAIN, LV_STYLE_OUTLINE_WIDTH));
    TEST_ASSERT_EQUAL(false, lv_obj_has_style_prop(obj, LV_PART_INDICATOR, LV_STYLE_OUTLINE_COLOR));

    lv_style_reset(&style);
}

#endif
