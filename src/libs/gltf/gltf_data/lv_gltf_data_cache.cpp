/**
 * @file lv_gltf_data_cache.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"

#if LV_USE_GLTF
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

fastgltf::math::fmat4x4 lv_gltf_data_get_cached_transform(lv_gltf_model_t * data,
                                                          fastgltf::Node * node)
{
    return data->node_transform_cache[node];
}

bool lv_gltf_data_has_cached_transform(lv_gltf_model_t * data, fastgltf::Node * node)
{
    return (data->node_transform_cache.find(node) !=
            data->node_transform_cache.end());
}
void lv_gltf_data_set_cached_transform(lv_gltf_model_t * data, fastgltf::Node * node,
                                       fastgltf::math::fmat4x4 M)
{
    data->node_transform_cache[node] = M;
}
void lv_gltf_data_clear_transform_cache(lv_gltf_model_t * data)
{
    data->node_transform_cache.clear();
}
bool lv_gltf_data_transform_cache_is_empty(lv_gltf_model_t * data)
{
    return data->node_transform_cache.size() == 0;
}

void recache_centerpoint(lv_gltf_model_t * data, size_t index_mesh, int32_t primitive)
{
    data->local_mesh_to_center_points_by_primitive[index_mesh][primitive] =
        lv_gltf_get_primitive_centerpoint(data, data->asset.meshes[index_mesh],
                                          primitive);
}

fastgltf::math::fvec3 lv_gltf_data_get_centerpoint(lv_gltf_model_t * gltf_data,
                                                   fastgltf::math::fmat4x4 matrix,
                                                   size_t mesh_index, int32_t elem)
{
    if(!lv_gltf_data_centerpoint_cache_contains(gltf_data, mesh_index, elem)) {
        recache_centerpoint(gltf_data, mesh_index, elem);
    }
    return get_cached_centerpoint(gltf_data, mesh_index, elem, matrix);
}
bool lv_gltf_data_centerpoint_cache_contains(lv_gltf_model_t * data, size_t index, int32_t element)
{
    return data->local_mesh_to_center_points_by_primitive.find(index) !=
           data->local_mesh_to_center_points_by_primitive.end() &&
           data->local_mesh_to_center_points_by_primitive[index].find(element) !=
           data->local_mesh_to_center_points_by_primitive[index].end();
}

fastgltf::math::fvec3 get_cached_centerpoint(lv_gltf_model_t * data, size_t index,
                                             int32_t element,
                                             fastgltf::math::fmat4x4 matrix)
{
    fastgltf::math::fvec4 tv = fastgltf::math::fvec4(
                                   data->local_mesh_to_center_points_by_primitive[index][element]);
    tv[3] = 1.f;
    tv = matrix * tv;
    return fastgltf::math::fvec3(tv[0], tv[1], tv[2]);
}



/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GLTF*/
