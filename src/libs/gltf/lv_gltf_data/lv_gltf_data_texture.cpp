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

void lv_gltf_data_destroy_textures(lv_gltf_model_t * data)
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

bool lv_gltf_data_get_texture_pixels(uint8_t * pixels, lv_gltf_model_t * data_obj, uint32_t model_texture_index,
                                     uint32_t mipmapnum,
                                     uint32_t width, uint32_t height, bool has_alpha)
{
    // This parameter is specified because WebGL can't read a texture's width from the GPU, however this isn't yet implemented so for now it either uses the GPU or it fails.
    LV_UNUSED(width);
    LV_UNUSED(height);

    if(model_texture_index >= data_obj->textures.size()) {
        return false;
    }
    GLuint texid = data_obj->textures[model_texture_index];
    // Bind the texture
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texid));
    GL_CALL(glGetTexImage(GL_TEXTURE_2D, mipmapnum, (has_alpha) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    return true;
}

void lv_gltf_data_texture_to_image_dsc(lv_image_dsc_t * new_image_dsc, lv_gltf_model_t * data_obj,
                                       uint32_t model_texture_index)
{
    size_t byte_total_count = 0;
    uint32_t source_pixel_width = 0;
    uint32_t source_pixel_height = 0;
    bool has_alpha = false;
    uint8_t * pixel_buffer;
    if(lv_gltf_data_get_texture_info(data_obj, model_texture_index, 0, &byte_total_count, &source_pixel_width,
                                     &source_pixel_height, &has_alpha)) {
        pixel_buffer = (uint8_t *)lv_malloc(byte_total_count);
        if(lv_gltf_data_get_texture_pixels(pixel_buffer, data_obj, model_texture_index, 0, source_pixel_width,
                                           source_pixel_height, has_alpha)) {
            if(pixel_buffer == NULL || byte_total_count == 0 || source_pixel_width == 0)
                return;

            if(new_image_dsc->data_size > 0) {
                lv_free((uint8_t *)new_image_dsc->data);
                new_image_dsc->data = NULL;
                new_image_dsc->data_size = 0;
            }

            lv_gltf_data_rgb_to_bgr(pixel_buffer, byte_total_count, has_alpha);
            size_t bytes_per_pixel = has_alpha ? 4 : 3;
            size_t pixel_count = (byte_total_count / bytes_per_pixel);

            new_image_dsc->data = (const uint8_t *)pixel_buffer;
            new_image_dsc->data_size = byte_total_count;
            new_image_dsc->header.w = source_pixel_width;
            new_image_dsc->header.h = (uint16_t)(pixel_count / source_pixel_width);
            new_image_dsc->header.cf = has_alpha ? LV_COLOR_FORMAT_ARGB8888 : LV_COLOR_FORMAT_RGB888;
        }
    }
}

bool lv_gltf_data_get_texture_info(lv_gltf_model_t * data_obj, uint32_t model_texture_index, uint32_t mipmapnum,
                                   size_t * byte_count,
                                   uint32_t * width, uint32_t * height, bool * has_alpha)
{
    *byte_count = 0;
    if(model_texture_index >= data_obj->textures.size()) {
        return false;
    }
    GLuint texid = data_obj->textures[model_texture_index];
    // Bind the texture
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texid));
    int32_t gl_color_format;
    GL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &gl_color_format));

    // Determine if the texture has an alpha channel
    *has_alpha = false;
    bool not_valid = false;

    switch(gl_color_format) {
        case GL_RGBA:
        case GL_BGRA:
        case GL_RGBA8:
            *has_alpha = true;
            break;
        case GL_RGB:
        case GL_BGR:
        case GL_RGB8:
            *has_alpha = false;
            break;
        default:
            LV_LOG_ERROR("Unhandled texture color format %d", gl_color_format);
            not_valid = true;
            break;
    }
    // even if the pixel format is invalid, we can still get the width and height
    GLint texture_width;
    GLint texture_height;
    GL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmapnum, GL_TEXTURE_WIDTH, &texture_width));
    GL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, mipmapnum, GL_TEXTURE_HEIGHT, &texture_height));
    *width = (uint32_t)(texture_width);
    *height = (uint32_t)(texture_height);

    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    if(not_valid) {
        return false;
    }

    *byte_count = texture_width * texture_height * (*has_alpha ? 4 : 3);
    return true;
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
