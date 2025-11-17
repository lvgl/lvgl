/**
 * @file lv_gltf_ibl_sampler.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_environment_private.h"

#if LV_USE_GLTF

#include "../../../misc/lv_math.h"
#include "../../../misc/lv_log.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../stdlib/lv_string.h"
#include "../../../drivers/opengles/lv_opengles_private.h"
#include "../../../drivers/opengles/lv_opengles_debug.h"

#include "../../../drivers/opengles/opengl_shader/lv_opengl_shader_internal.h"
#include "../gltf_view/assets/lv_gltf_view_shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"

/*********************
 *      DEFINES
 *********************/

#define INTERNAL_FORMAT     GL_RGBA8
#define TEXTURE_TARGET_TYPE GL_UNSIGNED_BYTE

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void ibl_sampler_load(lv_gltf_ibl_sampler_t * sampler, const char * path);
static void ibl_sampler_filter(lv_gltf_ibl_sampler_t * sampler);
static void ibl_sampler_destroy(lv_gltf_ibl_sampler_t * sampler);
static bool ibl_gl_has_extension(const char * extension);
static void ibl_texture_from_image(lv_gltf_ibl_sampler_t * sampler, lv_gltf_ibl_texture_t * texture,
                                   const lv_gltf_ibl_image_t * image);
static GLuint ibl_load_texture_hdr(lv_gltf_ibl_sampler_t * sampler, const lv_gltf_ibl_image_t * image);
static GLuint ibl_create_cube_map_texture(const lv_gltf_ibl_sampler_t * sampler, bool with_mipmaps);
static uint32_t ibl_create_lut_texture(const lv_gltf_ibl_sampler_t * sampler);
static void ibl_panorama_to_cubemap(lv_gltf_ibl_sampler_t * sampler);
static void ibl_apply_filter(lv_gltf_ibl_sampler_t * sampler, uint32_t distribution, float roughness,
                             uint32_t target_mip_level, GLuint target_texture, uint32_t sample_count, float lod_bias);
static void ibl_cube_map_to_lambertian(lv_gltf_ibl_sampler_t * sampler);
static void ibl_cube_map_to_ggx(lv_gltf_ibl_sampler_t * sampler);
static void ibl_cube_map_to_sheen(lv_gltf_ibl_sampler_t * sampler);
static void ibl_sample_lut(lv_gltf_ibl_sampler_t * sampler, uint32_t distribution, uint32_t targetTexture,
                           uint32_t currentTextureSize);
static void ibl_sample_ggx_lut(lv_gltf_ibl_sampler_t * sampler);
static void ibl_sample_charlie_lut(lv_gltf_ibl_sampler_t * sampler);
static int ibl_count_bits(int value);

static void init_fullscreen_quad(lv_gltf_ibl_sampler_t * sampler);
static void draw_fullscreen_quad(lv_gltf_ibl_sampler_t * sampler, GLuint program_id);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_gltf_ibl_sampler_t * lv_gltf_ibl_sampler_create(void)
{
    lv_gltf_ibl_sampler_t * sampler = lv_zalloc(sizeof(*sampler));
    LV_ASSERT_MALLOC(sampler)
    if(!sampler) {
        LV_LOG_WARN("Failed to create sampler");
        return NULL;
    }

    sampler->cube_map_resolution = LV_GLTF_DEFAULT_CUBE_MAP_RESOLUTION;
    sampler->ggx_sample_count = 128;
    sampler->lambertian_sample_count = 256;
    sampler->sheen_sample_count = 32;
    sampler->lod_bias = 0.0;
    sampler->lowest_mip_level = 3;
    sampler->lut_resolution = 1024;
    sampler->lut_sample_count = 64;
    sampler->scale_value = 1.0;

    lv_opengl_shader_portions_t env_shader_portions;
    lv_gltf_view_shader_get_env(&env_shader_portions);
    lv_opengl_shader_manager_init(&sampler->shader_manager, env_shader_portions.all, env_shader_portions.count, NULL,
                                  NULL);
    init_fullscreen_quad(sampler);
    return sampler;
}

void lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(lv_gltf_ibl_sampler_t * sampler, uint32_t resolution)
{
    if(!sampler) {
        LV_LOG_WARN("Can't set cube map resolution on a NULL sampler");
        return;
    }
    if(resolution == 0) {
        LV_LOG_WARN("Cube map resolution should be > 0");
        return;
    }
    sampler->cube_map_resolution = resolution;
}

void lv_gltf_ibl_sampler_delete(lv_gltf_ibl_sampler_t * sampler)
{
    if(!sampler) {
        LV_LOG_WARN("Can't delete a NULL sampler");
        return;
    }

    ibl_sampler_destroy(sampler);
    lv_free(sampler);
}

void lv_gltf_environment_set_angle(lv_gltf_environment_t * env, float angle)
{
    if(!env) {
        LV_LOG_WARN("Can't set angle on a NULL environment");
        return;
    }
    env->angle = angle;
}

lv_gltf_environment_t * lv_gltf_environment_create(lv_gltf_ibl_sampler_t * sampler, const char * file_path)
{
    if(!sampler) {
        LV_LOG_WARN("Can't create an environment with a NULL sampler");
        return NULL;
    }

    lv_gltf_environment_t * env = lv_zalloc(sizeof(*env));
    LV_ASSERT_MALLOC(env);
    if(!env) {
        LV_LOG_WARN("Failed to create environment");
        return NULL;
    }
    ibl_sampler_load(sampler, file_path);
    ibl_sampler_filter(sampler);

    env->diffuse = sampler->lambertian_texture_id;
    env->specular = sampler->ggx_texture_id;
    env->sheen = sampler->sheen_texture_id;
    env->ggxLut = sampler->ggxlut_texture_id;
    env->charlie_lut = sampler->charlielut_texture_id;
    env->mip_count = sampler->mipmap_levels;
    env->ibl_intensity_scale = sampler->scale_value;
    return env;
}

void lv_gltf_environment_delete(lv_gltf_environment_t * env)
{
    const unsigned int d[3] = { env->diffuse, env->specular, env->sheen };
    GL_CALL(glDeleteTextures(3, d));
    lv_free(env);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ibl_sampler_load(lv_gltf_ibl_sampler_t * sampler, const char * path)
{
    // vv -- WebGL Naming
    if(ibl_gl_has_extension("GL_NV_float") && ibl_gl_has_extension("GL_ARB_color_buffer_float")) {
        LV_LOG_INFO("Device supports float format textures");
    }
    // Native naming #2
    if(ibl_gl_has_extension("GL_ARB_color_buffer_float") || ibl_gl_has_extension("GL_NV_half_float")) {
        LV_LOG_INFO("Device supports half_float format textures");
    }

    int32_t src_width, src_height, src_nrChannels;

    float * data;
    if(path != NULL) {
        data = stbi_loadf(path, &src_width, &src_height, &src_nrChannels, 3);
    }
    else {
        extern unsigned char chromatic_jpg[];
        extern unsigned int chromatic_jpg_len;
        data = stbi_loadf_from_memory(chromatic_jpg, chromatic_jpg_len, &src_width, &src_height, &src_nrChannels, 3);
    }

    {
        lv_gltf_ibl_image_t panorama_image = {
            .data = (float *)lv_malloc(src_width * src_height * 3 * sizeof(float)),
            .data_len = src_width * src_height * 3,
            .width = src_width,
            .height = src_height,
        };
        LV_ASSERT_MALLOC(panorama_image.data);

        lv_memcpy(panorama_image.data, data, panorama_image.data_len * sizeof(*panorama_image.data));
        stbi_image_free(data);
        sampler->input_texture_id = ibl_load_texture_hdr(sampler, &panorama_image);
        lv_free(panorama_image.data);
    }

    GL_CALL(glGenFramebuffers(1, &sampler->framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, sampler->framebuffer));

    sampler->cube_map_texture_id = ibl_create_cube_map_texture(sampler, true);
    sampler->lambertian_texture_id = ibl_create_cube_map_texture(sampler, false);
    sampler->ggx_texture_id = ibl_create_cube_map_texture(sampler, true);
    sampler->sheen_texture_id = ibl_create_cube_map_texture(sampler, true);

    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, sampler->ggx_texture_id));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, sampler->sheen_texture_id));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    sampler->mipmap_levels = ibl_count_bits(sampler->cube_map_resolution) + 1 - sampler->lowest_mip_level;
}

static void ibl_sampler_filter(lv_gltf_ibl_sampler_t * sampler)
{
    GLint prev_framebuffer;
    GL_CALL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_framebuffer));

    ibl_panorama_to_cubemap(sampler);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, prev_framebuffer));

    ibl_cube_map_to_lambertian(sampler);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, prev_framebuffer));

    ibl_cube_map_to_ggx(sampler);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, prev_framebuffer));

    ibl_cube_map_to_sheen(sampler);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, prev_framebuffer));

    ibl_sample_ggx_lut(sampler);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, prev_framebuffer));

    ibl_sample_charlie_lut(sampler);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, prev_framebuffer));
}
static void ibl_sampler_destroy(lv_gltf_ibl_sampler_t * sampler)
{
    GL_CALL(glDeleteBuffers(1, &sampler->fullscreen_vertex_buffer));
    GL_CALL(glDeleteBuffers(1, &sampler->fullscreen_tex_coord_buffer));
    lv_opengl_shader_manager_deinit(&sampler->shader_manager);
}

static void ibl_texture_from_image(lv_gltf_ibl_sampler_t * sampler, lv_gltf_ibl_texture_t * texture,
                                   const lv_gltf_ibl_image_t * image)
{
    const size_t src_format_bpp = 3;
    const size_t dst_format_bpp = 4;

    texture->internal_format = INTERNAL_FORMAT;
    texture->format = GL_RGBA;
    texture->type = TEXTURE_TARGET_TYPE;
    size_t pixel_num = image->data_len / src_format_bpp;
    texture->data = (uint8_t *)lv_malloc(pixel_num * 4);
    LV_ASSERT_MALLOC(texture->data);

    float max_value = 0.0;
    float clamped_sum = 0.0;
    float diff_sum = 0.0;
    size_t src = 0;
    size_t dst = 0;

    for(size_t i = 0; i < pixel_num; i++) {
        const float r = image->data[src + 0];
        const float g = image->data[src + 1];
        const float b = image->data[src + 2];
        const float max_component = LV_MAX(LV_MAX(r, g), b);

        if(max_component > 1.0) {
            diff_sum += max_component - 1.0;
        }
        clamped_sum += LV_MIN(max_component, 1.0f);
        max_value = LV_MAX(max_component, max_value);

        texture->data[dst + 0] = LV_MIN(r * 255, 255);
        texture->data[dst + 1] = LV_MIN(g * 255, 255);
        texture->data[dst + 2] = LV_MIN(b * 255, 255);
        texture->data[dst + 3] = 0xFF;

        src += src_format_bpp;
        dst += dst_format_bpp;
    }

    float scale_factor = 1.0;
    if(clamped_sum > 1.0) {
        // Apply global scale factor to compensate for intensity lost when clamping
        scale_factor = (clamped_sum + diff_sum) / clamped_sum;
        LV_LOG_INFO("HDR Intensity Scale %f\n", scale_factor);
    }

    sampler->scale_value = scale_factor;
}
static uint32_t ibl_load_texture_hdr(lv_gltf_ibl_sampler_t * sampler, const lv_gltf_ibl_image_t * image)
{
    lv_gltf_ibl_texture_t texture;
    ibl_texture_from_image(sampler, &texture, image);
    GLuint texture_id;
    GL_CALL(glGenTextures(1, &texture_id));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, // target
                         0, // level
                         texture.internal_format, image->width, image->height,
                         0, // border
                         texture.format, // format of the pixel data
                         texture.type, // type of the pixel data
                         texture.data));

    lv_free(texture.data);

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    return texture_id;
}

static GLuint ibl_create_cube_map_texture(const lv_gltf_ibl_sampler_t * sampler, bool with_mipmaps)
{
    uint32_t targetTexture;
    GL_CALL(glGenTextures(1, &targetTexture));
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, targetTexture));
    for(int32_t i = 0; i < 6; ++i) {
        GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, INTERNAL_FORMAT, sampler->cube_map_resolution,
                             sampler->cube_map_resolution, 0, GL_RGBA, TEXTURE_TARGET_TYPE, NULL));
    }
    if(with_mipmaps) {
        GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    }
    else {
        GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    }
    GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    return targetTexture;
}
static GLuint ibl_create_lut_texture(const lv_gltf_ibl_sampler_t * sampler)
{
    GLuint texture;
    GL_CALL(glGenTextures(1, &texture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, sampler->lut_resolution, sampler->lut_resolution, 0, GL_RGBA,
                         TEXTURE_TARGET_TYPE, NULL));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    return texture;
}
static void ibl_panorama_to_cubemap(lv_gltf_ibl_sampler_t * sampler)
{
    for(int32_t i = 0; i < 6; ++i) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, sampler->framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                       sampler->cube_map_texture_id, 0));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, sampler->cube_map_texture_id));
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        while(status != GL_FRAMEBUFFER_COMPLETE) {
            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            LV_LOG_ERROR("Environnement render error not complete. Expected %d. Got %d", GL_FRAMEBUFFER_COMPLETE,
                         status);
        }
        GL_CALL(glViewport(0, 0, sampler->cube_map_resolution, sampler->cube_map_resolution));
        GL_CALL(glClearColor(1.0, 0.0, 0.0, 0.0));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        uint32_t frag_shader_hash;
        uint32_t vert_shader_hash;
        lv_result_t res = lv_opengl_shader_manager_select_shader(&sampler->shader_manager, "panorama_to_cubemap.frag", NULL, 0,
                                                                 LV_OPENGL_GLSL_VERSION_300ES, &frag_shader_hash);
        LV_ASSERT(res == LV_RESULT_OK);
        res = lv_opengl_shader_manager_select_shader(&sampler->shader_manager, "fullscreen.vert", NULL, 0,
                                                     LV_OPENGL_GLSL_VERSION_300ES, &vert_shader_hash);
        LV_ASSERT(res == LV_RESULT_OK);
        lv_opengl_shader_program_t * program =
            lv_opengl_shader_manager_get_program(&sampler->shader_manager, frag_shader_hash, vert_shader_hash);

        LV_ASSERT_MSG(program != NULL,
                      "Failed to link program. This probably means your platform doesn't support GLSL version 300 es");

        GLuint program_id = lv_opengl_shader_program_get_id(program);

        GL_CALL(glUseProgram(program_id));
        GL_CALL(glActiveTexture(GL_TEXTURE0 + 0));
        // Bind texture ID to active texture
        GL_CALL(glBindTexture(GL_TEXTURE_2D, sampler->input_texture_id));
        // map shader uniform to texture unit (TEXTURE0)
        GLuint location;
        GL_CALL(location = glGetUniformLocation(program_id, "u_panorama"));
        GL_CALL(glUniform1i(location, 0));
        program->update_uniform_1i(program, "u_currentFace", i);
        //fullscreen triangle
        draw_fullscreen_quad(sampler, program_id);
    }

    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, sampler->cube_map_texture_id));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
}
static void ibl_apply_filter(lv_gltf_ibl_sampler_t * sampler, uint32_t distribution, float roughness,
                             uint32_t target_mip_level, GLuint target_texture, uint32_t sample_count, float lod_bias)
{
    uint32_t current_texture_size = sampler->cube_map_resolution >> target_mip_level;
    for(uint32_t i = 0; i < 6; ++i) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, sampler->framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                       target_texture, target_mip_level));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, target_texture));
        GL_CALL(glViewport(0, 0, current_texture_size, current_texture_size));
        GL_CALL(glClearColor(0.0, 1.0, 0.0, 0.0));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        uint32_t frag_shader_hash;
        uint32_t vert_shader_hash;
        lv_result_t res = lv_opengl_shader_manager_select_shader(&sampler->shader_manager, "ibl_filtering.frag", NULL, 0,
                                                                 LV_OPENGL_GLSL_VERSION_300ES, &frag_shader_hash);
        LV_ASSERT(res == LV_RESULT_OK);
        res = lv_opengl_shader_manager_select_shader(&sampler->shader_manager, "fullscreen.vert", NULL, 0,
                                                     LV_OPENGL_GLSL_VERSION_300ES, &vert_shader_hash);
        LV_ASSERT(res == LV_RESULT_OK);
        lv_opengl_shader_program_t * program =
            lv_opengl_shader_manager_get_program(&sampler->shader_manager, frag_shader_hash, vert_shader_hash);

        LV_ASSERT_MSG(program != NULL,
                      "Failed to link program. This probably means your platform doesn't support GLSL version 300 es");
        GLuint program_id = lv_opengl_shader_program_get_id(program);


        GL_CALL(glUseProgram(program_id));
        GL_CALL(glActiveTexture(GL_TEXTURE0));
        // Bind texture ID to active texture
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, sampler->cube_map_texture_id));
        // map shader uniform to texture unit (TEXTURE0)
        uint32_t location = glGetUniformLocation(program_id, "u_cubemapTexture");
        GL_CALL(glUniform1i(location, 0)); // texture unit 0
        program->update_uniform_1f(program, "u_roughness", roughness);
        program->update_uniform_1i(program, "u_sampleCount", sample_count);
        /* Software rendered mode looks better with this and horrible with below */
        /*program->update_uniform_1i(program, "u_width", current_texture_size);  */
        /* Standard mode looks best with this and somewhat worse with above */
        program->update_uniform_1i(program, "u_width", sampler->cube_map_resolution);
        program->update_uniform_1f(program, "u_lodBias", lod_bias);
        program->update_uniform_1i(program, "u_distribution", distribution);
        program->update_uniform_1i(program, "u_currentFace", i);
        program->update_uniform_1i(program, "u_isGeneratingLUT", 0);
        program->update_uniform_1i(program, "u_floatTexture", 0);
        program->update_uniform_1f(program, "u_intensityScale", sampler->scale_value);
        //fullscreen triangle
        draw_fullscreen_quad(sampler, program_id);
    }
}
static void ibl_cube_map_to_lambertian(lv_gltf_ibl_sampler_t * sampler)
{
    ibl_apply_filter(sampler, 0, 0.0, 0, sampler->lambertian_texture_id, sampler->lambertian_sample_count, 0.0);
}
static void ibl_cube_map_to_ggx(lv_gltf_ibl_sampler_t * sampler)
{
    LV_ASSERT(sampler->mipmap_levels != 1);
    for(uint32_t current_mip_level = 0; current_mip_level <= sampler->mipmap_levels; ++current_mip_level) {
        float roughness = (current_mip_level) / (float)(sampler->mipmap_levels - 1);
        ibl_apply_filter(sampler, 1, roughness, current_mip_level, sampler->ggx_texture_id, sampler->ggx_sample_count,
                         0.0);
    }
}
static void ibl_cube_map_to_sheen(lv_gltf_ibl_sampler_t * sampler)
{
    LV_ASSERT(sampler->mipmap_levels != 1);
    for(uint32_t current_mip_level = 0; current_mip_level <= sampler->mipmap_levels; ++current_mip_level) {
        float roughness = (current_mip_level) / (float)(sampler->mipmap_levels - 1);
        ibl_apply_filter(sampler, 2, roughness, current_mip_level, sampler->sheen_texture_id,
                         sampler->sheen_sample_count, 0.0);
    }
}
static void ibl_sample_lut(lv_gltf_ibl_sampler_t * sampler, uint32_t distribution, uint32_t targetTexture,
                           uint32_t currentTextureSize)
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, sampler->framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture, 0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, targetTexture));
    GL_CALL(glViewport(0, 0, currentTextureSize, currentTextureSize));
    GL_CALL(glClearColor(0.0, 1.0, 1.0, 0.0));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    uint32_t frag_shader;
    uint32_t vert_shader;
    lv_result_t res    = lv_opengl_shader_manager_select_shader(&sampler->shader_manager, "ibl_filtering.frag", NULL, 0,
                                                                LV_OPENGL_GLSL_VERSION_300ES, &frag_shader);
    LV_ASSERT(res == LV_RESULT_OK);
    res = lv_opengl_shader_manager_select_shader(&sampler->shader_manager, "fullscreen.vert", NULL, 0,
                                                 LV_OPENGL_GLSL_VERSION_300ES, &vert_shader);
    LV_ASSERT(res == LV_RESULT_OK);
    lv_opengl_shader_program_t * program = lv_opengl_shader_manager_get_program(&sampler->shader_manager, frag_shader,
                                                                                vert_shader);
    LV_ASSERT_MSG(program != NULL,
                  "Failed to link program. This probably means your platform doesn't support GLSL version 300 es");

    GLuint program_id = lv_opengl_shader_program_get_id(program);

    GL_CALL(glUseProgram(program_id));
    //  TEXTURE0 = active.
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 0));
    // Bind texture ID to active texture
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, sampler->cube_map_texture_id));
    // map shader uniform to texture unit (TEXTURE0)
    uint32_t location = glGetUniformLocation(program_id, "u_cubemapTexture");
    GL_CALL(glUniform1i(location, 0)); // texture unit 0
    program->update_uniform_1f(program, "u_roughness", 0.0);
    program->update_uniform_1i(program, "u_sampleCount", sampler->lut_sample_count);
    //shader->update_uniform_1i( shader, "u_sampleCount", 512);
    program->update_uniform_1i(program, "u_width", 0.0);
    program->update_uniform_1f(program, "u_lodBias", 0.0);
    program->update_uniform_1i(program, "u_distribution", distribution);
    program->update_uniform_1i(program, "u_currentFace", 0);
    program->update_uniform_1i(program, "u_isGeneratingLUT", 1);
    //fullscreen triangle
    draw_fullscreen_quad(sampler, program_id);
}
static void ibl_sample_ggx_lut(lv_gltf_ibl_sampler_t * sampler)
{
    sampler->ggxlut_texture_id = ibl_create_lut_texture(sampler);
    ibl_sample_lut(sampler, 1, sampler->ggxlut_texture_id, sampler->lut_resolution);
}
static void ibl_sample_charlie_lut(lv_gltf_ibl_sampler_t * sampler)
{
    sampler->charlielut_texture_id = ibl_create_lut_texture(sampler);
    ibl_sample_lut(sampler, 2, sampler->charlielut_texture_id, sampler->lut_resolution);
}

static bool ibl_gl_has_extension(const char * extension)
{
    const GLubyte * extensions = glGetString(GL_EXTENSIONS);
    if(!extensions) {
        return false;
    }

    const char * ext_str = (const char *)extensions;
    const char * current = ext_str;
    const char * next;

    while(*current) {
        /* Find the next space or end of string */
        next = strchr(current, ' ');
        if(next) {
            size_t length = next - current;
            if(length == strlen(extension) && strncmp(current, extension, length) == 0) {
                return true;
            }
            current = next + 1;
        }
        else {
            /* Last extension (no space found) */
            if(strcmp(current, extension) == 0) {
                return true;
            }
            break;
        }
    }
    return false;
}

static int ibl_count_bits(int value)
{
    int count = 0;
    while(value > 1) {
        value >>= 1;
        count++;
    }
    return count;
}

static void init_fullscreen_quad(lv_gltf_ibl_sampler_t * sampler)
{
    /* Vertices go from -1 -1 (left bottom) to 1 1 (right top)*/
    GLfloat vertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f
    };

    /* Texture coords go from 0 0 (left botton) to 1 1 (right top)*/
    GLfloat texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    GL_CALL(glGenBuffers(1, &sampler->fullscreen_vertex_buffer));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, sampler->fullscreen_vertex_buffer));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GL_CALL(glGenBuffers(1, &sampler->fullscreen_tex_coord_buffer));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, sampler->fullscreen_tex_coord_buffer));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW));
}

void draw_fullscreen_quad(lv_gltf_ibl_sampler_t * sampler, GLuint program_id)
{
    GLuint positionAttrib = glGetAttribLocation(program_id, "aPosition");
    GL_CALL(glEnableVertexAttribArray(positionAttrib));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, sampler->fullscreen_vertex_buffer));
    GL_CALL(glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void *)0));

    GLuint texCoordAttrib = glGetAttribLocation(program_id, "aTexCoord");
    GL_CALL(glEnableVertexAttribArray(texCoordAttrib));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, sampler->fullscreen_tex_coord_buffer));
    GL_CALL(glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void *)0));

    GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

    GL_CALL(glDisableVertexAttribArray(positionAttrib));
    GL_CALL(glDisableVertexAttribArray(texCoordAttrib));
}

#endif /*LV_USE_GLTF*/
