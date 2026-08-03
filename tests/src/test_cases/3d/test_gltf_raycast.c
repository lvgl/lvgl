#if LV_BUILD_TEST
#include "../../lvgl.h"

#if LV_USE_GLTF

#include "unity/unity.h"

#define ASSET(name) "A:src/test_assets/gltf/generated/" name

#define VIEW_SIZE 200

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static lv_obj_t * create_loaded_view(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf, VIEW_SIZE, VIEW_SIZE);
    lv_obj_set_pos(gltf, 0, 0);
    TEST_ASSERT_NOT_NULL(lv_gltf_load_model_from_file(gltf, ASSET("cameras.gltf")));

    /* The camera matrices are set up while rendering, which the ray functions use */
    for(int i = 0; i < 2; i++) {
        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }
    return gltf;
}

void test_gltf_raycast_ground_plane(void)
{
    lv_3dplane_t plane = lv_get_ground_plane(0.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, plane.origin.x);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, plane.origin.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, plane.origin.z);
    /* It faces up */
    TEST_ASSERT_EQUAL_FLOAT(0.0f, plane.direction.x);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, plane.direction.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, plane.direction.z);

    plane = lv_get_ground_plane(2.5f);
    TEST_ASSERT_EQUAL_FLOAT(2.5f, plane.origin.y);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, plane.direction.y);
}

void test_gltf_raycast_ray_hits_plane(void)
{
    lv_3dplane_t ground = lv_get_ground_plane(0.0f);

    /* Straight down from above the origin */
    lv_3dray_t ray;
    ray.origin.x = 1.0f;
    ray.origin.y = 5.0f;
    ray.origin.z = -2.0f;
    ray.direction.x = 0.0f;
    ray.direction.y = -1.0f;
    ray.direction.z = 0.0f;

    lv_3dpoint_t hit;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_intersect_ray_with_plane(&ray, &ground, &hit));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, hit.x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, hit.y);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -2.0f, hit.z);
}

void test_gltf_raycast_ray_hits_raised_plane(void)
{
    lv_3dplane_t plane = lv_get_ground_plane(3.0f);

    lv_3dray_t ray;
    ray.origin.x = 0.0f;
    ray.origin.y = 10.0f;
    ray.origin.z = 0.0f;
    ray.direction.x = 0.0f;
    ray.direction.y = -1.0f;
    ray.direction.z = 0.0f;

    lv_3dpoint_t hit;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_intersect_ray_with_plane(&ray, &plane, &hit));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.0f, hit.y);
}

/* A ray running along the plane never meets it */
void test_gltf_raycast_parallel_ray_misses(void)
{
    lv_3dplane_t ground = lv_get_ground_plane(0.0f);

    lv_3dray_t ray;
    ray.origin.x = 0.0f;
    ray.origin.y = 4.0f;
    ray.origin.z = 0.0f;
    ray.direction.x = 1.0f;
    ray.direction.y = 0.0f;
    ray.direction.z = 0.0f;

    lv_3dpoint_t hit;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_intersect_ray_with_plane(&ray, &ground, &hit));
}

/* An angled ray meets the plane where the direction takes it */
void test_gltf_raycast_angled_ray(void)
{
    lv_3dplane_t ground = lv_get_ground_plane(0.0f);

    lv_3dray_t ray;
    ray.origin.x = 0.0f;
    ray.origin.y = 2.0f;
    ray.origin.z = 0.0f;
    ray.direction.x = 1.0f;
    ray.direction.y = -1.0f;
    ray.direction.z = 0.0f;

    lv_3dpoint_t hit;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_intersect_ray_with_plane(&ray, &ground, &hit));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.0f, hit.x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, hit.y);
}

void test_gltf_raycast_view_plane_faces_the_camera(void)
{
    lv_obj_t * gltf = create_loaded_view();

    lv_3dplane_t plane = lv_gltf_get_current_view_plane(gltf, 5.0f);

    /* The plane normal points somewhere, a zero vector would mean no valid camera */
    float length = plane.direction.x * plane.direction.x +
                   plane.direction.y * plane.direction.y +
                   plane.direction.z * plane.direction.z;
    TEST_ASSERT_GREATER_THAN(0.0f, length);

    /* Moving the plane further away moves its center */
    lv_3dplane_t far_plane = lv_gltf_get_current_view_plane(gltf, 20.0f);
    bool moved = far_plane.origin.x != plane.origin.x ||
                 far_plane.origin.y != plane.origin.y ||
                 far_plane.origin.z != plane.origin.z;
    TEST_ASSERT_TRUE(moved);
}

void test_gltf_raycast_ray_from_screen_position(void)
{
    lv_obj_t * gltf = create_loaded_view();

    lv_point_t center = { VIEW_SIZE / 2, VIEW_SIZE / 2 };
    lv_3dray_t ray = lv_gltf_get_ray_from_2d_coordinate(gltf, &center);

    float length = ray.direction.x * ray.direction.x +
                   ray.direction.y * ray.direction.y +
                   ray.direction.z * ray.direction.z;
    TEST_ASSERT_GREATER_THAN(0.0f, length);

    /* A ray through another pixel points somewhere else */
    lv_point_t corner = { 10, 10 };
    lv_3dray_t corner_ray = lv_gltf_get_ray_from_2d_coordinate(gltf, &corner);
    bool different = corner_ray.direction.x != ray.direction.x ||
                     corner_ray.direction.y != ray.direction.y ||
                     corner_ray.direction.z != ray.direction.z;
    TEST_ASSERT_TRUE(different);
}

/* The ray of a screen position meets the view plane at the point that maps back to
 * that same screen position */
void test_gltf_raycast_screen_to_world_round_trip(void)
{
    lv_obj_t * gltf = create_loaded_view();

    lv_3dplane_t plane = lv_gltf_get_current_view_plane(gltf, lv_gltf_get_world_distance(gltf));

    lv_point_t screen_pos = { VIEW_SIZE / 2 + 20, VIEW_SIZE / 2 - 15 };
    lv_3dray_t ray = lv_gltf_get_ray_from_2d_coordinate(gltf, &screen_pos);

    lv_3dpoint_t world;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_intersect_ray_with_plane(&ray, &plane, &world));

    lv_point_t back;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_world_to_screen(gltf, world, &back));

    TEST_ASSERT_INT_WITHIN(2, screen_pos.x, back.x);
    TEST_ASSERT_INT_WITHIN(2, screen_pos.y, back.y);
}

void test_gltf_raycast_world_to_screen_of_the_focal_point(void)
{
    lv_obj_t * gltf = create_loaded_view();

    /* The focal point is what the camera looks at, so it lands in the middle of the view */
    lv_3dpoint_t focal;
    focal.x = lv_gltf_get_focal_x(gltf);
    focal.y = lv_gltf_get_focal_y(gltf);
    focal.z = lv_gltf_get_focal_z(gltf);

    lv_point_t screen_pos;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_world_to_screen(gltf, focal, &screen_pos));

    TEST_ASSERT_INT_WITHIN(3, VIEW_SIZE / 2, screen_pos.x);
    TEST_ASSERT_INT_WITHIN(3, VIEW_SIZE / 2, screen_pos.y);
}

/* A point behind the camera has no screen position */
void test_gltf_raycast_world_to_screen_behind_the_camera(void)
{
    lv_obj_t * gltf = create_loaded_view();

    /* The plane sits one camera distance in front of the camera and its normal points
     * away from it, so stepping backwards along the normal walks past the camera */
    float distance = lv_gltf_get_world_distance(gltf);
    lv_3dplane_t plane = lv_gltf_get_current_view_plane(gltf, distance);

    lv_3dpoint_t behind;
    behind.x = plane.origin.x - plane.direction.x * distance * 2.0f;
    behind.y = plane.origin.y - plane.direction.y * distance * 2.0f;
    behind.z = plane.origin.z - plane.direction.z * distance * 2.0f;

    lv_point_t screen_pos;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_world_to_screen(gltf, behind, &screen_pos));
}

/* A point far away from what the camera looks at lands outside the view */
void test_gltf_raycast_world_to_screen_outside_the_view(void)
{
    lv_obj_t * gltf = create_loaded_view();

    lv_3dpoint_t focal;
    focal.x = lv_gltf_get_focal_x(gltf);
    focal.y = lv_gltf_get_focal_y(gltf);
    focal.z = lv_gltf_get_focal_z(gltf);

    /* Whichever way the camera is turned, moving far along one of the axes has to
     * leave the view */
    bool left_the_view = false;
    for(uint32_t axis = 0; axis < 3; axis++) {
        lv_3dpoint_t far_away = focal;
        float * component = (axis == 0) ? &far_away.x : (axis == 1) ? &far_away.y : &far_away.z;
        *component += 1000.0f;

        lv_point_t screen_pos;
        if(lv_gltf_world_to_screen(gltf, far_away, &screen_pos) != LV_RESULT_OK) {
            left_the_view = true;
            continue;
        }
        if(screen_pos.x < 0 || screen_pos.x >= VIEW_SIZE || screen_pos.y < 0 || screen_pos.y >= VIEW_SIZE) {
            left_the_view = true;
        }
    }

    TEST_ASSERT_TRUE(left_the_view);
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_raycast_ground_plane(void)
{
}

void test_gltf_raycast_ray_hits_plane(void)
{
}

void test_gltf_raycast_ray_hits_raised_plane(void)
{
}

void test_gltf_raycast_parallel_ray_misses(void)
{
}

void test_gltf_raycast_angled_ray(void)
{
}

void test_gltf_raycast_view_plane_faces_the_camera(void)
{
}

void test_gltf_raycast_ray_from_screen_position(void)
{
}

void test_gltf_raycast_screen_to_world_round_trip(void)
{
}

void test_gltf_raycast_world_to_screen_of_the_focal_point(void)
{
}

void test_gltf_raycast_world_to_screen_behind_the_camera(void)
{
}

void test_gltf_raycast_world_to_screen_outside_the_view(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
