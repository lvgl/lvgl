/**
 * @file lv_gltf_data_skin.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"

#if LV_USE_GLTF
#include <algorithm>

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

GLuint lv_gltf_data_get_skin_texture_at(lv_gltf_model_t * data, size_t index)
{
    return data->skin_tex[index];
}

bool lv_gltf_data_validated_skins_contains(lv_gltf_model_t * data, size_t index)
{
    return ((std::find(data->validated_skins.begin(),
                       data->validated_skins.end(),
                       index) != data->validated_skins.end()));
}

void lv_gltf_data_validate_skin(lv_gltf_model_t * data, size_t index)
{
    data->validated_skins.push_back(index);
}

size_t lv_gltf_data_get_skins_size(lv_gltf_model_t * data)
{
    return data->validated_skins.size();
}
size_t lv_gltf_data_get_skin(lv_gltf_model_t * data, size_t index)
{
    return data->validated_skins[index];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GLTF*/
