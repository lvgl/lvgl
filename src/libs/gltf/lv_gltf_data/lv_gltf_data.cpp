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


/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_gltf_data_t * lv_gltf_data_create_internal(const char * gltf_path,
                                              fastgltf::Asset asset)
{
    lv_gltf_data_t * data = (lv_gltf_data_t *)lv_zalloc(sizeof(*data));
    LV_ASSERT_MALLOC(data);
    new(data) lv_gltf_data_t;
    new(&data->asset) fastgltf::Asset(std::move(asset));
    data->filename = gltf_path;
    data->has_any_cameras = false;
    data->current_camera_index = -1;
    data->last_camera_index = -5;
    data->last_anim_num = -5;
    data->cur_anim_maxtime = -1.f;
    data->local_timestamp = 0.0f;
    data->last_material_index = 99999;
    data->last_frame_was_antialiased = false;
    data->_last_frame_no_motion = false;
    data->__last_frame_no_motion = false;
    data->nodes_parsed = false;
    new(&data->node_binds) NodeOverrideMap();
    new(&data->all_binds) OverrideVector();
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
    new(&data->shader_uniforms) std::vector<lv_gltf_uniform_locations_t>();
    new(&data->shader_sets) std::vector<lv_gltf_renwin_shaderset_t>();
    return data;
}

void lv_gltf_data_destroy(lv_gltf_data_t * data)
{
    lv_gltf_data_destroy_textures(data);
    lv_free(data);
}

const char * lv_gltf_get_filename(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->filename;
}

size_t lv_gltf_data_get_image_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.images.size();
}

size_t lv_gltf_data_get_texture_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.textures.size();
}

GLuint lv_gltf_data_get_texture(lv_gltf_data_t * data, size_t index)
{
    LV_ASSERT_NULL(data);
    LV_ASSERT(index < data->textures.size());
    return data->textures[index];
}

size_t lv_gltf_data_get_material_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.materials.size();
}
size_t lv_gltf_data_get_camera_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.cameras.size();
}
size_t lv_gltf_data_get_node_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.nodes.size();
}
size_t lv_gltf_data_get_mesh_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.meshes.size();
}
size_t lv_gltf_data_get_scene_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.scenes.size();
}
size_t lv_gltf_data_get_animation_count(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->asset.animations.size();
}

lv_gltf_data_t *
lv_gltf_data_load_from_file(const char * file_path,
                            lv_gl_shader_manager_t * shader_manager)
{
    return lv_gltf_data_load_internal(file_path, 0, shader_manager);
}

lv_gltf_data_t *
lv_gltf_data_load_from_bytes(const uint8_t * data, size_t data_size,
                             lv_gl_shader_manager_t * shader_manager)
{
    return lv_gltf_data_load_internal(data, data_size, shader_manager);
}

fastgltf::Asset * lv_gltf_data_get_asset(lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return &data->asset;
}
double lv_gltf_data_get_radius(lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->bound_radius;
}
fastgltf::math::fvec3 lv_gltf_data_get_center(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->vertex_cen;
}
fastgltf::math::fvec3 lv_gltf_data_get_bounds_min(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->vertex_min;
}
fastgltf::math::fvec3 lv_gltf_data_get_bounds_max(const lv_gltf_data_t * data)
{
    LV_ASSERT_NULL(data);
    return data->vertex_max;
}




void lv_gltf_data_copy_bounds_info(lv_gltf_data_t * to, lv_gltf_data_t * from)
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

#endif /*LV_USE_GLTF*/

