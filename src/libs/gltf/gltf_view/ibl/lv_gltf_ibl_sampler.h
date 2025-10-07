/**
 * @file lv_gltf_ibl_sampler.h
 *
 */

#ifndef LV_GLTF_IBL_SAMPLER_H
#define LV_GLTF_IBL_SAMPLER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lv_conf_internal.h"

#if LV_USE_GLTF
#include "../../../../misc/lv_types.h"
#include "../../../../drivers/opengles/opengl_shader/lv_opengl_shader_internal.h"
#include "../lv_gltf_view_internal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t texture_size;
    float lod_bias;
    uint32_t lowest_mip_level;
    uint32_t input_texture_id;
    uint32_t cubemap_texture_id;
    uint32_t framebuffer;
    uint32_t mipmap_count;

    uint32_t lambertian_texture_id;
    uint32_t lambertian_sample_count;

    uint32_t ggx_sample_count;
    uint32_t ggx_texture_id;

    uint32_t sheen_texture_id;
    uint32_t sheen_sample_count;

    uint32_t ggxlut_texture_id;

    uint32_t lut_sample_count;
    uint32_t lut_resolution;

    uint32_t charlielut_texture_id;

    float scale_value;
    uint32_t mipmap_levels;

    lv_opengl_shader_manager_t shader_manager;

    uint32_t fullscreen_vertex_buffer;
    uint32_t fullscreen_tex_coord_buffer;

} lv_gltf_ibl_sampler_t;

typedef struct {
    uint8_t * data;
    uint32_t internal_format;
    uint32_t format;
    uint32_t type;
} lv_gltf_ibl_texture_t;

typedef struct {
    float * data;
    size_t data_len;
    uint32_t width;
    uint32_t height;
} lv_gltf_ibl_image_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_gltf_ibl_generate_env_textures(lv_gltf_view_env_textures_t * env, const char * env_file_path,
                                       float env_rotation);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTF_IBL_SAMPLER_H*/
