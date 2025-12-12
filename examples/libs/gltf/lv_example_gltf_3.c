#include "lv_example_gltf.h"

#if LV_BUILD_EXAMPLES

#if LV_USE_GLTF

#define PLANT_COUNT 5

typedef struct {
    lv_gltf_model_node_t * root;
    lv_3dpoint_t position;
    lv_3dpoint_t rotation;
} plant_t;

struct {
    plant_t * plant;
    lv_3dpoint_t offset_point;
} selected_plant;

static lv_gltf_model_node_t * cursor_node;
static plant_t plant_positions[PLANT_COUNT];

static void hide_node(lv_gltf_model_node_t * cursor);
static void move_plant(plant_t * plant, const lv_3dpoint_t * point);
static void rotate_plant(plant_t * plant);

static plant_t * get_plant(const lv_3dpoint_t * point, lv_3dpoint_t * offset_distance);

static void clicked_event_cb(lv_event_t * e)
{
    lv_obj_t * gltf = lv_event_get_target_obj(e);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_RELEASED) {
        hide_node(cursor_node);
        return;
    }

    lv_indev_t * indev = (lv_indev_t *)lv_event_get_param(e);
    lv_point_t point;
    lv_indev_get_point(indev, &point);

    /* Create a ray starting from the 2d point in the same direction as the camera view */
    lv_3dray_t ray = lv_gltf_get_ray_from_2d_coordinate(gltf, &point);
    /* Create the ground plane */
    lv_3dplane_t plane = lv_get_ground_plane(0.f);

    /* Check if the ray intersects with the ground plane*/
    lv_3dpoint_t collision_point;
    lv_result_t res = lv_intersect_ray_with_plane(&ray, &plane, &collision_point);
    if(res != LV_RESULT_OK) {
        /* No hit*/
        hide_node(cursor_node);
        return;
    }

    if(code == LV_EVENT_PRESSED) {
        /* Get the selected plant if the collision point is near enough
         * and also compute its offset point so the plant doesn't suddenly
         * jump to collision point*/
        selected_plant.plant = get_plant(&collision_point, &selected_plant.offset_point);
        if(!selected_plant.plant) {
            /* No plant near hit*/
            hide_node(cursor_node);
            return;
        }
    }
    if(selected_plant.plant) {
        /* Calculate the offset based on when it first was pressed
         * so that the plant doesn't jump */
        collision_point.x -= selected_plant.offset_point.x;
        collision_point.y -= selected_plant.offset_point.y;
        collision_point.z -= selected_plant.offset_point.z;

        move_plant(selected_plant.plant, &collision_point);
        rotate_plant(selected_plant.plant);
    }
}

static plant_t * get_plant(const lv_3dpoint_t * point, lv_3dpoint_t * d)
{
    for(size_t i = 0; i < PLANT_COUNT; ++i) {
        lv_3dpoint_t * plant_position = &plant_positions[i].position;
        d->x = point->x - plant_position->x;
        d->y = point->y - plant_position->y;
        d->z = point->z - plant_position->z;
        float dist_sq = d->x * d->x + d->y * d->y + d->z * d->z;
        if(dist_sq < .1) {
            return &plant_positions[i];
        }
    }
    return NULL;
}
static void plant_position_cb(lv_event_t * e)
{
    lv_gltf_model_node_t * node = (lv_gltf_model_node_t *)lv_event_get_target(e);
    for(size_t i = 0; i < PLANT_COUNT; ++i) {
        if(node == plant_positions[i].root) {
            /* Since we're using the root node of the plant, the local position will be the same as the world position*/
            lv_gltf_model_node_get_local_position(e, &plant_positions[i].position);
            lv_gltf_model_node_get_euler_rotation(e, &plant_positions[i].rotation);
        }
    }
}

static void hide_node(lv_gltf_model_node_t * cursor)
{
    /* Setting the scale to 0 will make the node essentially invisible*/
    lv_gltf_model_node_set_scale_x(cursor, 0);
    lv_gltf_model_node_set_scale_y(cursor, 0);
    lv_gltf_model_node_set_scale_z(cursor, 0);
}

static void rotate_plant(plant_t * plant)
{
    lv_gltf_model_node_set_rotation_x(plant->root, plant->rotation.x + .01f);
}
static void move_plant(plant_t * plant, const lv_3dpoint_t * point)
{

    /* Make sure we can see the cursor by setting its scale back to 1*/
    lv_gltf_model_node_set_scale_x(cursor_node, 1);
    lv_gltf_model_node_set_scale_y(cursor_node, 1);
    lv_gltf_model_node_set_scale_z(cursor_node, 1);

    /* Set the cursor and plant position to the same point*/
    lv_gltf_model_node_set_position_x(cursor_node, point->x);
    lv_gltf_model_node_set_position_y(cursor_node, point->y);
    lv_gltf_model_node_set_position_z(cursor_node, point->z);
    lv_gltf_model_node_set_position_x(plant->root, point->x);
    lv_gltf_model_node_set_position_y(plant->root, point->y);
    lv_gltf_model_node_set_position_z(plant->root, point->z);
}

/**
 * Load multiple models in a single glTF object and modify their position, rotation and scale at runtime
 */
void lv_example_gltf_3(void)
{
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    lv_obj_set_size(gltf, LV_PCT(100), LV_PCT(100));
    lv_gltf_set_pitch(gltf, -22.5f);
    lv_gltf_set_distance(gltf, 5);
    static const float plant_x[] = {-1.0, -1.0, 0.0, 1.0, 1.0};
    static const float plant_z[] = {1.0, -1.0, 0.0, 1.0, -1.0};

    for(size_t i = 0; i < PLANT_COUNT; ++i) {
        lv_gltf_model_t * plant_model = lv_gltf_load_model_from_file(gltf,
                                                                     "A:lvgl/examples/libs/gltf/webp_diffuse_transmission_plant.glb");
        plant_positions[i].root = lv_gltf_model_node_get_by_numeric_path(plant_model, ".0");

        /* Register an event so that we can get the plant position when it's updated*/
        /* For the plant we are interested in its world position and its rotation
         * but since we're using the root node of the plant, its local position
         * will be the same as the world position so we can use it instead of the world position
         * to avoid computation overhead*/
        lv_gltf_model_node_add_event_cb(plant_positions[i].root, plant_position_cb,
                                        LV_EVENT_VALUE_CHANGED, NULL);

        lv_gltf_model_node_set_position_x(plant_positions[i].root, plant_x[i]);
        lv_gltf_model_node_set_position_z(plant_positions[i].root, plant_z[i]);
    }

    lv_gltf_model_t * grid_model = lv_gltf_load_model_from_file(gltf, "A:lvgl/examples/libs/gltf/support_assets.glb");

    cursor_node = lv_gltf_model_node_get_by_path(grid_model, "/cursor");

    hide_node(cursor_node);
    lv_obj_add_event_cb(gltf, clicked_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(gltf, clicked_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(gltf, clicked_event_cb, LV_EVENT_RELEASED, NULL);
}

#else

void lv_example_gltf_3(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "glTF web support is coming soon");
    lv_obj_center(label);
}

#endif
#endif
