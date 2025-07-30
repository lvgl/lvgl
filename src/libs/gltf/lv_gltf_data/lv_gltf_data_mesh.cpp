/**
 * @file lv_gltf_data_mesh.cpp
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

lv_gltf_mesh_data_t * lv_gltf_get_new_meshdata(lv_gltf_model_t * data)
{
    data->meshes.emplace_back(lv_gltf_mesh_data_t {});
    return &(data->meshes[data->meshes.size() - 1]);
}


lv_gltf_mesh_data_t * lv_gltf_data_get_mesh(lv_gltf_model_t * data, size_t index)
{
    return &data->meshes[index];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GLTF*/
