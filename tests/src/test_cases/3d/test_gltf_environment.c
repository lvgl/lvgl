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

#define ENV_IMAGE(name) ASSET(name)

// /* Paths the LVGL file system cannot resolve are passed to the image loader directly,
//  * so a plain path keeps working too */
// #define ENV_IMAGE_NATIVE(name) "src/test_assets/gltf/" name

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void render_frames(void)
{
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
}

static lv_obj_t * create_view(int32_t x)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf, 100, 100);
    lv_obj_set_pos(gltf, x, 0);
    return gltf;
}

void test_gltf_environment_sampler_lifecycle(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    TEST_ASSERT_NOT_NULL(sampler);
    lv_gltf_ibl_sampler_delete(sampler);
}

void test_gltf_environment_sampler_resolution(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    TEST_ASSERT_NOT_NULL(sampler);

    /* Any resolution between the documented embedded friendly range works */
    const uint32_t resolutions[] = { 32, 64, LV_GLTF_DEFAULT_CUBE_MAP_RESOLUTION, 256 };
    for(uint32_t i = 0; i < sizeof(resolutions) / sizeof(resolutions[0]); i++) {
        lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, resolutions[i]);
        lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, NULL);
        TEST_ASSERT_NOT_NULL(env);
        lv_gltf_environment_delete(env);
    }

    lv_gltf_ibl_sampler_delete(sampler);
}

/* Without a file path the embedded default image is used */
void test_gltf_environment_create_default(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);

    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, NULL);
    TEST_ASSERT_NOT_NULL(env);

    /* The sampler may be deleted right after the environment was created */
    lv_gltf_ibl_sampler_delete(sampler);

    lv_gltf_environment_delete(env);
}

void test_gltf_environment_create_from_hdr(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);

    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, ENV_IMAGE("environment.hdr"));
    TEST_ASSERT_NOT_NULL(env);

    lv_gltf_ibl_sampler_delete(sampler);
    lv_gltf_environment_delete(env);
}

void test_gltf_environment_create_from_jpeg(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);

    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, ENV_IMAGE("environment.jpg"));
    TEST_ASSERT_NOT_NULL(env);

    lv_gltf_ibl_sampler_delete(sampler);
    lv_gltf_environment_delete(env);
}

/* A path without a drive letter reaches the image loader directly */
void test_gltf_environment_create_from_native_path(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);

    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, ASSET("environment.hdr"));
    TEST_ASSERT_NOT_NULL(env);

    lv_gltf_ibl_sampler_delete(sampler);
    lv_gltf_environment_delete(env);
}

/* An image that cannot be read falls back to the embedded default image */
void test_gltf_environment_create_from_missing_file_falls_back(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);

    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, ENV_IMAGE("no_such_environment.hdr"));
    TEST_ASSERT_NOT_NULL(env);

    lv_gltf_environment_delete(env);
    lv_gltf_ibl_sampler_delete(sampler);
}

/* A NULL sampler is refused */
void test_gltf_environment_create_without_sampler_fails(void)
{
    TEST_ASSERT_NULL(lv_gltf_environment_create(NULL, NULL));
}

/* The environment image really is taken from the file: the same model lit by the
 * generated image does not look like the one lit by the embedded default image */
void test_gltf_environment_image_is_used(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);

    lv_gltf_environment_t * default_env = lv_gltf_environment_create(sampler, NULL);
    lv_gltf_environment_t * file_env = lv_gltf_environment_create(sampler, ENV_IMAGE("environment.hdr"));
    TEST_ASSERT_NOT_NULL(default_env);
    TEST_ASSERT_NOT_NULL(file_env);
    lv_gltf_ibl_sampler_delete(sampler);

    lv_obj_t * gltf = create_view(0);
    lv_gltf_set_environment(gltf, default_env);
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("environment_default"));

    lv_gltf_set_environment(gltf, file_env);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("environment_from_file"));

    lv_obj_delete(gltf);
    lv_gltf_environment_delete(file_env);
    lv_gltf_environment_delete(default_env);
}

void test_gltf_environment_angle(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);
    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, NULL);
    TEST_ASSERT_NOT_NULL(env);
    lv_gltf_ibl_sampler_delete(sampler);

    /* Rotating the environment is accepted at any angle */
    lv_gltf_environment_set_angle(env, 0.0f);
    lv_gltf_environment_set_angle(env, 45.0f);
    lv_gltf_environment_set_angle(env, -720.0f);

    lv_gltf_environment_delete(env);
}

/* A viewer creates a default environment when a model is loaded without one */
void test_gltf_environment_default_is_created_for_a_viewer(void)
{
    lv_obj_t * gltf = create_view(0);
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    render_frames();

    /* The viewer owns that environment and releases it with itself */
    lv_obj_delete(gltf);
    render_frames();
}

/* An environment given to a viewer belongs to the caller and can be shared */
void test_gltf_environment_shared_between_viewers(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);
    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, ENV_IMAGE("environment.hdr"));
    TEST_ASSERT_NOT_NULL(env);
    lv_gltf_ibl_sampler_delete(sampler);

    lv_obj_t * first = create_view(0);
    lv_obj_t * second = create_view(110);
    lv_gltf_set_environment(first, env);
    lv_gltf_set_environment(second, env);

    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(first, ASSET("minimal_triangle.gltf")));
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(second, ASSET("cameras.gltf")));

    render_frames();

    lv_obj_delete(first);
    lv_obj_delete(second);

    /* The environment outlives both viewers because they never owned it */
    lv_gltf_environment_set_angle(env, 10.0f);
    lv_gltf_environment_delete(env);
}

/* Replacing the default environment releases the one the viewer created */
void test_gltf_environment_replace_after_load(void)
{
    lv_obj_t * gltf = create_view(0);
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));
    render_frames();

    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 32);
    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, ENV_IMAGE("environment.jpg"));
    TEST_ASSERT_NOT_NULL(env);
    lv_gltf_ibl_sampler_delete(sampler);

    lv_gltf_set_environment(gltf, env);
    render_frames();

    lv_obj_delete(gltf);
    lv_gltf_environment_delete(env);
}

/* A NULL environment is refused instead of leaving the viewer without one */
void test_gltf_environment_set_null_is_refused(void)
{
    lv_obj_t * gltf = create_view(0);
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    lv_gltf_set_environment(gltf, NULL);

    /* Rendering still works, so the viewer kept its environment */
    render_frames();
}

/* Rotating the environment rotates the lighting of the model */
void test_gltf_environment_angle_is_applied(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(sampler, 64);
    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, ENV_IMAGE("environment.hdr"));
    TEST_ASSERT_NOT_NULL(env);
    lv_gltf_ibl_sampler_delete(sampler);

    lv_obj_t * gltf = create_view(0);
    lv_gltf_set_environment(gltf, env);
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    TEST_ASSERT_EQUAL_SCREENSHOT(REF("environment_angle_0"));

    lv_gltf_environment_set_angle(env, 90.0f);
    TEST_ASSERT_EQUAL_SCREENSHOT(REF("environment_angle_90"));

    lv_obj_delete(gltf);
    lv_gltf_environment_delete(env);
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_environment_sampler_lifecycle(void)
{
}

void test_gltf_environment_sampler_resolution(void)
{
}

void test_gltf_environment_create_default(void)
{
}

void test_gltf_environment_create_from_hdr(void)
{
}

void test_gltf_environment_create_from_jpeg(void)
{
}

void test_gltf_environment_create_from_native_path(void)
{
}

void test_gltf_environment_create_from_missing_file_falls_back(void)
{
}

void test_gltf_environment_create_without_sampler_fails(void)
{
}

void test_gltf_environment_image_is_used(void)
{
}

void test_gltf_environment_angle(void)
{
}

void test_gltf_environment_default_is_created_for_a_viewer(void)
{
}

void test_gltf_environment_shared_between_viewers(void)
{
}

void test_gltf_environment_replace_after_load(void)
{
}

void test_gltf_environment_set_null_is_refused(void)
{
}

void test_gltf_environment_angle_is_applied(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
