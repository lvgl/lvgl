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

void test_gltf_render_camera_orientation(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("pbr_textures.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_camera_front"));

    lv_gltf_set_yaw(gltf, 90.0f);
    lv_gltf_set_pitch(gltf, 30.0f);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_camera_turned"));
}

void test_gltf_render_distance(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("pbr_textures.gltf")));
    lv_gltf_set_background_mode(gltf, LV_GLTF_BG_MODE_SOLID);

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_distance_near"));

    /* The model covers fewer pixels when the camera moves away from it */
    lv_gltf_set_distance(gltf, 8.0f);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_distance_far"));
}

/* Turning a node around X, Y and Z are three different rotations, and each of them
 * changes the image in its own way */
void test_gltf_render_node_rotation_axes(void)
{
    lv_obj_t * gltf = create_view();
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, ASSET("pbr_textures.gltf"));
    TEST_ASSERT_NOT_NULL(model);
    lv_gltf_set_background_mode(gltf, LV_GLTF_BG_MODE_SOLID);

    lv_gltf_model_node_t * cube = lv_gltf_model_node_get_by_index(model, 0);
    TEST_ASSERT_NOT_NULL(cube);

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_rotation_none"));

    const float quarter_turn = 0.7854f;
    lv_gltf_model_node_set_rotation_x(cube, quarter_turn);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_rotation_x"));

    lv_gltf_model_node_set_rotation_x(cube, 0.0f);
    lv_gltf_model_node_set_rotation_y(cube, quarter_turn);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_rotation_y"));

    lv_gltf_model_node_set_rotation_y(cube, 0.0f);
    lv_gltf_model_node_set_rotation_z(cube, quarter_turn);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_rotation_z"));
}

void test_gltf_render_animation_advances(void)
{
    lv_obj_t * gltf = create_view();
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, ASSET("animation.gltf"));
    TEST_ASSERT_NOT_NULL(model);
    lv_gltf_set_background_mode(gltf, LV_GLTF_BG_MODE_SOLID);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_play_animation(model, 0));
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_animation_start"));

    lv_test_fast_forward(500);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_animation_later"));

    /* Once paused the image must stay the same, so the same reference still matches */
    lv_gltf_model_pause_animation(model);
    lv_test_fast_forward(500);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_animation_later"));
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_render_camera_orientation(void)
{
}

void test_gltf_render_distance(void)
{
}

void test_gltf_render_node_rotation_axes(void)
{
}

void test_gltf_render_animation_advances(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
