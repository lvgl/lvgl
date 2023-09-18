#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_scr_act());
}


static const char * lorem_ipsum =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed sit amet lectus accumsan, venenatis ex sit amet, porttitor ex. Mauris ornare orci ante, sed aliquet quam elementum quis. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Mauris posuere nibh ex, at congue quam porttitor volutpat. Sed id odio ut lorem sodales feugiat. Phasellus suscipit congue risus ac congue. Vivamus eu justo sapien. Etiam a lectus quis nibh sodales scelerisque. Cras auctor pharetra felis auctor eleifend. Nullam et dui sollicitudin eros ornare suscipit eget porta lectus. Curabitur imperdiet nunc sapien, non condimentum turpis molestie eget.";

static lv_obj_t * create_test_obj(const char * text, bool simple)
{
    lv_obj_t * obj = lv_button_create(lv_scr_act());
    lv_obj_set_size(obj, 120, 80);

    if(simple) {
        lv_obj_set_style_radius(obj, 0, 0);
        lv_obj_set_style_shadow_width(obj, 0, 0);
    }


    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label, text);
    lv_obj_center(label);

    return obj;
}

void test_layer_mixed_rgb(void)
{
    lv_obj_t * obj;

    obj = create_test_obj("Normal", true);

    obj = create_test_obj("Opa = 50%", true);
    lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);

    obj = create_test_obj("Opa = 20%", true);
    lv_obj_set_style_opa_layered(obj, LV_OPA_20, 0);

    obj = create_test_obj("Opa = 90%", true);
    lv_obj_set_style_opa_layered(obj, LV_OPA_90, 0);

    obj = create_test_obj("Subtractive", true);
    lv_obj_set_style_blend_mode(obj, LV_BLEND_MODE_SUBTRACTIVE, 0);

    obj = create_test_obj("angle = 30°", true);
    lv_obj_set_style_transform_angle(obj, 300, 0);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    obj = create_test_obj("Zoom = 120%", true);
    lv_obj_set_style_transform_zoom(obj, 307, 0);

    obj = create_test_obj("Zoom = 70%", true);
    lv_obj_set_style_transform_zoom(obj, 180, 0);

    obj = create_test_obj("Zoom = 70%\nAngle = 70°", true);
    lv_obj_set_style_transform_zoom(obj, 180, 0);
    lv_obj_set_style_transform_angle(obj, 700, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_mixed_rgb.png");
}

void test_layer_mixed_argb(void)
{
    lv_obj_t * obj;

    obj = create_test_obj("Normal", false);

    obj = create_test_obj("Opa = 50%", false);
    lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);

    obj = create_test_obj("Opa = 20%", false);
    lv_obj_set_style_opa_layered(obj, LV_OPA_20, 0);

    obj = create_test_obj("Opa = 90%", false);
    lv_obj_set_style_opa_layered(obj, LV_OPA_90, 0);

    obj = create_test_obj("Subtractive", false);
    lv_obj_set_style_blend_mode(obj, LV_BLEND_MODE_SUBTRACTIVE, 0);

    obj = create_test_obj("angle = 30°", false);
    lv_obj_set_style_transform_angle(obj, 300, 0);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    obj = create_test_obj("Zoom = 120%", false);
    lv_obj_set_style_transform_zoom(obj, 307, 0);

    obj = create_test_obj("Zoom = 70%", false);
    lv_obj_set_style_transform_zoom(obj, 180, 0);

    obj = create_test_obj("Zoom = 70%\nAngle = 70°", false);
    lv_obj_set_style_transform_zoom(obj, 180, 0);
    lv_obj_set_style_transform_angle(obj, 700, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_mixed_argb.png");
}

void test_layer_large_opa_argb(void)
{
    lv_obj_t * obj;
    obj = create_test_obj(lorem_ipsum, false);
    lv_obj_set_width(lv_obj_get_child(obj, 0), lv_pct(100));
    lv_obj_set_size(obj, 600, 400);
    lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_large_opa_argb.png");
}

void test_layer_large_opa_rgb(void)
{
    lv_obj_t * obj;
    obj = create_test_obj(lorem_ipsum, true);
    lv_obj_set_width(lv_obj_get_child(obj, 0), lv_pct(100));
    lv_obj_set_size(obj, 600, 400);
    lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_large_opa_rgb.png");
}

void test_layer_large_transform(void)
{
    lv_obj_t * obj;
    obj = create_test_obj(lorem_ipsum, false);
    lv_obj_set_width(lv_obj_get_child(obj, 0), lv_pct(100));
    lv_obj_set_size(obj, 200, 150);
    lv_obj_set_style_transform_zoom(obj, 200, 0);
    lv_obj_set_style_transform_angle(obj, 200, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_large_transform.png");
}

#endif
