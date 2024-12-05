#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void test_obj_property_fail_on_invalid_id(void)
{
#if LV_USE_OBJ_PROPERTY
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
#endif
}

void test_obj_property_set_get_should_match(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_t * root = lv_obj_create(lv_screen_active());
    lv_property_t prop = { };
    lv_color_t color = {.red = 0x11, .green = 0x22, .blue = 0x33};

    /* Style property should work */
    /* int type */
    prop.id = LV_PROPERTY_STYLE_X;
    prop.num = 0xaabb;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_UINT32(0xaabb, lv_obj_get_style_x(obj, 0));
    TEST_ASSERT_EQUAL_UINT32(0xaabb, lv_obj_get_property(obj, LV_PROPERTY_STYLE_X).num);

    /* color type */
    prop.id = LV_PROPERTY_STYLE_BG_COLOR;
    prop.color = color;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_COLOR(color, lv_obj_get_style_bg_color(obj, LV_PART_MAIN));
    TEST_ASSERT_EQUAL_COLOR(color, lv_obj_get_property(obj, LV_PROPERTY_STYLE_BG_COLOR).color);

    /* pointer type */
    prop.id = LV_PROPERTY_STYLE_TEXT_FONT;
    prop.ptr = &lv_font_montserrat_26;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_PTR(&lv_font_montserrat_26, lv_obj_get_style_text_font(obj, LV_PART_MAIN));
    TEST_ASSERT_EQUAL_PTR(&lv_font_montserrat_26, lv_obj_get_property(obj, LV_PROPERTY_STYLE_TEXT_FONT).ptr);

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
    TEST_ASSERT_EQUAL_UINT16(0x1234, lv_image_get_offset_x(img));
    TEST_ASSERT_EQUAL_UINT16(0x1234, lv_obj_get_property(img, LV_PROPERTY_IMAGE_OFFSET_X).num);
#endif
}

void test_obj_property_style_selector(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_property_t prop = { };

    /* Style property with default selector(0) should work */
    prop.id = LV_PROPERTY_STYLE_X;
    prop.num = 0xaabb;  /* `num` shares same memory with `prop.style.value.num` */
    /* selector is initialed to zero when prop is defined. */
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_UINT32(0xaabb, lv_obj_get_style_x(obj, 0));
    TEST_ASSERT_EQUAL_UINT32(0xaabb, lv_obj_get_style_property(obj, LV_PROPERTY_STYLE_X, 0).num);

    lv_style_selector_t selector = LV_PART_MAIN | LV_STATE_PRESSED;
    prop.id = LV_PROPERTY_STYLE_X;
    prop.num = 0x1122;
    prop.selector = selector;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_UINT32(0x1122, lv_obj_get_style_x(obj, selector));
    TEST_ASSERT_EQUAL_UINT32(0x1122, lv_obj_get_style_property(obj, LV_PROPERTY_STYLE_X, selector).num);
#endif
}

void test_obj_property_flag(void)
{
#if LV_USE_OBJ_PROPERTY
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
#endif
}

void test_obj_property_state(void)
{
#if LV_USE_OBJ_PROPERTY
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
#endif
}

void test_obj_property_type_point(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_image_create(lv_screen_active());
    lv_property_t prop = { };

    prop.id = LV_PROPERTY_IMAGE_PIVOT;
    prop.point.x = 0x1234;
    prop.point.y = 0x5678;

    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    lv_property_t prop_get = lv_obj_get_property(obj, LV_PROPERTY_IMAGE_PIVOT);
    TEST_ASSERT_EQUAL_UINT16(0x1234, prop_get.point.x);
    TEST_ASSERT_EQUAL_UINT16(0x5678, prop_get.point.y);
#endif
}

void test_obj_property_name(void)
{
#if LV_USE_OBJ_PROPERTY && LV_USE_OBJ_PROPERTY_NAME
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    /*Style name*/
    /*If widget property name conflicts with style name, property name comes first.*/
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_OBJ_X, lv_obj_property_get_id(obj, "x"));
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_STYLE_OPA, lv_obj_property_get_id(obj, "opa"));
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_STYLE_BG_MAIN_STOP, lv_obj_property_get_id(obj, "bg_main_stop"));

    /*Widget property*/
    obj = lv_image_create(lv_screen_active());
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_IMAGE_ANTIALIAS, lv_obj_property_get_id(obj, "antialias"));
    /*Base class property*/
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_OBJ_PARENT, lv_obj_property_get_id(obj, "parent"));

    /*OBJ flags*/
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_OBJ_FLAG_CLICKABLE, lv_obj_property_get_id(obj, "flag_clickable"));
#endif
}

void test_obj_class_property(void)
{
#if LV_USE_OBJ_PROPERTY && LV_USE_OBJ_PROPERTY_NAME
    /* An image obj has align property */
    lv_obj_t * img = lv_image_create(lv_screen_active());
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_OBJ_ALIGN, lv_obj_property_get_id(img, "align"));

    /* obj class has align property but image doesn't. */
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_OBJ_ALIGN, lv_obj_class_property_get_id(&lv_obj_class, "align"));
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_ID_INVALID, lv_obj_class_property_get_id(&lv_image_class, "align"));

    /* style can also have property name that conflicts with obj */
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_STYLE_X, lv_style_property_get_id("x"));
    TEST_ASSERT_EQUAL_UINT32(LV_PROPERTY_OBJ_X, lv_obj_property_get_id(img, "x"));
#endif
}

void test_label_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_label_create(lv_screen_active());
    lv_property_t prop = { };

    prop.id = LV_PROPERTY_LABEL_TEXT;
    prop.ptr = "Hello world";
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_STRING("Hello world", lv_label_get_text(obj));
    TEST_ASSERT_EQUAL_STRING("Hello world", lv_obj_get_property(obj, LV_PROPERTY_LABEL_TEXT).ptr);

    prop.id = LV_PROPERTY_LABEL_LONG_MODE;
    prop.num = LV_LABEL_LONG_MODE_SCROLL;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(LV_LABEL_LONG_MODE_SCROLL, lv_label_get_long_mode(obj));
    TEST_ASSERT_EQUAL_INT(LV_LABEL_LONG_MODE_SCROLL, lv_obj_get_property(obj, LV_PROPERTY_LABEL_LONG_MODE).num);

#if LV_LABEL_TEXT_SELECTION
    prop.id = LV_PROPERTY_LABEL_TEXT_SELECTION_START;
    prop.num = 2;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(2, lv_label_get_text_selection_start(obj));
    TEST_ASSERT_EQUAL_INT(2, lv_obj_get_property(obj, LV_PROPERTY_LABEL_TEXT_SELECTION_START).num);

    prop.id = LV_PROPERTY_LABEL_TEXT_SELECTION_END;
    prop.num = 5;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(5, lv_label_get_text_selection_end(obj));
    TEST_ASSERT_EQUAL_INT(5, lv_obj_get_property(obj, LV_PROPERTY_LABEL_TEXT_SELECTION_END).num);
#endif
#endif
}

#endif
