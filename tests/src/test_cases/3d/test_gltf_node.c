#if LV_BUILD_TEST
#include "../../lvgl.h"

#if LV_USE_GLTF

#include "unity/unity.h"

#define ASSET(name) "A:src/test_assets/gltf/generated/" name

/* The hierarchy asset holds
 *   /body                 at (1, 2, 3), numeric path ".0"
 *   /body/arm             at (0.5, 0, 0), scale 2, numeric path ".0.0"
 *   /body/arm/hand        at (0.25, 0, 0), numeric path ".0.0.0"
 *   /cursor               rotated 45 degrees around Z, numeric path ".1"
 *   /rot_x                rotated 30 degrees around X
 *   /rot_y                rotated 60 degrees around Y
 *   an unnamed child of /body, reachable as ".0.1" only
 */

typedef struct {
    uint32_t calls;
    lv_3dpoint_t local_position;
    lv_3dpoint_t world_position;
    lv_3dpoint_t scale;
    lv_3dpoint_t rotation;
    lv_result_t local_position_res;
    lv_result_t world_position_res;
    lv_result_t scale_res;
    lv_result_t rotation_res;
} node_report_t;

static node_report_t report;

void setUp(void)
{
    lv_memzero(&report, sizeof(report));
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void node_value_cb(lv_event_t * e)
{
    report.calls++;
    report.local_position_res = lv_gltf_model_node_get_local_position(e, &report.local_position);
    report.world_position_res = lv_gltf_model_node_get_world_position(e, &report.world_position);
    report.scale_res = lv_gltf_model_node_get_scale(e, &report.scale);
    report.rotation_res = lv_gltf_model_node_get_euler_rotation(e, &report.rotation);
}

/* Renders a frame, which applies the queued write operations and sends the value
 * changed events of the nodes. */
static void render_frames(void)
{
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
}

static lv_obj_t * create_view_with(const char * path, lv_gltf_model_t ** model_out)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf, 120, 120);
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf, path);
    TEST_ASSERT_NOT_NULL(model);
    if(model_out) {
        *model_out = model;
    }
    return gltf;
}

void test_gltf_node_get_by_index(void)
{
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("hierarchy.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(7, lv_gltf_model_get_node_count(model));

    for(size_t i = 0; i < 7; i++) {
        TEST_ASSERT_NOT_NULL(lv_gltf_model_node_get_by_index(model, i));
    }

    /* One past the last node and far beyond it are both out of range */
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_index(model, 7));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_index(model, 1000));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_index(NULL, 0));

    lv_gltf_model_delete(model);
}

void test_gltf_node_get_by_path(void)
{
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("hierarchy.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 0),
                          lv_gltf_model_node_get_by_path(model, "/body"));
    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 1),
                          lv_gltf_model_node_get_by_path(model, "/body/arm"));
    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 2),
                          lv_gltf_model_node_get_by_path(model, "/body/arm/hand"));
    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 3),
                          lv_gltf_model_node_get_by_path(model, "/cursor"));

    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_path(model, "/nope"));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_path(model, "/body/nope"));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_path(model, "body"));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_path(model, ""));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_path(NULL, "/body"));

    lv_gltf_model_delete(model);
}

void test_gltf_node_get_by_numeric_path(void)
{
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("hierarchy.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);

    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 0),
                          lv_gltf_model_node_get_by_numeric_path(model, ".0"));
    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 1),
                          lv_gltf_model_node_get_by_numeric_path(model, ".0.0"));
    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 2),
                          lv_gltf_model_node_get_by_numeric_path(model, ".0.0.0"));
    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 3),
                          lv_gltf_model_node_get_by_numeric_path(model, ".1"));

    /* The unnamed node has no useful name based path, only a numeric one */
    TEST_ASSERT_EQUAL_PTR(lv_gltf_model_node_get_by_index(model, 4),
                          lv_gltf_model_node_get_by_numeric_path(model, ".0.1"));

    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_numeric_path(model, ".9"));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_numeric_path(model, "0"));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_by_numeric_path(NULL, ".0"));

    lv_gltf_model_delete(model);
}

void test_gltf_node_paths_round_trip(void)
{
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("hierarchy.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);

    for(size_t i = 0; i < lv_gltf_model_get_node_count(model); i++) {
        lv_gltf_model_node_t * node = lv_gltf_model_node_get_by_index(model, i);
        const char * numeric_path = lv_gltf_model_node_get_ip(node);
        TEST_ASSERT_NOT_NULL(numeric_path);
        TEST_ASSERT_NOT_NULL(lv_gltf_model_node_get_path(node));

        /* The numeric path always leads back to the same node */
        TEST_ASSERT_EQUAL_PTR(node, lv_gltf_model_node_get_by_numeric_path(model, numeric_path));
    }

    TEST_ASSERT_NULL(lv_gltf_model_node_get_path(NULL));
    TEST_ASSERT_NULL(lv_gltf_model_node_get_ip(NULL));

    lv_gltf_model_delete(model);
}

/* Nodes that belong to a non default scene are still listed, they simply have no
 * name based path */
void test_gltf_node_nodes_outside_the_default_scene(void)
{
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(ASSET("multi_scene.gltf"), NULL);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(3, lv_gltf_model_get_node_count(model));

    /* With more than one scene the generated paths are prefixed with the scene */
    lv_gltf_model_node_t * in_scene = lv_gltf_model_node_get_by_index(model, 0);
    TEST_ASSERT_NOT_NULL(in_scene);
    TEST_ASSERT_EQUAL_STRING("scene_0/first", lv_gltf_model_node_get_path(in_scene));
    TEST_ASSERT_EQUAL_PTR(in_scene, lv_gltf_model_node_get_by_path(model, "scene_0/first"));

    for(size_t i = 1; i < 3; i++) {
        lv_gltf_model_node_t * other = lv_gltf_model_node_get_by_index(model, i);
        TEST_ASSERT_NOT_NULL(other);
        TEST_ASSERT_EQUAL_STRING("", lv_gltf_model_node_get_path(other));
    }

    lv_gltf_model_delete(model);
}

/* The properties of a node can only be read from the value changed callback */
void test_gltf_node_read_initial_values(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * body = lv_gltf_model_node_get_by_path(model, "/body");
    TEST_ASSERT_NOT_NULL(body);
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb(body, node_value_cb, LV_EVENT_VALUE_CHANGED, NULL));

    render_frames();

    TEST_ASSERT_GREATER_THAN(0, report.calls);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, report.local_position_res);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, report.local_position.x);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, report.local_position.y);
    TEST_ASSERT_EQUAL_FLOAT(3.0f, report.local_position.z);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, report.scale_res);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, report.scale.x);

    /* World position needs the other registration function */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, report.world_position_res);
}

void test_gltf_node_read_scale(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * arm = lv_gltf_model_node_get_by_path(model, "/body/arm");
    TEST_ASSERT_NOT_NULL(arm);
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb(arm, node_value_cb, LV_EVENT_VALUE_CHANGED, NULL));

    render_frames();

    TEST_ASSERT_GREATER_THAN(0, report.calls);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, report.scale_res);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, report.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, report.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, report.scale.z);
}

/* The Euler angles are reported as the rotations around X, Y and Z, in that order.
 * Each of these nodes is rotated around a single axis in the file. */
static void check_single_axis_rotation(const char * path, uint32_t axis, float expected)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * node = lv_gltf_model_node_get_by_path(model, path);
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb(node, node_value_cb, LV_EVENT_VALUE_CHANGED, NULL));

    render_frames();

    TEST_ASSERT_GREATER_THAN(0, report.calls);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, report.rotation_res);

    const float components[] = { report.rotation.x, report.rotation.y, report.rotation.z };
    for(uint32_t i = 0; i < 3; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.01f, i == axis ? expected : 0.0f, components[i]);
    }
}

void test_gltf_node_read_rotation_around_x(void)
{
    check_single_axis_rotation("/rot_x", 0, 0.5236f);
}

void test_gltf_node_read_rotation_around_y(void)
{
    check_single_axis_rotation("/rot_y", 1, 1.0472f);
}

void test_gltf_node_read_rotation_around_z(void)
{
    check_single_axis_rotation("/cursor", 2, 0.7854f);
}

/* The world position is the local position of the parents applied on top of each other */
void test_gltf_node_read_world_position(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * arm = lv_gltf_model_node_get_by_path(model, "/body/arm");
    TEST_ASSERT_NOT_NULL(arm);
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb_with_world_position(arm, node_value_cb,
                                                                             LV_EVENT_VALUE_CHANGED, NULL));

    render_frames();

    TEST_ASSERT_GREATER_THAN(0, report.calls);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, report.local_position_res);
    TEST_ASSERT_EQUAL_FLOAT(0.5f, report.local_position.x);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, report.world_position_res);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.5f, report.world_position.x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.0f, report.world_position.y);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.0f, report.world_position.z);
}

/* Reading a property outside of the callback must fail instead of handing out stale data */
void test_gltf_node_read_outside_callback_fails(void)
{
    lv_3dpoint_t point;

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_get_local_position(NULL, &point));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_get_world_position(NULL, &point));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_get_scale(NULL, &point));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_get_euler_rotation(NULL, &point));
}

void test_gltf_node_set_position(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * hand = lv_gltf_model_node_get_by_path(model, "/body/arm/hand");
    TEST_ASSERT_NOT_NULL(hand);
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb(hand, node_value_cb, LV_EVENT_VALUE_CHANGED, NULL));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_position_x(hand, -1.5f));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_position_y(hand, 2.5f));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_position_z(hand, 3.5f));

    render_frames();

    TEST_ASSERT_GREATER_THAN(0, report.calls);
    TEST_ASSERT_EQUAL_FLOAT(-1.5f, report.local_position.x);
    TEST_ASSERT_EQUAL_FLOAT(2.5f, report.local_position.y);
    TEST_ASSERT_EQUAL_FLOAT(3.5f, report.local_position.z);
}

void test_gltf_node_set_scale(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * cursor = lv_gltf_model_node_get_by_path(model, "/cursor");
    TEST_ASSERT_NOT_NULL(cursor);
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb(cursor, node_value_cb, LV_EVENT_VALUE_CHANGED, NULL));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_scale_x(cursor, 3.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_scale_y(cursor, 4.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_scale_z(cursor, 5.0f));

    render_frames();

    TEST_ASSERT_GREATER_THAN(0, report.calls);
    TEST_ASSERT_EQUAL_FLOAT(3.0f, report.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(4.0f, report.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(5.0f, report.scale.z);
}

/* Each rotation setter turns the node around its axis and is read back through the
 * matching component of the getter */
void test_gltf_node_set_rotation(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * body = lv_gltf_model_node_get_by_path(model, "/body");
    TEST_ASSERT_NOT_NULL(body);
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb(body, node_value_cb, LV_EVENT_VALUE_CHANGED, NULL));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_rotation_x(body, 0.5f));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_rotation_y(body, 0.25f));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_rotation_z(body, -0.75f));

    render_frames();

    TEST_ASSERT_GREATER_THAN(0, report.calls);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, report.rotation_res);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.5f, report.rotation.x);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.25f, report.rotation.y);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, -0.75f, report.rotation.z);
}

/* Moving a parent moves the world position of its children */
void test_gltf_node_parent_transform_affects_child(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * body = lv_gltf_model_node_get_by_path(model, "/body");
    lv_gltf_model_node_t * hand = lv_gltf_model_node_get_by_path(model, "/body/arm/hand");
    TEST_ASSERT_NOT_NULL(body);
    TEST_ASSERT_NOT_NULL(hand);
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb_with_world_position(hand, node_value_cb,
                                                                             LV_EVENT_VALUE_CHANGED, NULL));

    render_frames();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, report.world_position_res);
    float world_x_before = report.world_position.x;

    lv_memzero(&report, sizeof(report));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gltf_model_node_set_position_x(body, 11.0f));
    render_frames();

    TEST_ASSERT_GREATER_THAN(0, report.calls);
    /* The local position of the hand is unchanged, its world position moved with the body */
    TEST_ASSERT_EQUAL_FLOAT(0.25f, report.local_position.x);
    TEST_ASSERT_NOT_EQUAL(world_x_before, report.world_position.x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 11.0f + 0.5f + 0.25f * 2.0f, report.world_position.x);
}

void test_gltf_node_setters_reject_null(void)
{
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_position_x(NULL, 1.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_position_y(NULL, 1.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_position_z(NULL, 1.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_rotation_x(NULL, 1.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_rotation_y(NULL, 1.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_rotation_z(NULL, 1.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_scale_x(NULL, 1.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_scale_y(NULL, 1.0f));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gltf_model_node_set_scale_z(NULL, 1.0f));

    TEST_ASSERT_NULL(lv_gltf_model_node_add_event_cb(NULL, node_value_cb, LV_EVENT_VALUE_CHANGED, NULL));
    TEST_ASSERT_NULL(lv_gltf_model_node_add_event_cb_with_world_position(NULL, node_value_cb,
                                                                         LV_EVENT_VALUE_CHANGED, NULL));
}

/* The callback is only called when a property actually changed */
void test_gltf_node_event_only_on_change(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    lv_gltf_model_node_t * body = lv_gltf_model_node_get_by_path(model, "/body");
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb(body, node_value_cb, LV_EVENT_VALUE_CHANGED, NULL));

    render_frames();
    TEST_ASSERT_GREATER_THAN(0, report.calls);

    uint32_t calls_after_first_render = report.calls;
    render_frames();
    TEST_ASSERT_EQUAL(calls_after_first_render, report.calls);

    lv_gltf_model_node_set_position_y(body, 7.0f);
    render_frames();
    TEST_ASSERT_GREATER_THAN(calls_after_first_render, report.calls);
    TEST_ASSERT_EQUAL_FLOAT(7.0f, report.local_position.y);
}

/* User data is handed back to the callback */
static void user_data_cb(lv_event_t * e)
{
    uint32_t * counter = lv_event_get_user_data(e);
    (*counter)++;
}

void test_gltf_node_event_user_data(void)
{
    lv_gltf_model_t * model = NULL;
    create_view_with(ASSET("hierarchy.gltf"), &model);

    uint32_t counter = 0;
    lv_gltf_model_node_t * body = lv_gltf_model_node_get_by_path(model, "/body");
    TEST_ASSERT_NOT_NULL(lv_gltf_model_node_add_event_cb(body, user_data_cb, LV_EVENT_VALUE_CHANGED, &counter));

    render_frames();
    TEST_ASSERT_GREATER_THAN(0, counter);
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_node_get_by_index(void)
{
}

void test_gltf_node_get_by_path(void)
{
}

void test_gltf_node_get_by_numeric_path(void)
{
}

void test_gltf_node_paths_round_trip(void)
{
}

void test_gltf_node_nodes_outside_the_default_scene(void)
{
}

void test_gltf_node_read_initial_values(void)
{
}

void test_gltf_node_read_scale(void)
{
}

void test_gltf_node_read_rotation_around_x(void)
{
}

void test_gltf_node_read_rotation_around_y(void)
{
}

void test_gltf_node_read_rotation_around_z(void)
{
}

void test_gltf_node_read_world_position(void)
{
}

void test_gltf_node_read_outside_callback_fails(void)
{
}

void test_gltf_node_set_position(void)
{
}

void test_gltf_node_set_scale(void)
{
}

void test_gltf_node_set_rotation(void)
{
}

void test_gltf_node_parent_transform_affects_child(void)
{
}

void test_gltf_node_setters_reject_null(void)
{
}

void test_gltf_node_event_only_on_change(void)
{
}

void test_gltf_node_event_user_data(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
