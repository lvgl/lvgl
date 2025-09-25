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

    lv_xml_register_component_from_data("lin_grad", lin_grad_xml);
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

    lv_xml_register_component_from_data("rad_grad_def", rad_grad_def_xml);
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

    lv_xml_register_component_from_data("rad_grad_ofs", rad_grad_ofs_xml);
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

    lv_xml_register_component_from_data("con_grad_def", con_grad_def_xml);
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

    lv_xml_register_component_from_data("con_grad", con_grad_xml);
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

    lv_xml_register_component_from_data("hor_grad", hor_grad_xml);
    lv_xml_create(lv_screen_active(), "hor_grad", NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/gradients.png");
}



void test_xml_style_same_name(void)
{
    const char * comp1 =
        "<component>"
        "  <styles>"
        "    <style name=\"hello\" bg_opa=\"100\" border_width=\"2\"/>"
        "    <style name=\"hello\" figma_node_id=\"1234\" bg_opa=\"20\" border_side=\"right\"/>"
        "  </styles>"
        ""
        "  <view>"
        "     <style name=\"hello\"/>"
        "  </view>"
        "</component>";

    lv_xml_register_component_from_data("comp1", comp1);
    lv_obj_t * obj = lv_xml_create(lv_screen_active(), "comp1", NULL);
    TEST_ASSERT_EQUAL(20, lv_obj_get_style_bg_opa(obj, LV_PART_MAIN));
}



void test_xml_style_remove(void)
{
    const char * comp1 =
        "<component>"
        "  <styles>"
        "    <style name=\"s1\" bg_opa=\"100\" border_width=\"2\"/>"
        "    <style name=\"s2\" bg_color=\"0xff0000\"/>"
        "  </styles>"
        ""
        "  <view extends=\"lv_slider\">"
        "     <remove_style/>"
        "     <remove_style selector=\"knob\"/>"
        "     <style name=\"s1\" selector=\"knob|pressed\"/>"
        "     <style name=\"s2\" selector=\"knob|pressed\"/>"
        "     <remove_style name=\"s2\" selector=\"knob|pressed\"/>"
        "     <remove_style name=\"s3\"/>"
        "  </view>"
        "</component>";

    lv_xml_register_component_from_data("comp1", comp1);
    lv_obj_t * obj = lv_xml_create(lv_screen_active(), "comp1", NULL);

    /*The first remove_style removed the main style, so the radius should be the default 0*/
    TEST_ASSERT_EQUAL(0, lv_obj_get_style_radius(obj, LV_PART_MAIN));

    /*Styles are removed from the knob in the default state*/
    TEST_ASSERT_EQUAL(0, lv_obj_get_style_radius(obj, LV_PART_KNOB));

    lv_obj_add_state(obj, LV_STATE_PRESSED);
    lv_test_wait(1000); /*Wait for transitions*/

    /*s1 sets bg_opa=100 in pressed state*/
    TEST_ASSERT_EQUAL(100, lv_obj_get_style_bg_opa(obj, LV_PART_KNOB));

    /*s2 is added and removed so the red bg_color shouldn't be applied*/
    TEST_ASSERT_NOT_EQUAL_COLOR(lv_color_hex(0xff0000), lv_obj_get_style_bg_color(obj, LV_PART_KNOB));

    lv_xml_component_unregister("comp1");
}

void test_xml_style_binding(void)
{
    const char * comp1_xml = {
        "<component>"
        " 	<subjects>"
        " 		<int name=\"subject1\" value=\"3\"/>"
        " 	</subjects>"
        " 	<styles>"
        " 		<style name=\"style1\" bg_color=\"0xff0000\"/>"
        " 	</styles>"
        " 	<view>"
        " 		<bind_style name=\"style1\" selector=\"scrollbar\" subject=\"subject1\" ref_value=\"5\"/>"
        " 	</view>"
        "</component>"
    };

    lv_xml_register_component_from_data("comp1", comp1_xml);

    lv_obj_t * obj = lv_xml_create(lv_screen_active(), "comp1", NULL);

    TEST_ASSERT_NOT_EQUAL_COLOR(lv_color_hex(0xff0000), lv_obj_get_style_bg_color(obj, LV_PART_SCROLLBAR));

    lv_subject_t * subject = lv_xml_get_subject(lv_xml_component_get_scope("comp1"), "subject1");
    lv_subject_set_int(subject, 5);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex(0xff0000), lv_obj_get_style_bg_color(obj, LV_PART_SCROLLBAR));
}
#endif
