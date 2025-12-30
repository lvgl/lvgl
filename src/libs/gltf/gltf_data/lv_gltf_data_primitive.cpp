/**
 * @file lv_gltf_data_primitive.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"
#if LV_USE_GLTF
#include "../../../misc/lv_math.h"

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

lv_gltf_primitive_t * lv_gltf_data_get_primitive_from_mesh(lv_gltf_mesh_data_t * mesh, size_t index)
{
    return &(mesh->primitives[index]);
}

void lv_gltf_data_add_opaque_node_primitive(lv_gltf_model_t * data, size_t index,
                                            fastgltf::Node * node, size_t primitive_index)
{
    data->opaque_nodes_by_material_index[index].emplace_back(
        std::make_pair(node, primitive_index));
}

void lv_gltf_data_add_blended_node_primitive(lv_gltf_model_t * data, size_t index,
                                             fastgltf::Node * node, size_t primitive_index)
{
    data->blended_nodes_by_material_index[index].push_back(
        std::make_pair(node, primitive_index));
}

fastgltf::math::fvec4 lv_gltf_get_primitive_centerpoint(lv_gltf_model_t * data,
                                                        fastgltf::Mesh & mesh,
                                                        uint32_t prim_num)
{
    fastgltf::math::fvec4 result{ 0.f };
    fastgltf::math::fvec3 v_min{ 999999999.f };
    fastgltf::math::fvec3 v_max{ -999999999.f };
    fastgltf::math::fvec3 v_cen{ 0.f };
    float radius = 0.f;

    if(mesh.primitives.size() <= prim_num) {
        return result;
    }
    const auto & it = mesh.primitives[prim_num];
    const auto & asset = data->asset;

    const auto * positionIt = it.findAttribute("POSITION");
    const auto & positionAccessor =
        asset.accessors[positionIt->accessorIndex];
    if(!positionAccessor.bufferViewIndex.has_value()) {
        return result;
    }

    if(!(positionAccessor.min.has_value() &&
         positionAccessor.max.has_value())) {
        LV_LOG_ERROR(
            "Could not get primitive center point. Missing min/max values");
        return result;
    }

    fastgltf::math::fvec4 t_min{
        (float)(positionAccessor.min.value().get<double>((size_t)0)),
        (float)(positionAccessor.min.value().get<double>((size_t)1)),
        (float)(positionAccessor.min.value().get<double>((size_t)2)),
        0.f
    };
    fastgltf::math::fvec4 t_max{
        (float)(positionAccessor.max.value().get<double>((size_t)0)),
        (float)(positionAccessor.max.value().get<double>((size_t)1)),
        (float)(positionAccessor.max.value().get<double>((size_t)2)),
        0.f
    };

    v_max[0] = LV_MAX(t_min.x(), t_max.x());
    v_max[1] = LV_MAX(t_min.y(), t_max.y());
    v_max[2] = LV_MAX(t_min.z(), t_max.z());
    v_min[0] = LV_MIN(t_min.x(), t_max.x());
    v_min[1] = LV_MIN(t_min.y(), t_max.y());
    v_min[2] = LV_MIN(t_min.z(), t_max.z());
    v_cen[0] = (v_max[0] + v_min[0]) / 2.0f;
    v_cen[1] = (v_max[1] + v_min[1]) / 2.0f;
    v_cen[2] = (v_max[2] + v_min[2]) / 2.0f;
    float size_x = v_max[0] - v_min[0];
    float size_y = v_max[1] - v_min[1];
    float size_z = v_max[2] - v_min[2];
    radius = std::sqrt((size_x * size_x) + (size_y * size_y) +
                       (size_z * size_z)) /
             2.0f;
    result[0] = v_cen[0];
    result[1] = v_cen[1];
    result[2] = v_cen[2];
    result[3] = radius;
    return result;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GLTF*/
