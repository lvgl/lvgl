/**
 * @file lv_gltf_data_shader.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"

#if LV_USE_GLTF
#include "../../../misc/lv_array.h"
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

void lv_gltf_store_compiled_shader(lv_gltf_model_t * data, size_t identifier, lv_gltf_compiled_shader_t * shader)
{
    const size_t index = identifier - 1;
    bool has_to_resize = index >= lv_array_size(&data->compiled_shaders);
    if(!has_to_resize) {
        lv_array_assign(&data->compiled_shaders, index, shader);
    }
    while(index >= lv_array_size(&data->compiled_shaders)) {
        lv_array_push_back(&data->compiled_shaders, shader);
    }
}

lv_gltf_compiled_shader_t * lv_gltf_get_compiled_shader(lv_gltf_model_t * data, size_t identifier)
{
    const size_t index = identifier - 1;
    LV_ASSERT(index < lv_array_size(&data->compiled_shaders));
    return (lv_gltf_compiled_shader_t *)lv_array_at(&data->compiled_shaders, index);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GLTF*/
