/**
 * @file lv_gltf_data_shader.cpp
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

GLuint lv_gltf_data_get_shader_program(lv_gltf_data_t * data, size_t I)
{
    return data->shader_sets[I].program;
}

lv_gltf_renwin_shaderset_t * lv_gltf_data_get_shader_set(lv_gltf_data_t * data, size_t index)
{
    return &data->shader_sets[index];
}

lv_gltf_uniform_locations_t * get_uniform_ids(lv_gltf_data_t * data, size_t I)
{
    return &data->shader_uniforms[I];
}

void lv_gltf_data_set_shader(lv_gltf_data_t * data, size_t index, lv_gltf_uniform_locations_t uniforms,
                             lv_gltf_renwin_shaderset_t shaderset)
{
    data->shader_uniforms[index] = uniforms;
    data->shader_sets[index] = shaderset;
}

void lv_gltf_data_init_shaders(lv_gltf_data_t * data, uint64_t _max_index)
{
    auto _prevsize = data->shader_sets.size();
    data->shader_sets.resize(_max_index + 1);
    data->shader_uniforms.resize(_max_index + 1);
    if(_prevsize < _max_index) {
        for(uint64_t _ii = _prevsize; _ii <= _max_index; _ii++) {
            data->shader_sets[_ii] = lv_gltf_renwin_shaderset_t();
            data->shader_sets[_ii].ready = false;
        }
    }
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GLTF*/
