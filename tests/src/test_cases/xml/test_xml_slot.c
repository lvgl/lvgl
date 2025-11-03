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
    lv_obj_clean(lv_screen_active());
}

void test_xml_slot_1(void)
{
    const char * my_button =
        "<component>"
        "  <api>"
        "    <slot name=\"slot1\"/>"
        "    <prop name=\"label\" type=\"string\" default=\"Hello world\"/>"
        "  </api>"
        ""
        "  <view width=\"content\" height=\"content\">"
        "    <lv_button name=\"slot1\" flex_flow=\"row\">"
        "      <lv_label text=\"$label\"/>"
        "    </lv_button>"
        "  </view>"
        "</component>";

    const char * my_list =
        "<component>"
        "  <view flex_flow=\"column\" width=\"300\" height=\"400\">"
        "    <my_button/>"
        "    <my_button label=\"Custom label\">"
        "		<my_button-slot1>"
        "		  <lv_label text=\"New label\" style_text_color=\"0xf00\"/>"
        "		</my_button-slot1>"
        "    </my_button>"
        "  </view>"
        "</component>";

    lv_result_t res;

    res = lv_xml_register_component_from_data("my_button", my_button);
    TEST_ASSERT_EQUAL_INT(LV_RESULT_OK, res);

    res = lv_xml_register_component_from_data("my_list", my_list);
    TEST_ASSERT_EQUAL_INT(LV_RESULT_OK, res);

    lv_obj_t * my_list_0 = lv_xml_create(lv_screen_active(), "my_list", NULL);

    lv_obj_t * new_label = lv_obj_get_child_by_name(my_list_0, "my_button_1/slot1/lv_label_1");

    TEST_ASSERT_EQUAL_STRING("New label", lv_label_get_text(new_label));
}

#endif
