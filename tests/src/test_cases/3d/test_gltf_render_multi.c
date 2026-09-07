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
static lv_gltf_model_t * model;
void setUp(void)
{
    /* A known screen color makes it possible to tell rendered pixels from the background */
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);
}

void tearDown(void)
{
    lv_gltf_model_delete(model);
    model = NULL;
    lv_obj_clean(lv_screen_active());
}

static lv_obj_t * create_view(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf, VIEW_SIZE, VIEW_SIZE);
    lv_obj_set_pos(gltf, 0, 0);
    return gltf;
}

void test_gltf_render_two_models_in_one_view(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_one_model"));

    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf")));
    TEST_ASSERT_EQUAL(2, lv_gltf_get_model_count(gltf));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_two_models"));
}

void test_gltf_render_two_views_side_by_side(void)
{
    lv_obj_t * left = create_view();
    lv_obj_t * right = create_view();
    lv_obj_set_pos(right, VIEW_SIZE, 0);

    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(left, ASSET("minimal_triangle.gltf")));
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(right, ASSET("materials.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_two_views"));
}

void test_gltf_render_after_removing_a_model(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_one_model"));

    lv_gltf_model_t * second = lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf"));
    TEST_ASSERT_NOT_NULL(second);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_two_models"));

    lv_gltf_remove_model(gltf, second);
    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_one_model"));
}

void test_gltf_render_after_removing_a_transmissive_model(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    lv_gltf_model_t * transmissive = lv_gltf_load_model_from_file(gltf, ASSET("materials.gltf"));
    TEST_ASSERT_NOT_NULL(transmissive);

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_gltf_remove_model(gltf, transmissive);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_one_model"));
}

void test_gltf_render_after_removing_all_models(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_one_model"));

    lv_gltf_remove_all_models(gltf);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_without_model"));
}

void test_gltf_render_shared_model_after_deleting_a_view(void)
{
    lv_obj_t * left = create_view();
    lv_obj_t * right = create_view();
    lv_obj_set_pos(right, VIEW_SIZE, 0);

    model = lv_gltf_data_load_from_file(ASSET("minimal_triangle.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(left, model));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(right, model));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_both_views"));

    lv_obj_delete(right);

    /* Move the camera and put it back, so the survivor really redraws the model instead of
     * handing out the frame it already had */
    lv_gltf_set_yaw(left, 15.0f);
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
    lv_gltf_set_yaw(left, 0.0f);

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_one_model"));
}

void test_gltf_render_after_deleting_another_view(void)
{
    lv_obj_t * first = create_view();
    lv_obj_t * second = create_view();
    lv_obj_set_pos(second, VIEW_SIZE, 0);

    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(first, ASSET("minimal_triangle.gltf")));
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(second, ASSET("minimal_triangle.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_both_views"));

    /* The survivor keeps the image it had while the two views were alive */
    lv_obj_delete(first);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_surviving_view"));
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_render_two_models_in_one_view(void)
{
}

void test_gltf_render_two_views_side_by_side(void)
{
}

void test_gltf_render_after_deleting_another_view(void)
{
}

void test_gltf_render_after_removing_a_model(void)
{
}

void test_gltf_render_after_removing_a_transmissive_model(void)
{
}

void test_gltf_render_after_removing_all_models(void)
{
}

void test_gltf_render_shared_model_after_deleting_a_view(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
