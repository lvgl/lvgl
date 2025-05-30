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


void test_xml_widget_direct_create(void)
{
    lv_obj_set_style_pad_all(lv_screen_active(), 16, 0);

    lv_obj_t * slider;

    /*Simple create*/
    slider = lv_xml_create(lv_screen_active(), "lv_slider", NULL);

    /*Adjust the returned widget*/
    slider = lv_xml_create(lv_screen_active(), "lv_slider", NULL);
    lv_obj_set_pos(slider, 10, 100);
    lv_slider_set_value(slider, 40, LV_ANIM_OFF);

    /*Use attributes*/
    const char * attrs[] = {
        "min_value", "-100",
        "max_value", "100",
        "mode", "symmetrical",
        "value", "50",
        "name", "my_slider",
        NULL, NULL,
    };

    slider = lv_xml_create(lv_screen_active(), "lv_slider", attrs);
    lv_obj_set_pos(slider, 10, 200);
#if LV_USE_OBJ_NAME
    lv_obj_t * same_slider = lv_obj_get_child_by_name(lv_screen_active(), "my_slider");
    TEST_ASSERT_EQUAL_PTR(slider, same_slider);
#endif
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/widget_create_1.png");
}

void test_xml_widget_create_from_component(void)
{
    lv_obj_set_style_pad_all(lv_screen_active(), 16, 0);

    const char * red_slider_xml =
        "<component>"
        "<view extends=\"lv_slider\" value=\"20\" width=\"100%\" style_bg_color=\"0xff0000\">"
        "</view>"
        "</component>";

    lv_xml_component_register_from_data("red_slider", red_slider_xml);

    lv_obj_t * slider;

    /*Simple create*/
    slider = lv_xml_create(lv_screen_active(), "red_slider", NULL);

    /*Adjust the returned widget*/
    slider = lv_xml_create(lv_screen_active(), "red_slider", NULL);
    lv_obj_set_pos(slider, 10, 100);
    lv_slider_set_value(slider, 40, LV_ANIM_OFF);

    /*Use attributes*/
    const char * attrs[] = {
        "min_value", "-100",
        "max_value", "100",
        "mode", "symmetrical",
        "value", "50",
        NULL, NULL,
    };

    slider = lv_xml_create(lv_screen_active(), "red_slider", attrs);
    lv_obj_set_pos(slider, 10, 200);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/component_create_1.png");
}

void test_xml_nesting(void)
{
    const char * red_button_xml =
        "<component>"
        "<view extends=\"lv_button\" radius=\"0\" style_bg_color=\"0xff0000\">"
        "</view>"
        "</component>";

    const char * card_xml =
        "<component>"
        "<view width=\"200\" height=\"content\">"
        "<lv_label text=\"Some text here\" align=\"top_mid\"/>"
        "<red_button y=\"20\">"
        "<lv_label text=\"Button 1\" align=\"center\"/>"
        "</red_button>"
        "</view>"
        "</component>";

    lv_xml_component_register_from_data("red_button", red_button_xml);
    lv_xml_component_register_from_data("card", card_xml);

    lv_obj_t * card;
    card = lv_xml_create(lv_screen_active(), "card", NULL);
    card = lv_xml_create(lv_screen_active(), "card", NULL);
    lv_obj_set_y(card, 80);

    /*Use attributes*/
    const char * attrs[] = {
        "y", "160",
        NULL, NULL,
    };
    card = lv_xml_create(lv_screen_active(), "card", attrs);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/nested_1.png");
}

/*Pass style and simple properties 3 level deep*/
void test_xml_component_params(void)
{
    const char * h3_xml =
        "<component>"
        "  <api>"
        "    <prop name=\"style\" type=\"style\"/>"
        "  </api>"
        "  <view extends=\"lv_label\">"
        "    <style name=\"$style\"/>"
        "  </view>"
        "</component>";

    const char * red_button_xml =
        "<component>"
        "  <api>"
        "    <prop type=\"string\" name=\"btn_text\"/>"
        "    <prop type=\"style\" name=\"label_style\"/>"
        "  </api>"
        "  <view extends=\"lv_button\" style_radius=\"0\" style_bg_color=\"0xff0000\">"
        "    <h3 text=\"$btn_text\"> "
        "      <style name=\"$label_style\"/>"
        "    </h3>"
        "  </view>"
        "</component>";

    const char * card_xml =
        "<component>"
        "  <api>"
        "    <prop type=\"string\" name=\"action\" default=\"Default\"/>"
        "  </api>"
        "  <styles>"
        "    <style name=\"style1\" text_color=\"0xffff00\"/>"
        "  </styles>"
        "  <view width=\"200\" height=\"content\">"
        "    <h3 text=\"Title\" align=\"top_mid\" style_text_color=\"0xff0000\"/>"
        "    <red_button btn_text=\"$action\" label_style=\"style1\" y=\"20\"/>"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("h3", h3_xml);
    lv_xml_component_register_from_data("red_button", red_button_xml);
    lv_xml_component_register_from_data("card", card_xml);

    lv_xml_create(lv_screen_active(), "card", NULL);

    /*Use attributes*/
    const char * attrs[] = {
        "y", "100",
        "action", "Ext. text",
        NULL, NULL,
    };
    lv_xml_create(lv_screen_active(), "card", attrs);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/params_1.png");
}

void test_xml_component_consts(void)
{
    const char * h3_xml =
        "<component>"
        "  <consts>"
        "    <string name=\"action\" value=\"Log in\"/>"
        "    <color name=\"dark_color\" value=\"0x804000\"/>"
        "    <color name=\"accent_color\" value=\"0xff8000\"/>"
        "    <int name=\"size\" value=\"200\"/>"
        "  </consts>"
        ""
        "  <styles>"
        "    <style name=\"style1\" bg_color=\"#dark_color\" bg_opa=\"255\"/>"
        "  </styles>"
        ""
        "  <view extends=\"lv_label\" width=\"#size\" style_text_color=\"#accent_color\" text=\"#action\">"
        "  	<style name=\"style1\"/>"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("h3", h3_xml);

    lv_xml_create(lv_screen_active(), "h3", NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/consts_1.png");
}

void test_xml_component_styles(void)
{
    const char * my_btn_xml =
        "<component>"
        "  <styles>"
        "    <style name=\"rel_style\" bg_color=\"0xff0000\"/>"
        "    <style name=\"pr_style\" bg_color=\"0x800000\"/>"
        "  </styles>"
        "  <view extends=\"lv_button\" style_text_color=\"0x0000ff\" style_text_color:checked=\"0xa0a0ff\">"
        "    <style name=\"rel_style\"/>"
        "    <style name=\"pr_style\" selector=\"checked\"/>"
        "    <lv_label/>"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("my_btn", my_btn_xml);

    lv_xml_create(lv_screen_active(), "my_btn", NULL);
    lv_obj_t * btn = lv_xml_create(lv_screen_active(), "my_btn", NULL);
    lv_obj_set_pos(btn, 0, 100);
    lv_obj_add_state(btn, LV_STATE_CHECKED);

    lv_test_wait(300); /*Wait for the state transition animation*/
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/styles_1.png");
}

void test_xml_error_resilience_syntax_ok(void)
{
    const char * my_btn_xml =
        "<component>"
        "  <consts>"
        "      <int name=\"abc\" value=\"0xff0000\"/>"
        "      <not_a_type name=\"xyz\" value=\"0xff0000\"/>"
        "      <int not_a_prop=\"abc\" value=\"0xff0000\"/>"
        "      <int name=\"abc\" not_a_value=\"0xff0000\"/>"
        "  </consts>"
        ""
        "  <styles>"
        "    <style name=\"rel_style\" bg_color=\"0xff0000\" not_a_prop=\"0xff0000\"/>"
        "    <inv_style name=\"rel_style\" bg_color=\"0x800000\"/>"
        "    <style bg_color=\"0x800000\"/>"
        "  </styles>"
        ""
        "  <view extends=\"not_a_widget\" style_text_color=\"0x0000ff\" style_text_color:checked=\"0x8080ff\">"
        "    <unknown/>"
        "    <lv_label not_an_attr=\"40\"/>"
        "  </view>"
        "</component>";

    lv_xml_component_register_from_data("my_btn", my_btn_xml);

    lv_obj_t * btn = lv_xml_create(lv_screen_active(), "my_btn", NULL);
    if(btn) lv_obj_set_pos(btn, 0, 100);
}

void test_xml_image_and_font(void)
{
    const char * btn_xml =
        "<component>"
        "  <consts>"
        "    <font name=\"font1\" value=\"lv_montserrat_18\"/>"
        "    <image name=\"image1\" value=\"test_img1\"/>"
        "  </consts>"
        ""
        "  <styles>"
        "    <style name=\"style_rel\" text_font=\"#font1\" text_color=\"0xffffff\" bg_image_src=\"#image1\" bg_image_tiled=\"true\" radius=\"0\"/>"
        "    <style name=\"style_chk\" text_font=\"lv_montserrat_16\" text_color=\"0xffff00\" bg_image_src=\"test_img2\"/>"
        "  </styles>"
        ""
        "  <view extends=\"lv_obj\" width=\"100\" height=\"70\">"
        "    <style name=\"style_rel\"/>"
        "    <style name=\"style_chk\" selector=\"checked\"/>"
        "    <lv_label text=\"hello\" align=\"center\" style_bg_color=\"0x888888\" style_bg_opa=\"200\"/>"
        "  </view>"
        "</component>";

    /*Monstserrat fonts are registered by LVGL */
    LV_IMAGE_DECLARE(img_render_lvgl_logo_l8);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_rgb565);
    lv_xml_register_image(NULL, "test_img1", &img_render_lvgl_logo_l8);
    lv_xml_register_image(NULL, "test_img2", &img_render_lvgl_logo_rgb565);

    lv_xml_register_font(NULL, "lv_montserrat_16", &lv_font_montserrat_16);
    lv_xml_register_font(NULL, "lv_montserrat_18", &lv_font_montserrat_18);

    lv_xml_component_register_from_data("btn", btn_xml);

    lv_obj_t * btn;
    btn = lv_xml_create(lv_screen_active(), "btn", NULL);

    btn = lv_xml_create(lv_screen_active(), "btn", NULL);
    lv_obj_set_pos(btn, 0, 100);
    lv_obj_add_state(btn, LV_STATE_CHECKED);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/image_and_font_1.png");
}

void test_xml_error_resilience_not_closed_tag(void)
{
    const char * my_btn_xml =
        "<component>"
        "<view extends=\"lv_button\">"
        "<lv_label/>"
        "</component>";

    lv_xml_component_register_from_data("my_btn", my_btn_xml);

    lv_obj_t * btn = lv_xml_create(lv_screen_active(), "my_btn", NULL);
    if(btn) lv_obj_set_pos(btn, 0, 100);
}

void test_xml_error_resilience_string(void)
{
    const char * my_btn_xml =
        "<component>"
        "<view extends=\"lv_button>"
        "<lv_label/>"
        "</component>";

    lv_xml_component_register_from_data("my_btn", my_btn_xml);

    lv_obj_t * btn = lv_xml_create(lv_screen_active(), "my_btn", NULL);
    if(btn) lv_obj_set_pos(btn, 0, 100);
}

void test_xml_complex(void)
{
    lv_xml_component_register_from_file("A:src/test_assets/xml/my_h3.xml");
    lv_xml_component_register_from_file("A:src/test_assets/xml/my_card.xml");
    lv_xml_component_register_from_file("A:src/test_assets/xml/my_button.xml");
    lv_xml_component_register_from_file("A:src/test_assets/xml/view.xml");

    lv_obj_t * obj = lv_xml_create(lv_screen_active(), "view", NULL);
    lv_obj_set_pos(obj, 10, 10);

    const char * my_button_attrs[] = {
        "x", "10",
        "y", "-10",
        "align", "bottom_left",
        "btn_text", "New button",
        NULL, NULL,
    };

    lv_xml_create(lv_screen_active(), "my_button", my_button_attrs);

    const char * slider_attrs[] = {
        "x", "200",
        "y", "-15",
        "align", "bottom_left",
        "value", "30",
        NULL, NULL,
    };

    lv_obj_t * slider = lv_xml_create(lv_screen_active(), "lv_slider", slider_attrs);
    lv_obj_set_width(slider, 100);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/complex_1.png");
}

#endif
