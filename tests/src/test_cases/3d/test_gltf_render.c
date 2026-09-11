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

/* An empty viewer must render without touching the rest of the screen */
void test_gltf_render_without_model(void)
{
    create_view();

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_without_model"));
}

/* This is the first test of the file on purpose: rendering a glTF view as the very
 * first thing in the process caught the view leaving the framebuffer binding at 0. */
void test_gltf_render_draws_into_the_view(void)
{
    lv_obj_t * gltf = create_view();

    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    /* The environment background alone covers the whole view */
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_triangle"));
}

/* Everything outside the view keeps being drawn by the normal draw unit. A glTF view
 * renders through a framebuffer of its own, and it must be restored afterwards. */
void test_gltf_render_keeps_the_rest_of_the_screen(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    lv_obj_t * marker = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(marker);
    lv_obj_set_size(marker, 40, 40);
    lv_obj_set_pos(marker, VIEW_SIZE + 20, 20);
    lv_obj_set_style_bg_color(marker, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_bg_opa(marker, LV_OPA_COVER, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_with_marker"));
}

void test_gltf_render_solid_background_uses_the_style_color(void)
{
    lv_obj_t * gltf = create_view();
    lv_obj_set_style_bg_color(gltf, lv_color_hex(0x0000FF), 0);
    /* The solid background is cleared with the style color and the style opacity */
    lv_obj_set_style_bg_opa(gltf, LV_OPA_COVER, 0);
    lv_gltf_set_background_mode(gltf, LV_GLTF_BG_MODE_SOLID);

    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_solid_background"));
}

void test_gltf_render_antialiasing_modes(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    lv_gltf_set_antialiasing_mode(gltf, LV_GLTF_AA_MODE_OFF);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_aa_off"));

    lv_gltf_set_antialiasing_mode(gltf, LV_GLTF_AA_MODE_ON);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_aa_on"));

    lv_gltf_set_antialiasing_mode(gltf, LV_GLTF_AA_MODE_DYNAMIC);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("render_aa_dynamic"));
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_render_without_model(void)
{
}

void test_gltf_render_draws_into_the_view(void)
{
}

void test_gltf_render_keeps_the_rest_of_the_screen(void)
{
}

void test_gltf_render_solid_background_uses_the_style_color(void)
{
}

void test_gltf_render_antialiasing_modes(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
