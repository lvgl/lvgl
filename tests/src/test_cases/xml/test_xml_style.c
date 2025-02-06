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

void test_xml_style_gradients(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(lv_screen_active(), 16, 0);

    const char * lin_grad_xml =
        "<component>"
        "  <gradients>"
        "    <linear name=\"grad1\" start=\"50 50\" end=\"100 80\">"
        "      <stop color=\"#ff0000\" offset=\"20%\" opa=\"100%\"/>"
        "      <stop color=\"#00ff00\" offset=\"240\" opa=\"100%\"/>"
        "    </linear>"
        "  </gradients>"
        ""
        "  <view extends=\"lv_obj\" width=\"200\" height=\"150\" style_bg_grad=\"grad1\">"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("lin_grad", lin_grad_xml);
    lv_xml_create(lv_screen_active(), "lin_grad", NULL);

    const char * rad_grad_def_xml =
        "<component>"
        "  <gradients>"
        "    <radial name=\"grad1\">"
        "      <stop color=\"#ff0000\"  opa=\"100%\"/>"
        "      <stop color=\"#00ff00\" opa=\"100%\"/>"
        "    </radial>"
        "  </gradients>"
        ""
        "  <view extends=\"lv_obj\" width=\"200\" height=\"150\" style_bg_grad=\"grad1\">"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("rad_grad_def", rad_grad_def_xml);
    lv_xml_create(lv_screen_active(), "rad_grad_def", NULL);

    const char * rad_grad_ofs_xml =
        "<component>"
        "  <gradients>"
        "    <radial name=\"grad1\" center=\"100 50%\" edge=\"200 50\" "
        "                     focal_center=\"50 80%\" focal_edge=\"55 80%\">"
        "      <stop color=\"#ff0000\"  opa=\"100%\"/>"
        "      <stop color=\"#00ff00\" opa=\"100%\"/>"
        "    </radial>"
        "  </gradients>"
        ""
        "  <view extends=\"lv_obj\" width=\"200\" height=\"150\" style_bg_grad=\"grad1\">"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("rad_grad_ofs", rad_grad_ofs_xml);
    lv_xml_create(lv_screen_active(), "rad_grad_ofs", NULL);

    const char * con_grad_def_xml =
        "<component>"
        "  <gradients>"
        "    <conical name=\"grad1\">"
        "      <stop color=\"#ff0000\"  opa=\"100%\"/>"
        "      <stop color=\"#00ff00\" opa=\"100%\"/>"
        "    </conical>"
        "  </gradients>"
        ""
        "  <view extends=\"lv_obj\" width=\"200\" height=\"150\" style_bg_grad=\"grad1\">"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("con_grad_def", con_grad_def_xml);
    lv_xml_create(lv_screen_active(), "con_grad_def", NULL);

    const char * con_grad_xml =
        "<component>"
        "  <gradients>"
        "    <conical name=\"grad1\" center=\"80 50%\" angle=\"45 270\">"
        "      <stop color=\"#ff0000\"  opa=\"100%\"/>"
        "      <stop color=\"#00ff00\" opa=\"100%\"/>"
        "    </conical>"
        "  </gradients>"
        ""
        "  <view extends=\"lv_obj\" width=\"200\" height=\"150\" style_bg_grad=\"grad1\">"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("con_grad", con_grad_xml);
    lv_xml_create(lv_screen_active(), "con_grad", NULL);

    const char * hor_grad_xml =
        "<component>"
        "  <gradients>"
        "    <horizontal name=\"grad1\">"
        "      <stop color=\"#ff0000\" offset=\"20%\" opa=\"40%\"/>"
        "      <stop color=\"#00ff00\" offset=\"128\" opa=\"100%\"/>"
        "    </horizontal>"
        "  </gradients>"
        ""
        "  <view extends=\"lv_obj\" width=\"200\" height=\"150\" style_bg_grad=\"grad1\">"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("hor_grad", hor_grad_xml);
    lv_xml_create(lv_screen_active(), "hor_grad", NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/gradients.png");
}

#endif
