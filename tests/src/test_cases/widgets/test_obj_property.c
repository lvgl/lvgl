#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_obj_property_fail_on_invalid_id(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_property_t prop = { };

    prop.id = LV_PROPERTY_ID_INVALID;
    TEST_ASSERT_EQUAL_INT(LV_RESULT_INVALID, lv_obj_set_property(obj, &prop));

    prop.id = LV_PROPERTY_ID_BUILTIN_LAST; /* No widget use this ID */
    TEST_ASSERT_EQUAL_INT(LV_RESULT_INVALID, lv_obj_set_property(obj, &prop));

    prop.id = LV_PROPERTY_OBJ_PARENT + 1; /* Not a valid ID for obj */
    TEST_ASSERT_EQUAL_INT(LV_RESULT_INVALID, lv_obj_set_property(obj, &prop));

    prop.id = LV_PROPERTY_IMAGE_OFFSET_X; /* Not an ID for obj but for image */
    TEST_ASSERT_EQUAL_INT(LV_RESULT_INVALID, lv_obj_set_property(obj, &prop));

    prop.id = LV_PROPERTY_OBJ_PARENT; /* Valid ID */
    prop.ptr = lv_screen_active();
    TEST_ASSERT_EQUAL_INT(LV_RESULT_OK, lv_obj_set_property(obj, &prop));
}

void test_obj_property_set_get_should_match(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_t * root = lv_obj_create(lv_screen_active());
    lv_property_t prop = { };
    lv_color_t color = {.red = 0x11, .green = 0x22, .blue = 0x33};

    /* Style property should work */
    /* int type */
    prop.id = LV_STYLE_X;
    prop.num = 0xaabb;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_UINT32(0xaabb, lv_obj_get_style_x(obj, 0));
    TEST_ASSERT_EQUAL_UINT32(0xaabb, lv_obj_get_property(obj, LV_STYLE_X).num);

    /* color type */
    prop.id = LV_STYLE_BG_COLOR;
    prop.color = color;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_COLOR(color, lv_obj_get_style_bg_color(obj, LV_PART_MAIN));
    TEST_ASSERT_EQUAL_COLOR(color, lv_obj_get_property(obj, LV_STYLE_BG_COLOR).color);

    /* pointer type */
    prop.id = LV_STYLE_TEXT_FONT;
    prop.ptr = &lv_font_montserrat_26;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_PTR(&lv_font_montserrat_26, lv_obj_get_style_text_font(obj, LV_PART_MAIN));
    TEST_ASSERT_EQUAL_PTR(&lv_font_montserrat_26, lv_obj_get_property(obj, LV_STYLE_TEXT_FONT).ptr);

    /* Object flags */
    prop.id = LV_PROPERTY_OBJ_FLAG_HIDDEN ;
    prop.num = 1;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_TRUE(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    TEST_ASSERT_TRUE(lv_obj_get_property(obj, LV_PROPERTY_OBJ_FLAG_HIDDEN).num);

    prop.id = LV_PROPERTY_OBJ_FLAG_CLICKABLE;
    prop.num = 0;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_FALSE(lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICKABLE));
    TEST_ASSERT_FALSE(lv_obj_get_property(obj, LV_PROPERTY_OBJ_FLAG_CLICKABLE).num);

    /* Obj property */
    prop.id = LV_PROPERTY_OBJ_PARENT;
    prop.ptr = root;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_PTR(root, lv_obj_get_parent(obj));
    TEST_ASSERT_EQUAL_PTR(root, lv_obj_get_property(obj, LV_PROPERTY_OBJ_PARENT).ptr);

    /* Derived widget could use same property */
    lv_obj_t * img = lv_image_create(obj);
    prop.id = LV_PROPERTY_OBJ_PARENT;
    prop.ptr = root;
    TEST_ASSERT_TRUE(lv_obj_set_property(img, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_PTR(root, lv_obj_get_parent(img));
    TEST_ASSERT_EQUAL_PTR(root, lv_obj_get_property(img, LV_PROPERTY_OBJ_PARENT).ptr);

    /* Image properties */
    prop.id = LV_PROPERTY_IMAGE_OFFSET_X;
    prop.num = 0x1234;
    TEST_ASSERT_TRUE(lv_obj_set_property(img, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_UINT16(0x1234, lv_img_get_offset_x(img));
    TEST_ASSERT_EQUAL_UINT16(0x1234, lv_obj_get_property(img, LV_PROPERTY_IMAGE_OFFSET_X).num);
}

void test_obj_property_flag(void)
{
    const struct {
        uint32_t flag;
        uint32_t id;
    } properties[] = {
        { LV_OBJ_FLAG_HIDDEN,                    LV_PROPERTY_OBJ_FLAG_HIDDEN },
        { LV_OBJ_FLAG_CLICKABLE,                 LV_PROPERTY_OBJ_FLAG_CLICKABLE },
        { LV_OBJ_FLAG_CLICK_FOCUSABLE,           LV_PROPERTY_OBJ_FLAG_CLICK_FOCUSABLE },
        { LV_OBJ_FLAG_CHECKABLE,                 LV_PROPERTY_OBJ_FLAG_CHECKABLE },
        { LV_OBJ_FLAG_SCROLLABLE,                LV_PROPERTY_OBJ_FLAG_SCROLLABLE },
        { LV_OBJ_FLAG_SCROLL_ELASTIC,            LV_PROPERTY_OBJ_FLAG_SCROLL_ELASTIC },
        { LV_OBJ_FLAG_SCROLL_MOMENTUM,           LV_PROPERTY_OBJ_FLAG_SCROLL_MOMENTUM },
        { LV_OBJ_FLAG_SCROLL_ONE,                LV_PROPERTY_OBJ_FLAG_SCROLL_ONE },
        { LV_OBJ_FLAG_SCROLL_CHAIN_HOR,          LV_PROPERTY_OBJ_FLAG_SCROLL_CHAIN_HOR },
        { LV_OBJ_FLAG_SCROLL_CHAIN_VER,          LV_PROPERTY_OBJ_FLAG_SCROLL_CHAIN_VER },
        { LV_OBJ_FLAG_SCROLL_ON_FOCUS,           LV_PROPERTY_OBJ_FLAG_SCROLL_ON_FOCUS },
        { LV_OBJ_FLAG_SCROLL_WITH_ARROW,         LV_PROPERTY_OBJ_FLAG_SCROLL_WITH_ARROW },
        { LV_OBJ_FLAG_SNAPPABLE,                 LV_PROPERTY_OBJ_FLAG_SNAPPABLE },
        { LV_OBJ_FLAG_PRESS_LOCK,                LV_PROPERTY_OBJ_FLAG_PRESS_LOCK },
        { LV_OBJ_FLAG_EVENT_BUBBLE,              LV_PROPERTY_OBJ_FLAG_EVENT_BUBBLE },
        { LV_OBJ_FLAG_GESTURE_BUBBLE,            LV_PROPERTY_OBJ_FLAG_GESTURE_BUBBLE },
        { LV_OBJ_FLAG_ADV_HITTEST,               LV_PROPERTY_OBJ_FLAG_ADV_HITTEST },
        { LV_OBJ_FLAG_IGNORE_LAYOUT,             LV_PROPERTY_OBJ_FLAG_IGNORE_LAYOUT },
        { LV_OBJ_FLAG_FLOATING,                  LV_PROPERTY_OBJ_FLAG_FLOATING },
        { LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS,     LV_PROPERTY_OBJ_FLAG_SEND_DRAW_TASK_EVENTS },
        { LV_OBJ_FLAG_OVERFLOW_VISIBLE,          LV_PROPERTY_OBJ_FLAG_OVERFLOW_VISIBLE },
        { LV_OBJ_FLAG_FLEX_IN_NEW_TRACK,         LV_PROPERTY_OBJ_FLAG_FLEX_IN_NEW_TRACK },
        { LV_OBJ_FLAG_LAYOUT_1,                  LV_PROPERTY_OBJ_FLAG_LAYOUT_1 },
        { LV_OBJ_FLAG_LAYOUT_2,                  LV_PROPERTY_OBJ_FLAG_LAYOUT_2 },
        { LV_OBJ_FLAG_WIDGET_1,                  LV_PROPERTY_OBJ_FLAG_WIDGET_1 },
        { LV_OBJ_FLAG_WIDGET_2,                  LV_PROPERTY_OBJ_FLAG_WIDGET_2 },
        { LV_OBJ_FLAG_USER_1,                    LV_PROPERTY_OBJ_FLAG_USER_1 },
        { LV_OBJ_FLAG_USER_2,                    LV_PROPERTY_OBJ_FLAG_USER_2 },
        { LV_OBJ_FLAG_USER_3,                    LV_PROPERTY_OBJ_FLAG_USER_3 },
        { LV_OBJ_FLAG_USER_4,                    LV_PROPERTY_OBJ_FLAG_USER_4 },
    };

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    obj->flags = 0;
    for(unsigned long i = 0; i < sizeof(properties) / sizeof(properties[0]); i++) {

        TEST_ASSERT_FALSE(lv_obj_get_property(obj, properties[i].id).num);
        lv_obj_add_flag(obj, properties[i].flag);
        TEST_ASSERT_TRUE(lv_obj_get_property(obj, properties[i].id).num);

        lv_obj_remove_flag(obj, properties[i].flag);
        TEST_ASSERT_FALSE(lv_obj_get_property(obj, properties[i].id).num);

        lv_property_t prop = { };
        prop.id = properties[i].id;
        prop.num = 1;
        TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
        TEST_ASSERT_TRUE(lv_obj_get_property(obj, properties[i].id).num);
        TEST_ASSERT_TRUE(lv_obj_has_flag(obj, properties[i].flag));

        prop.id = properties[i].id;
        prop.num = 0;
        TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
        TEST_ASSERT_FALSE(lv_obj_get_property(obj, properties[i].id).num);
        TEST_ASSERT_FALSE(lv_obj_has_flag(obj, properties[i].flag));
    }
}

void test_obj_property_state(void)
{
    const struct {
        uint32_t state;
        uint32_t id;
    } states[] = {
        { LV_STATE_CHECKED,     LV_PROPERTY_OBJ_STATE_CHECKED },
        { LV_STATE_FOCUSED,     LV_PROPERTY_OBJ_STATE_FOCUSED },
        { LV_STATE_FOCUS_KEY,   LV_PROPERTY_OBJ_STATE_FOCUS_KEY },
        { LV_STATE_EDITED,      LV_PROPERTY_OBJ_STATE_EDITED },
        { LV_STATE_HOVERED,     LV_PROPERTY_OBJ_STATE_HOVERED },
        { LV_STATE_PRESSED,     LV_PROPERTY_OBJ_STATE_PRESSED },
        { LV_STATE_SCROLLED,    LV_PROPERTY_OBJ_STATE_SCROLLED },
        { LV_STATE_DISABLED,    LV_PROPERTY_OBJ_STATE_DISABLED },
        { LV_STATE_USER_1,      LV_PROPERTY_OBJ_STATE_USER_1 },
        { LV_STATE_USER_2,      LV_PROPERTY_OBJ_STATE_USER_2 },
        { LV_STATE_USER_3,      LV_PROPERTY_OBJ_STATE_USER_3 },
        { LV_STATE_USER_4,      LV_PROPERTY_OBJ_STATE_USER_4 },
    };

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    obj->state = 0;
    for(unsigned long i = 0; i < sizeof(states) / sizeof(states[0]); i++) {
        TEST_ASSERT_FALSE(lv_obj_get_property(obj, states[i].id).num);
        lv_obj_add_state(obj, states[i].state);
        printf("state: %d, value: %d\n", states[i].state, lv_obj_get_property(obj, states[i].id).num);
        TEST_ASSERT_TRUE(lv_obj_get_property(obj, states[i].id).num);

        lv_obj_remove_state(obj, states[i].state);
        TEST_ASSERT_FALSE(lv_obj_get_property(obj, states[i].id).num);

        lv_property_t prop = { };
        prop.id = states[i].id;
        prop.num = 1;
        TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
        TEST_ASSERT_TRUE(lv_obj_get_property(obj, states[i].id).num);
        TEST_ASSERT_TRUE(lv_obj_get_state(obj) & states[i].state);

        prop.id = states[i].id;
        prop.num = 0;
        TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
        TEST_ASSERT_FALSE(lv_obj_get_property(obj, states[i].id).num);
        TEST_ASSERT_FALSE(lv_obj_get_state(obj) & states[i].state);
    }
}

#endif
