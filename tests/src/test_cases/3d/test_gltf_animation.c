#if LV_BUILD_TEST
#include "../../lvgl.h"

#if LV_USE_GLTF

#include "unity/unity.h"

#define ASSET(name) "A:src/test_assets/gltf/generated/" name

/* animation.gltf holds three animations of the same node: "move", "spin" and "pulse" */
#define ANIMATION_COUNT 3

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static lv_gltf_model_t * load_animated(void)
{
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("animation.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);
    return model;
}

void test_gltf_animation_count(void)
{
    lv_gltf_model_t * model = load_animated();
    TEST_ASSERT_EQUAL(ANIMATION_COUNT, lv_gltf_model_get_animation_count(model));
    lv_gltf_model_delete(model);

    lv_gltf_model_t * still = lv_gltf_data_load_from_file(ASSET("minimal_triangle.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(still);
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_animation_count(still));
    lv_gltf_model_delete(still);
}

/* Nothing plays until an animation is selected */
void test_gltf_animation_paused_by_default(void)
{
    lv_gltf_model_t * model = load_animated();

    TEST_ASSERT_TRUE(lv_gltf_model_is_animation_paused(model));
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_animation(model));

    lv_gltf_model_delete(model);
}

void test_gltf_animation_play_selects_the_animation(void)
{
    lv_gltf_model_t * model = load_animated();

    for(size_t i = 0; i < ANIMATION_COUNT; i++) {
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_play_animation(model, i));
        TEST_ASSERT_EQUAL(i, lv_gltf_model_get_animation(model));
        TEST_ASSERT_FALSE(lv_gltf_model_is_animation_paused(model));
    }

    lv_gltf_model_delete(model);
}

void test_gltf_animation_play_out_of_range_fails(void)
{
    lv_gltf_model_t * model = load_animated();

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_play_animation(model, ANIMATION_COUNT));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_play_animation(model, 1000));

    /* A failed selection leaves the state alone */
    TEST_ASSERT_TRUE(lv_gltf_model_is_animation_paused(model));

    lv_gltf_model_delete(model);
}

void test_gltf_animation_play_without_animations_fails(void)
{
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("minimal_triangle.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_play_animation(model, 0));
    TEST_ASSERT_TRUE(lv_gltf_model_is_animation_paused(model));

    lv_gltf_model_delete(model);
}

void test_gltf_animation_pause_and_resume(void)
{
    lv_gltf_model_t * model = load_animated();

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_play_animation(model, 1));
    TEST_ASSERT_FALSE(lv_gltf_model_is_animation_paused(model));

    lv_gltf_model_pause_animation(model);
    TEST_ASSERT_TRUE(lv_gltf_model_is_animation_paused(model));
    /* Pausing keeps the selected animation */
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_animation(model));

    /* Pausing twice is not an error */
    lv_gltf_model_pause_animation(model);
    TEST_ASSERT_TRUE(lv_gltf_model_is_animation_paused(model));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_play_animation(model, 1));
    TEST_ASSERT_FALSE(lv_gltf_model_is_animation_paused(model));

    lv_gltf_model_delete(model);
}

void test_gltf_animation_speed(void)
{
    lv_gltf_model_t * model = load_animated();

    /* The speed is a ratio of LV_GLTF_ANIM_SPEED_NORMAL, which is the default */
    TEST_ASSERT_EQUAL(LV_GLTF_ANIM_SPEED_NORMAL, lv_gltf_model_get_animation_speed(model));

    const uint32_t speeds[] = {
        LV_GLTF_ANIM_SPEED_TENTH, LV_GLTF_ANIM_SPEED_QUARTER, LV_GLTF_ANIM_SPEED_HALF,
        LV_GLTF_ANIM_SPEED_NORMAL, LV_GLTF_ANIM_SPEED_2X, LV_GLTF_ANIM_SPEED_3X,
        LV_GLTF_ANIM_SPEED_4X, 0u, 12345u
    };
    for(uint32_t i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++) {
        lv_gltf_model_set_animation_speed(model, speeds[i]);
        TEST_ASSERT_EQUAL(speeds[i], lv_gltf_model_get_animation_speed(model));
    }

    lv_gltf_model_delete(model);
}

/* The viewer level speed functions are deprecated but must keep working: they act on
 * the primary model of the viewer */
void test_gltf_animation_deprecated_viewer_speed(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf, 100, 100);
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, ASSET("animation.gltf"));
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL(LV_GLTF_ANIM_SPEED_NORMAL, lv_gltf_get_animation_speed(gltf));

    lv_gltf_set_animation_speed(gltf, LV_GLTF_ANIM_SPEED_HALF);
    TEST_ASSERT_EQUAL(LV_GLTF_ANIM_SPEED_HALF, lv_gltf_get_animation_speed(gltf));
    TEST_ASSERT_EQUAL(LV_GLTF_ANIM_SPEED_HALF, lv_gltf_model_get_animation_speed(model));

    /* And the other way around */
    lv_gltf_model_set_animation_speed(model, LV_GLTF_ANIM_SPEED_3X);
    TEST_ASSERT_EQUAL(LV_GLTF_ANIM_SPEED_3X, lv_gltf_get_animation_speed(gltf));
}

/* Every animation of the skinned and the light asset can be selected too */
void test_gltf_animation_other_assets(void)
{
    const char * assets[] = { ASSET("skin.gltf"), ASSET("lights.gltf") };

    for(uint32_t i = 0; i < sizeof(assets) / sizeof(assets[0]); i++) {
        lv_gltf_model_t * model = lv_gltf_data_load_from_file(assets[i], NULL);
        TEST_ASSERT_NOT_NULL(model);
        TEST_ASSERT_EQUAL(1, lv_gltf_model_get_animation_count(model));
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_play_animation(model, 0));
        TEST_ASSERT_FALSE(lv_gltf_model_is_animation_paused(model));
        lv_gltf_model_delete(model);
    }
}

/* An animation keeps the model dirty, so the viewer redraws while it plays */
void test_gltf_animation_runs_in_a_viewer(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf, 100, 100);
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, ASSET("animation.gltf"));
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_play_animation(model, 0));
    lv_gltf_model_set_animation_speed(model, LV_GLTF_ANIM_SPEED_2X);

    /* Playing an animation over several frames must not upset the viewer */
    for(uint32_t i = 0; i < 4; i++) {
        lv_test_fast_forward(100);
        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_FALSE(lv_gltf_model_is_animation_paused(model));
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_animation(model));
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_animation_count(void)
{
}

void test_gltf_animation_paused_by_default(void)
{
}

void test_gltf_animation_play_selects_the_animation(void)
{
}

void test_gltf_animation_play_out_of_range_fails(void)
{
}

void test_gltf_animation_play_without_animations_fails(void)
{
}

void test_gltf_animation_pause_and_resume(void)
{
}

void test_gltf_animation_speed(void)
{
}

void test_gltf_animation_deprecated_viewer_speed(void)
{
}

void test_gltf_animation_other_assets(void)
{
}

void test_gltf_animation_runs_in_a_viewer(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
