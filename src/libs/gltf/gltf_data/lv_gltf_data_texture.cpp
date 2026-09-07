/**
 * @file lv_gltf_data_texture.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"
#if LV_USE_GLTF

#include <cstdint>

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

void lv_gltf_data_delete_textures(lv_gltf_model_t * data)
{
    LV_ASSERT(data != NULL);
    glDeleteTextures(data->skin_tex.size(), data->skin_tex.data());
    data->skin_tex.clear();
}

GLuint lv_gltf_data_create_texture(lv_gltf_model_t * data)
{
    LV_ASSERT(data != NULL);
    GLuint texture;
    GL_CALL(glGenTextures(1, &texture));
    data->skin_tex.push_back(texture);
    return texture;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GLTF*/
