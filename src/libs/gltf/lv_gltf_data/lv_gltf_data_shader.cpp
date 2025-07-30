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
    if(index >= lv_array_size(&data->compiled_shaders)) {
        lv_array_resize(&data->compiled_shaders, index);
        lv_array_push_back(&data->compiled_shaders, shader);
    }
    else {
        void * dest = lv_array_at(&data->compiled_shaders, index);
        lv_memcpy(dest, shader, sizeof(*shader));
    }
}

lv_gltf_compiled_shader_t * lv_gltf_get_compiled_shader(lv_gltf_model_t * data, size_t identifier)
{
    LV_ASSERT(identifier - 1 < lv_array_size(&data->compiled_shaders));
    return (lv_gltf_compiled_shader_t *)lv_array_at(&data->compiled_shaders, identifier - 1);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GLTF*/
