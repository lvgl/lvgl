#if LV_BUILD_TEST
#include "../../lvgl.h"

#if LV_USE_GLTF

#include "unity/unity.h"

#define ASSET(name) "A:src/test_assets/gltf/generated/" name

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static lv_obj_t * create_view(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    TEST_ASSERT_NOT_NULL(gltf);
    lv_obj_set_size(gltf, 120, 120);
    return gltf;
}

void test_gltf_viewer_create_is_empty(void)
{
    lv_obj_t * gltf = create_view();

    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_NULL(lv_gltf_get_primary_model(gltf));
    TEST_ASSERT_NULL(lv_gltf_get_model_by_index(gltf, 0));

    /* A glTF view is an lv_obj, so the usual functions work on it */
    TEST_ASSERT_EQUAL_PTR(lv_screen_active(), lv_obj_get_parent(gltf));
    lv_obj_update_layout(gltf);
    TEST_ASSERT_EQUAL(120, lv_obj_get_width(gltf));
}

void test_gltf_viewer_load_model_from_file(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf"));
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_EQUAL_PTR(model, lv_gltf_get_primary_model(gltf));
    TEST_ASSERT_EQUAL_PTR(model, lv_gltf_get_model_by_index(gltf, 0));
    TEST_ASSERT_NULL(lv_gltf_get_model_by_index(gltf, 1));
}

void test_gltf_viewer_load_missing_model_keeps_viewer_usable(void)
{
    lv_obj_t * gltf = create_view();

    TEST_ASSERT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("no_such_model.glb")));
    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(gltf));

    /* The failed load does not prevent a later one */
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));
    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(gltf));
}

void test_gltf_viewer_multiple_models(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * first = lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf"));
    lv_gltf_model_t * second = lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf"));
    lv_gltf_model_t * third = lv_gltf_load_model_from_file(gltf, ASSET("materials.gltf"));
    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    TEST_ASSERT_NOT_NULL(third);

    TEST_ASSERT_EQUAL(3, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_EQUAL_PTR(first, lv_gltf_get_model_by_index(gltf, 0));
    TEST_ASSERT_EQUAL_PTR(second, lv_gltf_get_model_by_index(gltf, 1));
    TEST_ASSERT_EQUAL_PTR(third, lv_gltf_get_model_by_index(gltf, 2));
    TEST_ASSERT_NULL(lv_gltf_get_model_by_index(gltf, 3));

    /* The primary model stays the first one that was added */
    TEST_ASSERT_EQUAL_PTR(first, lv_gltf_get_primary_model(gltf));
}

void test_gltf_viewer_add_model_keeps_ownership_with_the_caller(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("minimal_triangle.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(gltf, model));
    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_EQUAL_PTR(model, lv_gltf_get_primary_model(gltf));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_obj_delete(gltf);

    /* The model outlived the viewer, so it is still valid here */
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_mesh_count(model));
    lv_gltf_model_delete(model);
}

/* The same model can be shown by two viewers at once */
void test_gltf_viewer_share_a_model(void)
{
    lv_obj_t * first = create_view();
    lv_obj_t * second = create_view();
    lv_obj_set_pos(second, 130, 0);

    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("cameras.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(first, model));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(second, model));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_obj_delete(first);
    lv_obj_delete(second);
    lv_gltf_model_delete(model);
}

void test_gltf_viewer_remove_model(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * first = lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf"));
    lv_gltf_model_t * second = lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf"));
    lv_gltf_model_t * third = lv_gltf_load_model_from_file(gltf, ASSET("materials.gltf"));
    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    TEST_ASSERT_NOT_NULL(third);

    lv_gltf_remove_model(gltf, second);

    TEST_ASSERT_EQUAL(2, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_EQUAL_PTR(first, lv_gltf_get_model_by_index(gltf, 0));
    TEST_ASSERT_EQUAL_PTR(third, lv_gltf_get_model_by_index(gltf, 1));
    TEST_ASSERT_NULL(lv_gltf_get_model_by_index(gltf, 2));
    TEST_ASSERT_EQUAL_PTR(first, lv_gltf_get_primary_model(gltf));
}

void test_gltf_viewer_remove_the_primary_model(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * first = lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf"));
    lv_gltf_model_t * second = lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf"));
    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);

    /* Read the center of the second model through the focal point, then move away from it */
    lv_gltf_recenter(gltf, second);
    const float center_x = lv_gltf_get_focal_x(gltf);
    const float center_y = lv_gltf_get_focal_y(gltf);
    lv_gltf_set_focal_x(gltf, 10.0f);
    lv_gltf_set_focal_y(gltf, 10.0f);

    lv_gltf_remove_model(gltf, first);

    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_EQUAL_PTR(second, lv_gltf_get_primary_model(gltf));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, center_x, lv_gltf_get_focal_x(gltf));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, center_y, lv_gltf_get_focal_y(gltf));
}

void test_gltf_viewer_remove_model_keeps_a_borrowed_model_alive(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("minimal_triangle.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(gltf, model));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_gltf_remove_model(gltf, model);
    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(gltf));

    /* The model outlived its stay in the viewer, so it is still valid here */
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_mesh_count(model));
    lv_gltf_model_delete(model);
}

void test_gltf_viewer_remove_all_models(void)
{
    lv_obj_t * gltf = create_view();

    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("materials.gltf")));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_gltf_remove_all_models(gltf);

    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_NULL(lv_gltf_get_primary_model(gltf));
    TEST_ASSERT_EQUAL(0, lv_gltf_get_camera_count(gltf));

    /* An emptied viewer takes models again */
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));
    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(gltf));
}

void test_gltf_viewer_remove_all_models_holding_one_model_twice(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf"));
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(gltf, model));
    TEST_ASSERT_EQUAL(2, lv_gltf_get_model_count(gltf));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_gltf_remove_all_models(gltf);

    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(gltf));

    /* The viewer renders on without the models it lost */
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
}

/* The same duplicated entry, this time released by the viewer going away */
void test_gltf_viewer_delete_view_holding_one_model_twice(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf"));
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(gltf, model));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_obj_delete(gltf);
}

void test_gltf_viewer_delete_a_model_in_use(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("minimal_triangle.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(gltf, model));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_gltf_model_delete(model);

    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(gltf));
    TEST_ASSERT_NULL(lv_gltf_get_primary_model(gltf));

    /* The viewer renders on without the model it lost */
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
}

void test_gltf_viewer_delete_a_shared_model(void)
{
    lv_obj_t * first = create_view();
    lv_obj_t * second = create_view();
    lv_obj_set_pos(second, 130, 0);

    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("cameras.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(first, model));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_add_model(second, model));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_gltf_model_delete(model);

    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(first));
    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(second));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
}

void test_gltf_viewer_remove_model_tolerates_unknown_models(void)
{
    lv_obj_t * first = create_view();
    lv_obj_t * second = create_view();

    lv_gltf_model_t * mine = lv_gltf_load_model_from_file(first, ASSET("minimal_triangle.gltf"));
    lv_gltf_model_t * other = lv_gltf_load_model_from_file(second, ASSET("cameras.gltf"));
    TEST_ASSERT_NOT_NULL(mine);
    TEST_ASSERT_NOT_NULL(other);

    lv_gltf_remove_model(first, other);
    lv_gltf_remove_model(first, NULL);

    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(first));
    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(second));

    lv_gltf_remove_all_models(first);
    /* Removing all models twice in a row is harmless */
    lv_gltf_remove_all_models(first);
    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(first));
    TEST_ASSERT_EQUAL(1, lv_gltf_get_model_count(second));
}

void test_gltf_viewer_add_model_rejects_null(void)
{
    lv_obj_t * gltf = create_view();

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_add_model(gltf, NULL));
    TEST_ASSERT_EQUAL(0, lv_gltf_get_model_count(gltf));
}

void test_gltf_viewer_camera_selection(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf")));

    /* The asset holds a perspective and an orthographic camera */
    TEST_ASSERT_EQUAL(2, lv_gltf_get_camera_count(gltf));

    /* Index 0 is the viewer's own camera, 1 and 2 are the ones from the file */
    TEST_ASSERT_EQUAL(LV_GLTF_DEFAULT_CAMERA, lv_gltf_get_camera(gltf));

    for(uint32_t i = 0; i <= 2; i++) {
        lv_gltf_set_camera(gltf, i);
        TEST_ASSERT_EQUAL(i, lv_gltf_get_camera(gltf));
    }

    /* An index beyond the last camera is clamped to the last one */
    lv_gltf_set_camera(gltf, 3);
    TEST_ASSERT_EQUAL(2, lv_gltf_get_camera(gltf));
    lv_gltf_set_camera(gltf, 1000);
    TEST_ASSERT_EQUAL(2, lv_gltf_get_camera(gltf));

    lv_gltf_set_camera(gltf, LV_GLTF_DEFAULT_CAMERA);
    TEST_ASSERT_EQUAL(LV_GLTF_DEFAULT_CAMERA, lv_gltf_get_camera(gltf));
}

void test_gltf_viewer_camera_count_without_cameras(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf")));

    TEST_ASSERT_EQUAL(0, lv_gltf_get_camera_count(gltf));

    /* Without cameras in the file everything clamps to the viewer's own camera */
    lv_gltf_set_camera(gltf, 1);
    TEST_ASSERT_EQUAL(LV_GLTF_DEFAULT_CAMERA, lv_gltf_get_camera(gltf));
}

/* The camera of a viewer without models cannot be changed */
void test_gltf_viewer_camera_without_model(void)
{
    lv_obj_t * gltf = create_view();

    TEST_ASSERT_EQUAL(0, lv_gltf_get_camera_count(gltf));
    lv_gltf_set_camera(gltf, 1);
    TEST_ASSERT_EQUAL(LV_GLTF_DEFAULT_CAMERA, lv_gltf_get_camera(gltf));
}

void test_gltf_viewer_camera_orientation(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_set_yaw(gltf, 45.0f);
    TEST_ASSERT_EQUAL_FLOAT(45.0f, lv_gltf_get_yaw(gltf));
    lv_gltf_set_yaw(gltf, -390.5f);
    TEST_ASSERT_EQUAL_FLOAT(-390.5f, lv_gltf_get_yaw(gltf));

    lv_gltf_set_pitch(gltf, -30.0f);
    TEST_ASSERT_EQUAL_FLOAT(-30.0f, lv_gltf_get_pitch(gltf));

    lv_gltf_set_distance(gltf, 7.5f);
    TEST_ASSERT_EQUAL_FLOAT(7.5f, lv_gltf_get_distance(gltf));
}

void test_gltf_viewer_field_of_view(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_set_fov(gltf, 60.0f);
    TEST_ASSERT_EQUAL_FLOAT(60.0f, lv_gltf_get_fov(gltf));

    /* A zero field of view selects the orthographic projection */
    lv_gltf_set_fov(gltf, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, lv_gltf_get_fov(gltf));
}

void test_gltf_viewer_focal_point(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_set_focal_x(gltf, 1.5f);
    lv_gltf_set_focal_y(gltf, -2.5f);
    lv_gltf_set_focal_z(gltf, 3.5f);

    TEST_ASSERT_EQUAL_FLOAT(1.5f, lv_gltf_get_focal_x(gltf));
    TEST_ASSERT_EQUAL_FLOAT(-2.5f, lv_gltf_get_focal_y(gltf));
    TEST_ASSERT_EQUAL_FLOAT(3.5f, lv_gltf_get_focal_z(gltf));
}

/* Recentering points the camera at the middle of the model */
void test_gltf_viewer_recenter(void)
{
    lv_obj_t * gltf = create_view();
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, ASSET("minimal_triangle.gltf"));
    TEST_ASSERT_NOT_NULL(model);

    /* The triangle spans x 0..1 and y 0..1 around z 0 */
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, lv_gltf_get_focal_x(gltf));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, lv_gltf_get_focal_y(gltf));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, lv_gltf_get_focal_z(gltf));

    lv_gltf_set_focal_x(gltf, 10.0f);
    lv_gltf_set_focal_y(gltf, 10.0f);
    lv_gltf_set_focal_z(gltf, 10.0f);

    /* Passing NULL recenters on the first model */
    lv_gltf_recenter(gltf, NULL);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, lv_gltf_get_focal_x(gltf));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, lv_gltf_get_focal_y(gltf));

    lv_gltf_set_focal_x(gltf, 10.0f);
    lv_gltf_recenter(gltf, model);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, lv_gltf_get_focal_x(gltf));
}

/* The world distance is the camera distance in world units, so it follows the
 * distance scale */
void test_gltf_viewer_world_distance(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf")));

    lv_gltf_set_distance(gltf, 2.0f);
    float near_distance = lv_gltf_get_world_distance(gltf);
    TEST_ASSERT_GREATER_THAN(0.0f, near_distance);

    lv_gltf_set_distance(gltf, 4.0f);
    TEST_ASSERT_GREATER_THAN(near_distance, lv_gltf_get_world_distance(gltf));
}

void test_gltf_viewer_background_mode(void)
{
    lv_obj_t * gltf = create_view();

    /* The environment background is the default */
    TEST_ASSERT_EQUAL(LV_GLTF_BG_MODE_ENVIRONMENT, lv_gltf_get_background_mode(gltf));

    lv_gltf_set_background_mode(gltf, LV_GLTF_BG_MODE_SOLID);
    TEST_ASSERT_EQUAL(LV_GLTF_BG_MODE_SOLID, lv_gltf_get_background_mode(gltf));

    lv_gltf_set_background_mode(gltf, LV_GLTF_BG_MODE_ENVIRONMENT);
    TEST_ASSERT_EQUAL(LV_GLTF_BG_MODE_ENVIRONMENT, lv_gltf_get_background_mode(gltf));
}

void test_gltf_viewer_background_blur(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_set_background_blur(gltf, 0);
    TEST_ASSERT_EQUAL(0, lv_gltf_get_background_blur(gltf));

    lv_gltf_set_background_blur(gltf, 75);
    TEST_ASSERT_EQUAL(75, lv_gltf_get_background_blur(gltf));

    /* Anything above the documented maximum is clamped */
    lv_gltf_set_background_blur(gltf, 250);
    TEST_ASSERT_EQUAL(100, lv_gltf_get_background_blur(gltf));
}

void test_gltf_viewer_env_brightness_and_exposure(void)
{
    lv_obj_t * gltf = create_view();

    lv_gltf_set_environment_brightness(gltf, 2.5f);
    TEST_ASSERT_EQUAL_FLOAT(2.5f, lv_gltf_get_environment_brightness(gltf));
    lv_gltf_set_environment_brightness(gltf, 0.f);
    TEST_ASSERT_EQUAL_FLOAT(0.f, lv_gltf_get_environment_brightness(gltf));

    lv_gltf_set_image_exposure(gltf, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, lv_gltf_get_image_exposure(gltf));
    lv_gltf_set_image_exposure(gltf, 2.75f);
    TEST_ASSERT_EQUAL_FLOAT(2.75f, lv_gltf_get_image_exposure(gltf));
}

void test_gltf_viewer_antialiasing_mode(void)
{
    lv_obj_t * gltf = create_view();

    TEST_ASSERT_EQUAL(LV_GLTF_AA_MODE_OFF, lv_gltf_get_antialiasing_mode(gltf));

    const lv_gltf_aa_mode_t modes[] = {
        LV_GLTF_AA_MODE_ON, LV_GLTF_AA_MODE_DYNAMIC, LV_GLTF_AA_MODE_OFF
    };
    for(uint32_t i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        lv_gltf_set_antialiasing_mode(gltf, modes[i]);
        TEST_ASSERT_EQUAL(modes[i], lv_gltf_get_antialiasing_mode(gltf));
    }
}

/* Every viewer setting keeps its value while a model is loaded and rendered */
void test_gltf_viewer_settings_survive_a_render(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf")));

    lv_gltf_set_yaw(gltf, 33.0f);
    lv_gltf_set_pitch(gltf, -12.0f);
    lv_gltf_set_distance(gltf, 3.25f);
    lv_gltf_set_fov(gltf, 55.0f);
    lv_gltf_set_focal_x(gltf, 0.5f);
    lv_gltf_set_background_blur(gltf, 40);
    lv_gltf_set_environment_brightness(gltf, 1.8f);
    lv_gltf_set_image_exposure(gltf, 1.4f);
    lv_gltf_set_antialiasing_mode(gltf, LV_GLTF_AA_MODE_DYNAMIC);
    lv_gltf_set_camera(gltf, 1);

    for(int i = 0; i < 2; i++) {
        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_FLOAT(33.0f, lv_gltf_get_yaw(gltf));
    TEST_ASSERT_EQUAL_FLOAT(-12.0f, lv_gltf_get_pitch(gltf));
    TEST_ASSERT_EQUAL_FLOAT(3.25f, lv_gltf_get_distance(gltf));
    TEST_ASSERT_EQUAL_FLOAT(55.0f, lv_gltf_get_fov(gltf));
    TEST_ASSERT_EQUAL_FLOAT(0.5f, lv_gltf_get_focal_x(gltf));
    TEST_ASSERT_EQUAL(40, lv_gltf_get_background_blur(gltf));
    TEST_ASSERT_EQUAL_FLOAT(1.8f, lv_gltf_get_environment_brightness(gltf));
    TEST_ASSERT_EQUAL_FLOAT(1.4f, lv_gltf_get_image_exposure(gltf));
    TEST_ASSERT_EQUAL(LV_GLTF_AA_MODE_DYNAMIC, lv_gltf_get_antialiasing_mode(gltf));
    TEST_ASSERT_EQUAL(1, lv_gltf_get_camera(gltf));
}

/* Deleting the viewer deletes the models it owns */
void test_gltf_viewer_delete_with_owned_models(void)
{
    lv_obj_t * gltf = create_view();
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("pbr_textures.gltf")));
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("skin.gltf")));

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_obj_delete(gltf);
    /* Reaching this point without a leak or a crash is the assertion */
    lv_refr_now(NULL);
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_viewer_create_is_empty(void)
{
}

void test_gltf_viewer_load_model_from_file(void)
{
}

void test_gltf_viewer_load_missing_model_keeps_viewer_usable(void)
{
}

void test_gltf_viewer_multiple_models(void)
{
}

void test_gltf_viewer_add_model_keeps_ownership_with_the_caller(void)
{
}

void test_gltf_viewer_share_a_model(void)
{
}

void test_gltf_viewer_remove_model(void)
{
}

void test_gltf_viewer_remove_the_primary_model(void)
{
}

void test_gltf_viewer_remove_model_keeps_a_borrowed_model_alive(void)
{
}

void test_gltf_viewer_remove_all_models(void)
{
}

void test_gltf_viewer_remove_model_tolerates_unknown_models(void)
{
}

void test_gltf_viewer_remove_all_models_holding_one_model_twice(void)
{
}

void test_gltf_viewer_delete_view_holding_one_model_twice(void)
{
}

void test_gltf_viewer_delete_a_model_in_use(void)
{
}

void test_gltf_viewer_delete_a_shared_model(void)
{
}

void test_gltf_viewer_add_model_rejects_null(void)
{
}

void test_gltf_viewer_camera_selection(void)
{
}

void test_gltf_viewer_camera_count_without_cameras(void)
{
}

void test_gltf_viewer_camera_without_model(void)
{
}

void test_gltf_viewer_camera_orientation(void)
{
}

void test_gltf_viewer_field_of_view(void)
{
}

void test_gltf_viewer_focal_point(void)
{
}

void test_gltf_viewer_recenter(void)
{
}

void test_gltf_viewer_world_distance(void)
{
}

void test_gltf_viewer_background_mode(void)
{
}

void test_gltf_viewer_background_blur(void)
{
}

void test_gltf_viewer_env_brightness_and_exposure(void)
{
}

void test_gltf_viewer_antialiasing_mode(void)
{
}

void test_gltf_viewer_settings_survive_a_render(void)
{
}

void test_gltf_viewer_delete_with_owned_models(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
