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

static void centerpoint_cache(lv_gltf_model_t * data, size_t index_mesh, size_t primitive);
static bool centerpoint_cache_contains(lv_gltf_model_t * data, size_t index, size_t element);
static fastgltf::math::fvec3 centerpoint_cache_get(lv_gltf_model_t * data, size_t index, size_t element,
                                                   fastgltf::math::fmat4x4 matrix);


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

fastgltf::math::fmat4x4 lv_gltf_data_get_node_transform(lv_gltf_model_t * model,
                                                        fastgltf::Node * node)
{
    LV_ASSERT(model != NULL);
    LV_ASSERT(node != NULL);
    return model->transforms[node];
}

bool lv_gltf_model_has_node_transform(lv_gltf_model_t * model, fastgltf::Node * node)
{
    LV_ASSERT(model != NULL);
    LV_ASSERT(node != NULL);
    return model->transforms.find(node) !=
           model->transforms.end();
}
void lv_gltf_model_set_transforms(lv_gltf_model_t * model, fastgltf::Node * node,
                                  fastgltf::math::fmat4x4 M)
{
    LV_ASSERT(model != NULL);
    LV_ASSERT(node != NULL);
    model->transforms[node] = M;
    model->transforms_changed = true;
}
void lv_gltf_model_clear_transforms(lv_gltf_model_t * model)
{
    LV_ASSERT(model != NULL);
    model->transforms.clear();
    model->transforms_changed = true;
}
bool lv_gltf_model_needs_transforms(lv_gltf_model_t * model)
{
    LV_ASSERT(model != NULL);
    return model->transforms.size() == 0 || model->transforms_changed;
}


fastgltf::math::fvec3 lv_gltf_data_get_centerpoint(lv_gltf_model_t * gltf_data,
                                                   fastgltf::math::fmat4x4 matrix,
                                                   size_t mesh_index, size_t elem)
{
    LV_ASSERT(gltf_data != NULL);
    if(!centerpoint_cache_contains(gltf_data, mesh_index, elem)) {
        centerpoint_cache(gltf_data, mesh_index, elem);
    }
    return centerpoint_cache_get(gltf_data, mesh_index, elem, matrix);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void centerpoint_cache(lv_gltf_model_t * data, size_t index_mesh, size_t primitive)
{
    LV_ASSERT(data != NULL);
    LV_ASSERT(index_mesh < data->asset.meshes.size());
    LV_ASSERT(primitive < data->asset.meshes[index_mesh].primitives.size());

    data->local_mesh_to_center_points_by_primitive[index_mesh][primitive] =
        lv_gltf_get_primitive_centerpoint(data, data->asset.meshes[index_mesh],
                                          primitive);
}

static bool centerpoint_cache_contains(lv_gltf_model_t * data, size_t index, size_t element)
{
    LV_ASSERT(data != NULL);
    return data->local_mesh_to_center_points_by_primitive.find(index) !=
           data->local_mesh_to_center_points_by_primitive.end() &&
           data->local_mesh_to_center_points_by_primitive[index].find(element) !=
           data->local_mesh_to_center_points_by_primitive[index].end();
}

static fastgltf::math::fvec3 centerpoint_cache_get(lv_gltf_model_t * data, size_t index,
                                                   size_t element,
                                                   fastgltf::math::fmat4x4 matrix)
{
    LV_ASSERT(data != NULL);
    LV_ASSERT(centerpoint_cache_contains(data, index, element));

    fastgltf::math::fvec4 tv = fastgltf::math::fvec4(
                                   data->local_mesh_to_center_points_by_primitive[index][element]);
    tv[3] = 1.f;
    tv = matrix * tv;
    return fastgltf::math::fvec3(tv[0], tv[1], tv[2]);
}

#endif /*LV_USE_GLTF*/
