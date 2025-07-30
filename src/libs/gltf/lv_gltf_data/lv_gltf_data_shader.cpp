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

void lv_gltf_store_compiled_shader(lv_gltf_data_t * data, size_t identifier, lv_gltf_compiled_shader_t * shader)
{
    /* Asserting this condition is true allows us to skip storing the identifier alongside the compiled shader*/
    LV_ASSERT(identifier == lv_array_size(&data->compiled_shaders) + 1);
    lv_array_push_back(&data->compiled_shaders, shader);
}

lv_gltf_compiled_shader_t * lv_gltf_get_compiled_shader(lv_gltf_data_t * data, size_t identifier)
{
    LV_ASSERT(identifier - 1 < lv_array_size(&data->compiled_shaders));
    return (lv_gltf_compiled_shader_t *)lv_array_at(&data->compiled_shaders, identifier - 1);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GLTF*/
