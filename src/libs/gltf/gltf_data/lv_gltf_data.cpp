/**
 * @file lv_gltf_data.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"

#if LV_USE_GLTF

#include <fastgltf/tools.hpp>
#include "../../../misc/lv_assert.h"
#include "../../../core/lv_obj_pos.h"
#include "../../../misc/lv_timer.h"
#include "../gltf_view/lv_gltf_view_internal.h"


/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void update_animation_cb(lv_timer_t * timer);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_gltf_model_t * lv_gltf_data_create_internal(const char * gltf_path,
                                               fastgltf::Asset asset)
{
    lv_gltf_model_t * data = (lv_gltf_model_t *)lv_zalloc(sizeof(*data));
    LV_ASSERT_MALLOC(data);
    new(data) lv_gltf_model_t;
    new(&data->asset) fastgltf::Asset(std::move(asset));
    data->filename = gltf_path;
    data->last_camera_index = -5;
    data->last_anim_num = -5;
    data->current_animation_max_time = 0;
    data->local_timestamp = 0.0f;
    data->last_material_index = 99999;
    data->last_frame_was_antialiased = false;
    data->last_frame_no_motion = false;
    data->_last_frame_no_motion = false;

    data->animation_update_timer = lv_timer_create(update_animation_cb, LV_DEF_REFR_PERIOD, data);
    lv_timer_pause(data->animation_update_timer);
    LV_ASSERT_NULL(data->animation_update_timer);

    new(&data->node_transform_cache) NodeTransformMap();
    new(&data->opaque_nodes_by_material_index) MaterialIndexMap();
    new(&data->blended_nodes_by_material_index) MaterialIndexMap();
    new(&data->validated_skins) LongVector();
    new(&data->skin_tex) IntVector();
    new(&data->local_mesh_to_center_points_by_primitive)
    NodePrimCenterMap();
    new(&data->node_by_light_index) NodeVector();
    new(&data->meshes) std::vector<lv_gltf_mesh_data_t>();
    new(&data->textures) std::vector<GLuint>();

    lv_array_init(&data->compiled_shaders, 1, sizeof(lv_gltf_compiled_shader_t));
    return data;
}

void lv_gltf_data_delete(lv_gltf_model_t * data)
{
    lv_gltf_data_delete_textures(data);
    uint32_t node_count = lv_array_size(&data->nodes);
    for(uint32_t i = 0; i < node_count; ++i) {
        lv_gltf_model_node_t * node  = (lv_gltf_model_node_t *) lv_array_at(&data->nodes, i);
        lv_gltf_model_node_deinit(node);
    }
    lv_free(data);
}

const char * lv_gltf_get_filename(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->filename;
}

size_t lv_gltf_model_get_image_count(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.images.size();
}

size_t lv_gltf_model_get_texture_count(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.textures.size();
}

GLuint lv_gltf_data_get_texture(lv_gltf_model_t * data, size_t index)
{
    LV_ASSERT_NULL(data);
    LV_ASSERT(index < data->textures.size());
    return data->textures[index];
}

size_t lv_gltf_model_get_material_count(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.materials.size();
}
size_t lv_gltf_model_get_camera_count(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.cameras.size();
}
size_t lv_gltf_model_get_node_count(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.nodes.size();
}
size_t lv_gltf_model_get_mesh_count(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.meshes.size();
}
size_t lv_gltf_model_get_scene_count(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.scenes.size();
}
size_t lv_gltf_model_get_animation_count(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.animations.size();
}

lv_result_t lv_gltf_model_play_animation(lv_gltf_model_t * model, size_t index)
{
    LV_ASSERT_NULL(model);
    if(index >= model->asset.animations.size()) {
        return LV_RESULT_INVALID;
    }

    if(lv_timer_get_paused(model->animation_update_timer)) {
        model->last_tick = lv_tick_get();
        lv_timer_resume(model->animation_update_timer);
    }

    model->current_animation_max_time = lv_gltf_data_get_animation_total_time(model, index);
    model->current_animation = index;
    model->is_animation_enabled = true;
    return LV_RESULT_OK;
}

void lv_gltf_model_pause_animation(lv_gltf_model_t * model)
{
    LV_ASSERT_NULL(model);
    model->is_animation_enabled = false;
    lv_timer_pause(model->animation_update_timer);
}

bool lv_gltf_model_is_animation_paused(lv_gltf_model_t * model)
{

    LV_ASSERT_NULL(model);
    return !model->is_animation_enabled;
}

size_t lv_gltf_model_get_animation(lv_gltf_model_t * model)
{

    LV_ASSERT_NULL(model);
    return model->current_animation;
}

lv_gltf_model_t *
lv_gltf_data_load_from_file(const char * file_path,
                            lv_opengl_shader_manager_t * shader_manager)
{
    return lv_gltf_data_load_internal(file_path, 0, shader_manager);
}

lv_gltf_model_t *
lv_gltf_data_load_from_bytes(const uint8_t * data, size_t data_size,
                             lv_opengl_shader_manager_t * shader_manager)
{
    return lv_gltf_data_load_internal(data, data_size, shader_manager);
}

fastgltf::Asset * lv_gltf_data_get_asset(lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return &data->asset;
}
double lv_gltf_data_get_radius(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->bound_radius;
}
fastgltf::math::fvec3 lv_gltf_data_get_center(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->vertex_cen;
}
fastgltf::math::fvec3 lv_gltf_data_get_bounds_min(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->vertex_min;
}
fastgltf::math::fvec3 lv_gltf_data_get_bounds_max(const lv_gltf_model_t * data)
{
    LV_ASSERT_NULL(data);
    return data->vertex_max;
}

void lv_gltf_data_copy_bounds_info(lv_gltf_model_t * to, lv_gltf_model_t * from)
{
    {
        to->vertex_min[0] = from->vertex_min[0];
        to->vertex_min[1] = from->vertex_min[1];
        to->vertex_min[2] = from->vertex_min[2];
    }
    {
        to->vertex_max[0] = from->vertex_max[0];
        to->vertex_max[1] = from->vertex_max[1];
        to->vertex_max[2] = from->vertex_max[2];
    }
    {
        to->vertex_cen[0] = from->vertex_cen[0];
        to->vertex_cen[1] = from->vertex_cen[1];
        to->vertex_cen[2] = from->vertex_cen[2];
    }
    to->bound_radius = from->bound_radius;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void update_animation_cb(lv_timer_t * timer)
{
    lv_gltf_model_t * model = (lv_gltf_model_t *)lv_timer_get_user_data(timer);

    const uint32_t current_tick = lv_tick_get();
    const uint32_t delta = lv_tick_diff(current_tick, model->last_tick);

    model->last_tick = current_tick;
    model->local_timestamp += (delta * model->viewer->desc.animation_speed_ratio) / 1000;

    if(model->local_timestamp >= model->current_animation_max_time) {
        model->local_timestamp = 50;
    }
    lv_obj_invalidate((lv_obj_t *)model->viewer);
}

#endif /*LV_USE_GLTF*/
