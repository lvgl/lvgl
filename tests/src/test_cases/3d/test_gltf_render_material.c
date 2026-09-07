#if LV_BUILD_TEST
#include "../../lvgl.h"

#if LV_USE_GLTF

#include "unity/unity.h"

#define ASSET(name) "A:src/test_assets/gltf/generated/" name

#ifdef NON_AMD64_BUILD
    #define REF(name) "gltf/" name ".lp32.png"
#else
    #define REF(name) "gltf/" name ".lp64.png"
#endif

#define VIEW_SIZE 160

void setUp(void)
{
    /* A known screen color makes it possible to tell rendered pixels from the background */
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static lv_obj_t * create_view(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf, VIEW_SIZE, VIEW_SIZE);
    lv_obj_set_pos(gltf, 0, 0);
    return gltf;
}

void test_gltf_render_all_material_features(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("materials.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_materials"));
}

void test_gltf_render_model_added_again(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("materials.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(gltf, model));
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_materials"));

    lv_gltf_remove_model(gltf, model);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(gltf, model));
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_materials"));

    lv_gltf_model_delete(model);
}

/* The sheen material of this asset has a black base color and no dielectric specular,
 * so everything that shows up comes from the sheen lobe. It stays black when the sheen
 * lighting tables are not generated. */
void test_gltf_render_sheen_material_is_lit(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("sheen.gltf")));
    lv_gltf_set_background_mode(gltf, LV_GLTF_BG_MODE_SOLID);
    /* Sheen shows up at grazing angles, so the cube is turned away from the camera, and
     * the environment is turned up to lift the dim lobe above the 8 bit quantization */
    lv_gltf_set_yaw(gltf, 45.0f);
    lv_gltf_set_pitch(gltf, 35.0f);
    lv_gltf_set_environment_brightness(gltf, 10.f);
    lv_gltf_set_image_exposure(gltf, 4.0f);

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_sheen"));
}

void test_gltf_render_lights_and_skin(void)
{
    lv_obj_t * lights = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(lights, ASSET("lights.gltf")));

    lv_obj_t * skin = create_view();
    lv_obj_set_pos(skin, VIEW_SIZE, 0);
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(skin, ASSET("skin.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_lights_and_skin"));
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_render_all_material_features(void)
{
}

void test_gltf_render_model_added_again(void)
{
}

void test_gltf_render_sheen_material_is_lit(void)
{
}

void test_gltf_render_lights_and_skin(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
