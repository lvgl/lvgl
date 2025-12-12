/**
 * @file lv_gltf_view.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_view_internal.h"

#if LV_USE_GLTF

#include "../gltf_data/lv_gltf_model.h"
#include "../gltf_data/lv_gltf_data_internal.hpp"
#include "../../../draw/lv_draw_3d.h"
#include "../fastgltf/lv_fastgltf.hpp"
#include "../../../core/lv_obj_class_private.h"
#include "../../../misc/lv_types.h"
#include "../../../widgets/3dtexture/lv_3dtexture.h"
#include "../gltf_environment/lv_gltf_environment.h"
#include "assets/lv_gltf_view_shader.h"
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>

/*********************
 *      DEFINES
 *********************/


#define MY_CLASS (&lv_gltf_class)

#ifndef LV_GLTF_INITIAL_MODEL_CAPACITY
    #define LV_GLTF_INITIAL_MODEL_CAPACITY 1
#endif /*LV_GLTF_INITIAL_MODEL_CAPACITY*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_gltf_model_t * lv_gltf_add_model(lv_gltf_t * viewer, lv_gltf_model_t * model);
static void lv_gltf_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gltf_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gltf_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_gltf_view_state_init(lv_gltf_t * state);
static void lv_gltf_view_desc_init(lv_gltf_view_desc_t * state);
static void lv_gltf_parse_model(lv_gltf_t * viewer, lv_gltf_model_t * model);
static void destroy_environment(lv_gltf_environment_t * env);
static void setup_compile_and_load_bg_shader(lv_opengl_shader_manager_t * manager);
static void setup_background_environment(GLuint program, GLuint * vao, GLuint * indexBuffer, GLuint * vertexBuffer);

static lv_result_t create_default_environment(lv_gltf_t * gltf);

static void display_refr_end_event_cb(lv_event_t * e);

const lv_obj_class_t lv_gltf_class = {
    &lv_3dtexture_class,
    lv_gltf_constructor,
    lv_gltf_destructor,
    lv_gltf_event,
#if LV_USE_OBJ_PROPERTY
    0,
    0,
    NULL,
    0,
    NULL,
    0,
#endif
    NULL,
    "lv_gltf",
    LV_DPI_DEF * 2,
    LV_DPI_DEF / 10,
    LV_OBJ_CLASS_EDITABLE_INHERIT,
    LV_OBJ_CLASS_GROUP_DEF_INHERIT,
    sizeof(lv_gltf_t),
    LV_OBJ_CLASS_THEME_INHERITABLE_FALSE
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_gltf_create(lv_obj_t * parent)
{
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    lv_display_t * disp = lv_obj_get_display(obj);
    LV_ASSERT_NULL(disp);
    lv_display_add_event_cb(disp, display_refr_end_event_cb, LV_EVENT_REFR_READY, obj);
    return obj;
}

lv_gltf_model_t * lv_gltf_load_model_from_file(lv_obj_t * obj, const char * path)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(!viewer->environment) {
        lv_result_t res = create_default_environment(viewer);
        if(res != LV_RESULT_OK) {
            return NULL;
        }
    }

    lv_gltf_model_t * model = lv_gltf_data_load_from_file(path, &viewer->shader_manager);
    return lv_gltf_add_model(viewer, model);
}

lv_gltf_model_t * lv_gltf_load_model_from_bytes(lv_obj_t * obj, const uint8_t * bytes, size_t len)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(!viewer->environment) {
        lv_result_t res = create_default_environment(viewer);
        if(res != LV_RESULT_OK) {
            return NULL;
        }
    }

    lv_gltf_model_t * model = lv_gltf_data_load_from_bytes(bytes, len, &viewer->shader_manager);
    return lv_gltf_add_model(viewer, model);
}
void lv_gltf_set_environment(lv_obj_t * obj, lv_gltf_environment_t * env)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * gltf = (lv_gltf_t *)obj;
    if(env == NULL) {
        LV_LOG_WARN("Refusing to assign a NULL environment to the glTF object");
        return;
    }

    if(gltf->environment && gltf->owns_environment) {
        lv_gltf_environment_delete(gltf->environment);
        gltf->environment = NULL;
    }
    gltf->environment = env;
    gltf->owns_environment = false;
}

size_t lv_gltf_get_model_count(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_array_size(&((lv_gltf_t *)obj)->models);
}

lv_gltf_model_t * lv_gltf_get_model_by_index(lv_obj_t * obj, size_t id)
{

    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *) obj;

    if(id >= lv_array_size(&viewer->models)) {
        return NULL;
    }
    return *(lv_gltf_model_t **)lv_array_at(&((lv_gltf_t *)obj)->models, id);

}
lv_gltf_model_t * lv_gltf_get_primary_model(lv_obj_t * obj)
{

    return lv_gltf_get_model_by_index(obj, 0);
}

void lv_gltf_set_yaw(lv_obj_t * obj, float yaw)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.yaw = yaw;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_yaw(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.yaw;
}

void lv_gltf_set_pitch(lv_obj_t * obj, float pitch)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.pitch = pitch;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_pitch(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.pitch;
}

void lv_gltf_set_fov(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.fov = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_fov(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.fov;
}

void lv_gltf_set_distance(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.distance = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_distance(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.distance;
}

float lv_gltf_get_world_distance(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    lv_gltf_view_desc_t * view_desc = &viewer->desc;
    if(viewer->models.size == 0) {
        return 0.0f;
    }
    lv_gltf_model_t * model = *(lv_gltf_model_t **)lv_array_at(&viewer->models, 0);
    return (lv_gltf_data_get_radius(model) * LV_GLTF_DISTANCE_SCALE_FACTOR) * view_desc->distance;
}

void lv_gltf_set_animation_speed(lv_obj_t * obj, uint32_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.animation_speed_ratio = value;
    lv_obj_invalidate(obj);
}

uint32_t lv_gltf_get_animation_speed(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.animation_speed_ratio;
}

void lv_gltf_set_focal_x(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.focal_x = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_focal_x(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.focal_x;
}

void lv_gltf_set_focal_y(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.focal_y = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_focal_y(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.focal_y;
}

void lv_gltf_set_focal_z(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.focal_z = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_focal_z(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.focal_z;
}

void lv_gltf_set_camera(lv_obj_t * obj, uint32_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(lv_array_is_empty(&viewer->models)) {
        return;
    }

    lv_gltf_model_t * model = *(lv_gltf_model_t **) lv_array_at(&viewer->models, 0);

    if(value > model->asset.cameras.size()) {
        return;
    }

    model->camera = value;
    lv_obj_invalidate(obj);
}

uint32_t lv_gltf_get_camera(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(lv_array_is_empty(&viewer->models)) {
        return 0;
    }
    const lv_gltf_model_t * model = *(const lv_gltf_model_t **)lv_array_at(&viewer->models, 0);
    return model->camera;
}

uint32_t lv_gltf_get_camera_count(const lv_obj_t * obj)
{

    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(lv_array_is_empty(&viewer->models)) {
        return 0;
    }
    const lv_gltf_model_t * model = *(const lv_gltf_model_t **) lv_array_at(&viewer->models, 0);
    return lv_gltf_model_get_camera_count(model);
}

void lv_gltf_set_antialiasing_mode(lv_obj_t * obj, lv_gltf_aa_mode_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.aa_mode = value;
    lv_obj_invalidate(obj);
}

lv_gltf_aa_mode_t lv_gltf_get_antialiasing_mode(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.aa_mode;
}

void lv_gltf_set_background_mode(lv_obj_t * obj, lv_gltf_bg_mode_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.bg_mode = value;
    lv_obj_invalidate(obj);
}

lv_gltf_bg_mode_t lv_gltf_get_background_mode(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.bg_mode;
}

void lv_gltf_set_background_blur(lv_obj_t * obj, uint32_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    if(value > 100) {
        value = 100;
    }
    viewer->desc.blur_bg = value / 100.f;
    lv_obj_invalidate(obj);
}

uint32_t lv_gltf_get_background_blur(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.blur_bg * 100;
}

void lv_gltf_set_env_brightness(lv_obj_t * obj, uint32_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.env_pow = value / 100.;
    lv_obj_invalidate(obj);
}

uint32_t lv_gltf_get_env_brightness(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.env_pow * 100;
}

void lv_gltf_set_image_exposure(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.exposure = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_image_exposure(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.exposure;
}
void lv_gltf_recenter(lv_obj_t * obj, lv_gltf_model_t * model)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    if(model == NULL) {
        LV_ASSERT(lv_array_size(&viewer->models) > 0);
        model = *(lv_gltf_model_t **)lv_array_at(&viewer->models, 0);
    }

    const auto & center_position = lv_gltf_data_get_center(model);
    viewer->desc.focal_x = center_position[0];
    viewer->desc.focal_y = center_position[1];
    viewer->desc.focal_z = center_position[2];
}

lv_3dray_t lv_gltf_get_ray_from_2d_coordinate(lv_obj_t * obj, const lv_point_t * screen_pos)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    float norm_mouse_x = (float)screen_pos->x / (float)(lv_obj_get_width(obj));
    float norm_mouse_y = (float)screen_pos->y / (float)(lv_obj_get_height(obj));

    lv_3dray_t outray = {0};

    fastgltf::math::fmat4x4 proj_mat = fastgltf::math::invert(fastgltf::math::fmat4x4(viewer->projection_matrix));

    /* Convert mouse coordinates to NDC */
    float x = norm_mouse_x * 2.0f - 1.0f;
    float y = 1.0f - (norm_mouse_y * 2.0f);
    float z = -1.0f; /* Clip space z */

    fastgltf::math::fvec4 clip_space_pos = fastgltf::math::fvec4(x, y, z, 1.f);
    auto ray_eye = (proj_mat) * clip_space_pos;
    ray_eye[2] = -1.0f;
    ray_eye[3] = 0.0f;

    /* Calculate ray world direction */
    fastgltf::math::fvec4 ray_world = fastgltf::math::invert(viewer->view_matrix) * ray_eye;
    auto ray_direction = fastgltf::math::normalize(fastgltf::math::fvec3(ray_world[0], ray_world[1], ray_world[2]));

    outray.direction = {ray_direction[0], ray_direction[1], ray_direction[2]};
    outray.origin = {viewer->camera_pos[0], viewer->camera_pos[1], viewer->camera_pos[2]};

    return outray;
}

lv_result_t lv_intersect_ray_with_plane(const lv_3dray_t * ray, const lv_3dplane_t * plane,
                                        lv_3dpoint_t * collision_point)
{
    fastgltf::math::fvec3 plane_center = fastgltf::math::fvec3(plane->origin.x, plane->origin.y, plane->origin.z);
    fastgltf::math::fvec3 plane_normal = fastgltf::math::fvec3(plane->direction.x, plane->direction.y, plane->direction.z);
    fastgltf::math::fvec3 ray_start = fastgltf::math::fvec3(ray->origin.x, ray->origin.y, ray->origin.z);
    fastgltf::math::fvec3 ray_direction = fastgltf::math::fvec3(ray->direction.x, ray->direction.y, ray->direction.z);

    float denom = fastgltf::math::dot(plane_normal, ray_direction);
    if(fabs(denom) > 1e-6) {  /* Check if the ray is not parallel to the plane */
        fastgltf::math::fvec3 diff = plane_center - ray_start;
        float t = fastgltf::math::dot(diff, plane_normal) / denom;

        if(t >= 0) {  /* Intersection occurs ahead of the ray origin */
            /* Calculate the collision point */
            (*collision_point).x = ray_start[0] + t * ray_direction[0];
            (*collision_point).y = ray_start[1] + t * ray_direction[1];
            (*collision_point).z = ray_start[2] + t * ray_direction[2];
            return LV_RESULT_OK; /* Collision point found */
        }
    }
    return LV_RESULT_INVALID; /* No intersection */
}

lv_3dplane_t lv_gltf_get_current_view_plane(lv_obj_t * obj, float distance)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    lv_3dplane_t outplane = {0};

    /* Forward vector is the third column of the matrix */
    auto forward = fastgltf::math::fvec3(viewer->view_matrix[0][2], viewer->view_matrix[1][2], viewer->view_matrix[2][2]);
    forward = fastgltf::math::normalize(forward);

    /* Calculate the plane center */
    const auto & camera_pos = viewer->camera_pos;
    auto plane_pos = fastgltf::math::fvec3(camera_pos[0], camera_pos[1], camera_pos[2]) - forward * distance;
    outplane.origin = {plane_pos[0], plane_pos[1], plane_pos[2]};
    outplane.direction = {-forward[0], -forward[1], -forward[2]};
    return outplane;
}

lv_result_t lv_gltf_world_to_screen(lv_obj_t * obj, const lv_3dpoint_t world_pos, lv_point_t * screen_pos)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    fastgltf::math::fvec4 world_position_h = fastgltf::math::fvec4(world_pos.x, world_pos.y, world_pos.z, 1.0f);
    fastgltf::math::fvec4 clip_space_pos = viewer->projection_matrix * viewer->view_matrix * world_position_h;

    /* Check for perspective division (w must not be zero) */
    if(clip_space_pos[3] == 0.0f) {
        screen_pos->x = -1;
        screen_pos->y = -1;
        return LV_RESULT_INVALID; /* Position is not valid for screen mapping */
    }

    clip_space_pos /= clip_space_pos[3];
    float norm_screen_x = clip_space_pos[0] * 0.5f + 0.5f;
    float norm_screen_y = 0.5f - (clip_space_pos[1] * 0.5f);
    int32_t win_width = lv_obj_get_width(obj);
    int32_t win_height = lv_obj_get_height(obj);
    screen_pos->x = norm_screen_x * win_width;
    screen_pos->y = norm_screen_y * win_height;
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_gltf_model_t * lv_gltf_add_model(lv_gltf_t * viewer, lv_gltf_model_t * model)
{
    if(!model) {
        return NULL;
    }
    if(lv_array_push_back(&viewer->models, &model) == LV_RESULT_INVALID) {
        lv_gltf_data_delete(model);
        return NULL;
    }
    model->viewer = viewer;
    lv_gltf_parse_model(viewer, model);


    if(lv_array_size(&viewer->models) == 1) {
        lv_gltf_recenter((lv_obj_t *)viewer, model);
    }

    return model;
}

static lv_result_t create_default_environment(lv_gltf_t * gltf)
{
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    gltf->environment = lv_gltf_environment_create(sampler, NULL);
    lv_gltf_ibl_sampler_delete(sampler);
    if(!gltf->environment) {
        LV_LOG_WARN("Failed to create default gltf environment");
        return LV_RESULT_INVALID;
    }
    gltf->owns_environment = true;
    return LV_RESULT_OK;
}

static void lv_gltf_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_gltf_t * view = (lv_gltf_t *)obj;
    lv_gltf_view_state_init(view);
    lv_gltf_view_desc_init(&view->desc);
    view->view_matrix = fastgltf::math::fmat4x4(1.0f);
    view->projection_matrix = fastgltf::math::fmat4x4(1.0f);
    view->view_projection_matrix = fastgltf::math::fmat4x4(1.0f);
    view->camera_pos = fastgltf::math::fvec3(0.0f);
    view->texture.h_flip = false;
    view->texture.v_flip = false;
    new(&view->ibm_by_skin_then_node) std::map<int32_t, std::map<fastgltf::Node *, fastgltf::math::fmat4x4>>;

    lv_opengl_shader_portions_t portions;
    lv_gltf_view_shader_get_src(&portions);
    char * vertex_shader = lv_gltf_view_shader_get_vertex();
    char * frag_shader = lv_gltf_view_shader_get_fragment();
    lv_opengl_shader_manager_init(&view->shader_manager, portions.all, portions.count, vertex_shader, frag_shader);
    lv_free(vertex_shader);
    lv_free(frag_shader);

    lv_array_init(&view->models, LV_GLTF_INITIAL_MODEL_CAPACITY, sizeof(lv_gltf_model_t *));

    LV_TRACE_OBJ_CREATE("end");
}

static void lv_gltf_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = (lv_obj_t *)lv_event_get_current_target(e);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(code == LV_EVENT_DRAW_MAIN) {
        GLuint texture_id = lv_gltf_view_render(viewer);
        lv_3dtexture_set_src((lv_obj_t *)&viewer->texture, (lv_3dtexture_id_t)texture_id);
    }

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) {
        return;
    }
}
static void lv_gltf_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_gltf_t * view = (lv_gltf_t *)obj;
    lv_opengl_shader_manager_deinit(&view->shader_manager);
    using IbmBySkinThenNodeMap = std::map<int32_t, std::map<fastgltf::Node *, fastgltf::math::fmat4x4>>;

    view->ibm_by_skin_then_node.~IbmBySkinThenNodeMap();
    const size_t n = lv_array_size(&view->models);
    for(size_t i = 0; i < n; ++i) {
        lv_gltf_data_delete(*(lv_gltf_model_t **)lv_array_at(&view->models, i));
    }
    if(view->environment && view->owns_environment) {
        lv_gltf_environment_delete(view->environment);
    }
    lv_display_t * disp = lv_obj_get_display(obj);
    LV_ASSERT_NULL(disp);
    lv_display_remove_event_cb_with_user_data(disp, display_refr_end_event_cb, obj);
}

static void lv_gltf_view_state_init(lv_gltf_t * view)
{
    lv_memset(&view->state, 0, sizeof(view->state));
    view->state.opaque_frame_buffer_width = LV_GLTF_TRANSMISSION_PASS_SIZE;
    view->state.opaque_frame_buffer_height = LV_GLTF_TRANSMISSION_PASS_SIZE;
    view->state.material_variant = 0;
    view->state.render_state_ready = false;
    view->state.render_opaque_buffer = false;
}
static void lv_gltf_view_desc_init(lv_gltf_view_desc_t * desc)
{
    lv_memset(desc, 0, sizeof(*desc));
    desc->distance = 2.f;
    desc->exposure = 1.0f;
    desc->env_pow = 1.8f;
    desc->blur_bg = 0.5f;
    desc->bg_mode = LV_GLTF_BG_MODE_ENVIRONMENT;
    desc->aa_mode = LV_GLTF_AA_MODE_OFF;
    desc->fov = 45.f;
    desc->animation_speed_ratio = LV_GLTF_ANIM_SPEED_NORMAL;
    desc->frame_was_antialiased = false;
}
static void lv_gltf_parse_model(lv_gltf_t * viewer, lv_gltf_model_t * model)
{
    const auto & iterate_callback = [&](fastgltf::Node & node, const fastgltf::math::fmat4x4 & matrix) {
        LV_UNUSED(matrix);
        if(!node.meshIndex) {
            return;
        }
        auto & mesh_index = node.meshIndex.value();
        if(node.skinIndex) {
            auto skin_index = node.skinIndex.value();
            if(!lv_gltf_data_validated_skins_contains(model, skin_index)) {
                lv_gltf_data_validate_skin(model, skin_index);
                auto skin = model->asset.skins[skin_index];
                if(skin.inverseBindMatrices) {
                    auto & ibm_value = skin.inverseBindMatrices.value();
                    auto & ibm_accessor = model->asset.accessors[ibm_value];
                    if(ibm_accessor.bufferViewIndex) {
                        fastgltf::iterateAccessorWithIndex<fastgltf::math::fmat4x4>(
                            model->asset, ibm_accessor,
                        [&](fastgltf::math::fmat4x4 _matrix, std::size_t idx) {
                            auto & joint_node = model->asset.nodes[skin.joints[idx]];
                            viewer->ibm_by_skin_then_node[skin_index][&joint_node] = _matrix;
                        });
                    }
                }
            }
        }
        for(size_t mp = 0; mp < model->asset.meshes[mesh_index].primitives.size(); mp++) {
            auto & model_primitive = model->asset.meshes[mesh_index].primitives[mp];
            const auto & mappings = model_primitive.mappings;
            ssize_t material_index =
                (!mappings.empty() && mappings[viewer->state.material_variant]) ?
                mappings[viewer->state.material_variant].value() + 1 :
                ((model_primitive.materialIndex) ? (model_primitive.materialIndex.value() + 1) : 0);
            if(material_index < 0) {
                lv_gltf_data_add_opaque_node_primitive(model, 0, &node, mp);
                continue;
            }
            const fastgltf::Material & material = model->asset.materials[material_index - 1];

            viewer->state.render_opaque_buffer |= material.transmission != NULL;

            if(material.alphaMode == fastgltf::AlphaMode::Blend || material.transmission != NULL) {
                lv_gltf_data_add_blended_node_primitive(model, material_index + 1, &node, mp);
            }
            else {
                lv_gltf_data_add_opaque_node_primitive(model, material_index + 1, &node, mp);
            }

            lv_array_t defines;
            lv_array_init(&defines, 64, sizeof(lv_opengl_shader_define_t));
            lv_result_t result =
                lv_gltf_view_shader_injest_discover_defines(&defines, model, &node, &model_primitive);

            LV_ASSERT_MSG(result == LV_RESULT_OK, "Couldn't injest shader defines");
            lv_gltf_compiled_shader_t compiled_shader;
            compiled_shader.shaderset = lv_gltf_view_shader_compile_program(viewer, (lv_opengl_shader_define_t *)defines.data,
                                                                            lv_array_size(&defines));
            compiled_shader.uniforms = lv_gltf_uniform_locations_create(compiled_shader.shaderset.program);
            lv_gltf_store_compiled_shader(model, material_index, &compiled_shader);
            const size_t n = lv_array_size(&defines);
            for(size_t i = 0; i < n; ++i) {
                lv_opengl_shader_define_t * define = (lv_opengl_shader_define_t *) lv_array_at(&defines, i);
                if(define->value_allocated) {
                    lv_free((void *)define->value);
                }
            }
            lv_array_deinit(&defines);
        }
    };

    setup_compile_and_load_bg_shader(&viewer->shader_manager);
    fastgltf::iterateSceneNodes(model->asset, 0, fastgltf::math::fmat4x4(), iterate_callback);
}

static void setup_compile_and_load_bg_shader(lv_opengl_shader_manager_t * manager)
{
    lv_opengl_shader_define_t frag_defs[1] = { { "TONEMAP_KHR_PBR_NEUTRAL", NULL, false} };
    uint32_t frag_shader_hash ;
    uint32_t vert_shader_hash;
    lv_result_t res = lv_opengl_shader_manager_select_shader(manager, "cubemap.frag", frag_defs, 1,
                                                             LV_OPENGL_GLSL_VERSION_300ES,
                                                             &frag_shader_hash);

    LV_ASSERT(res == LV_RESULT_OK);
    res = lv_opengl_shader_manager_select_shader(manager, "cubemap.vert", nullptr, 0, LV_OPENGL_GLSL_VERSION_300ES,
                                                 &vert_shader_hash);
    LV_ASSERT(res == LV_RESULT_OK);

    lv_opengl_shader_program_t * program = lv_opengl_shader_manager_get_program(manager, frag_shader_hash,
                                                                                vert_shader_hash);

    manager->bg_program = lv_opengl_shader_program_get_id(program);
    setup_background_environment(manager->bg_program, &manager->bg_vao, &manager->bg_index_buf, &manager->bg_vertex_buf);
}


static void setup_background_environment(GLuint program, GLuint * vao, GLuint * indexBuffer, GLuint * vertexBuffer)
{
    int32_t indices[] = { 1, 2, 0, 2, 3, 0, 6, 2, 1, 1, 5, 6, 6, 5, 4, 4, 7, 6,
                          6, 3, 2, 7, 3, 6, 3, 7, 0, 7, 4, 0, 5, 1, 0, 4, 5, 0
                        };
    float verts[] = { -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
                      -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f
                    };

    GL_CALL(glUseProgram(program));
    GL_CALL(glGenVertexArrays(1, vao));
    GL_CALL(glBindVertexArray(*vao));
    GL_CALL(glGenBuffers(1, indexBuffer));
    GL_CALL(glGenBuffers(1, vertexBuffer));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBuffer));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GLint positionAttributeLocation = glGetAttribLocation(program, "a_position");

    // Specify the layout of the vertex data
    glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(positionAttributeLocation);

    GL_CALL(glBindVertexArray(0));
    GL_CALL(glUseProgram(0));
}


static void display_refr_end_event_cb(lv_event_t * e)
{
    lv_gltf_t * viewer = (lv_gltf_t *) lv_event_get_user_data(e);
    uint32_t model_count = lv_array_size(&viewer->models);
    for(uint32_t i = 0; i < model_count; ++i) {
        lv_gltf_model_t * model = *(lv_gltf_model_t **)lv_array_at(&viewer->models, i);
        lv_gltf_model_send_new_values(model);
    }
}
#endif /*LV_USE_GLTF*/
