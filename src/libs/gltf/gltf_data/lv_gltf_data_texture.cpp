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
#include "../../../misc/lv_color.h"

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
    glDeleteTextures(data->skin_tex.size(), data->skin_tex.data());
    data->skin_tex.clear();
}

GLuint lv_gltf_data_create_texture(lv_gltf_model_t * data)
{
    GLuint texture;
    GL_CALL(glGenTextures(1, &texture));
    data->skin_tex.push_back(texture);
    return texture;
}

void lv_gltf_data_rgb_to_bgr(uint8_t * pixels, size_t byte_total_count, bool has_alpha)
{
    size_t bytes_per_pixel = has_alpha ? 4 : 3;
    size_t pixel_count = (byte_total_count / bytes_per_pixel);
    if(bytes_per_pixel == 4) {
        for(size_t p = 0; p < pixel_count; p++) {
            size_t index = p << 2;
            uint8_t r = pixels[index + 0];
            uint8_t g = pixels[index + 1];
            uint8_t b = pixels[index + 2];
            uint8_t a = pixels[index + 3];
            pixels[index + 0] = b;
            pixels[index + 1] = g;
            pixels[index + 2] = r;
            pixels[index + 3] = a;
        }
    }
    else {
        for(size_t p = 0; p < pixel_count; p++) {
            size_t index = p * 3;
            uint8_t r = pixels[index + 0];
            uint8_t g = pixels[index + 1];
            uint8_t b = pixels[index + 2];
            pixels[index + 0] = b;
            pixels[index + 1] = g;
            pixels[index + 2] = r;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GLTF*/
