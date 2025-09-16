#ifndef LV_GLTFDATA_PRIVATE_H
#define LV_GLTFDATA_PRIVATE_H

#include "../../../lv_conf_internal.h"
#if LV_USE_GLTF
#include "../../../drivers/opengles/opengl_shader/lv_opengl_shader_internal.h"
#include "../../../draw/lv_image_dsc.h"
#include "../../../misc/lv_types.h"

typedef struct {
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLint baseVertex;
    GLuint baseInstance;
} IndirectDrawCommand;

typedef struct {
    IndirectDrawCommand draw;
    GLenum primitiveType;
    GLenum indexType;
    GLuint vertexArray;

    GLuint vertexBuffer;
    GLuint indexBuffer;

    GLuint materialUniformsIndex;
    GLuint albedoTexture;
    GLuint emissiveTexture;
    GLuint metalRoughTexture;
    GLuint occlusionTexture;
    GLuint normalTexture;
    GLuint diffuseTransmissionTexture;
    GLuint diffuseTransmissionColorTexture;
    GLuint transmissionTexture;
    GLuint transmissionTexcoordIndex;

    GLint baseColorTexcoordIndex;
    GLint emissiveTexcoordIndex;

    GLint metallicRoughnessTexcoordIndex;
    GLint occlusionTexcoordIndex;
    GLint normalTexcoordIndex;
    GLint diffuseTransmissionTexcoordIndex;
    GLint diffuseTransmissionColorTexcoordIndex;

    GLint clearcoatTexture;
    GLint clearcoatRoughnessTexture;
    GLint clearcoatNormalTexture;
    GLint clearcoatTexcoordIndex;
    GLint clearcoatRoughnessTexcoordIndex;
    GLint clearcoatNormalTexcoordIndex;

    GLuint thicknessTexture;
    GLint thicknessTexcoordIndex;

    GLuint diffuseTexture;
    GLint diffuseTexcoordIndex;

    GLuint specularGlossinessTexture;
    GLint specularGlossinessTexcoordIndex;

} lv_gltf_primitive_t;

typedef struct {
    GLint camera;
    GLint view_projection_matrix;
    GLint model_matrix;
    GLint view_matrix;
    GLint projection_matrix;

    GLint env_intensity;
    GLint env_diffuse_sampler;
    GLint env_specular_sampler;
    GLint env_sheen_sampler;
    GLint env_ggx_lut_sampler;
    GLint env_charlie_lut_sampler;
    GLint env_mip_count;

    GLint exposure;
    GLint roughness_factor;

    GLint base_color_factor;
    GLint base_color_sampler;
    GLint base_color_uv_set;
    GLint base_color_uv_transform;

    GLint emissive_factor;
    GLint emissive_sampler;
    GLint emissive_uv_set;
    GLint emissive_uv_transform;
    GLint emissive_strength;

    GLint metallic_factor;
    GLint metallic_roughness_sampler;
    GLint metallic_roughness_uv_set;
    GLint metallic_roughness_uv_transform;

    GLint occlusion_strength;
    GLint occlusion_sampler;
    GLint occlusion_uv_set;
    GLint occlusion_uv_transform;

    GLint normal_scale;
    GLint normal_sampler;
    GLint normal_uv_set;
    GLint normal_uv_transform;

    GLint clearcoat_factor;
    GLint clearcoat_roughness_factor;
    GLint clearcoat_sampler;
    GLint clearcoat_uv_set;
    GLint clearcoat_uv_transform;
    GLint clearcoat_roughness_sampler;
    GLint clearcoat_roughness_uv_set;
    GLint clearcoat_roughness_uv_transform;
    GLint clearcoat_normal_scale;
    GLint clearcoat_normal_sampler;
    GLint clearcoat_normal_uv_set;
    GLint clearcoat_normal_uv_transform;

    GLint thickness;
    GLint thickness_sampler;
    GLint thickness_uv_set;
    GLint thickness_uv_transform;

    GLint diffuse_transmission_sampler;
    GLint diffuse_transmission_uv_set;
    GLint diffuse_transmission_uv_transform;

    GLint diffuse_transmission_color_sampler;
    GLint diffuse_transmission_color_uv_set;
    GLint diffuse_transmission_color_uv_transform;

    GLint sheen_color_factor;
    GLint sheen_roughness_factor;

    GLint specular_color_factor;
    GLint specular_factor;

    GLint diffuse_transmission_color_factor;
    GLint diffuse_transmission_factor;

    GLint ior;
    GLint alpha_cutoff;

    GLint dispersion;
    GLint screen_size;
    GLint transmission_factor;
    GLint transmission_sampler;
    GLint transmission_uv_set;
    GLint transmission_uv_transform;
    GLint transmission_framebuffer_sampler;
    GLint transmission_framebuffer_size;

    GLint attenuation_distance;
    GLint attenuation_color;

    GLint joints_sampler;

    GLint diffuse_factor;
    GLint glossiness_factor;

    GLint diffuse_sampler;
    GLint diffuse_uv_set;
    GLint diffuse_uv_transform;
    GLint specular_glossiness_sampler;
    GLint specular_glossiness_uv_set;
    GLint specular_glossiness_uv_transform;

} lv_gltf_uniform_locations_t;

lv_gltf_uniform_locations_t lv_gltf_uniform_locations_create(GLuint program);

typedef struct {
    GLuint program;
    uint32_t bg_program;
    uint32_t vert;
    uint32_t frag;
} lv_gltf_shaderset_t;

typedef struct {
    lv_gltf_uniform_locations_t uniforms;
    lv_gltf_shaderset_t shaderset;
} lv_gltf_compiled_shader_t;

void lv_gltf_store_compiled_shader(lv_gltf_model_t * data, size_t identifier, lv_gltf_compiled_shader_t * shader);
lv_gltf_compiled_shader_t * lv_gltf_get_compiled_shader(lv_gltf_model_t * data, size_t identifier);

/**
 * @brief Load the gltf file at the specified filepath
 *
 * @param gltf_path The gltf filename
 * @param ret_data Pointer to the data container that will be populated.
 * @param shaders Pointer to the shader cache object this file uses.
 */
lv_gltf_model_t *
lv_gltf_data_load_from_file(const char * file_path,
                            lv_opengl_shader_manager_t * shader_manager);

/**
 * @brief Load the gltf file encoded within the supplied byte array
 *
 * @param gltf_path The gltf filename
 * @param gltf_data_size if gltf_path is instead a byte array, pass the size of that array in through this variable (or 0 if it's a file path).
 * @param ret_data Pointer to the data container that will be populated.
 * @param shaders Pointer to the shader cache object this file uses.
 */

lv_gltf_model_t *
lv_gltf_data_load_from_bytes(const uint8_t * data, size_t data_size,
                             lv_opengl_shader_manager_t * shader_manager);


/**
 * @brief Retrieve the radius of the GLTF data object.
 *
 * @param D Pointer to the lv_gltf_data_t object from which to get the radius.
 * @return The radius of the GLTF data object.
 */
double lv_gltf_data_get_radius(lv_gltf_model_t * D);


/**
 * @brief Destroy a GLTF data object and free associated resources.
 *
 * @param _data Pointer to the lv_gltf_data_t object to be destroyed.
 */
void lv_gltf_data_destroy(lv_gltf_model_t * _data);

/**
 * @brief Copy the bounds information from one GLTF data object to another.
 *
 * @param to Pointer to the destination lv_gltf_data_t object.
 * @param from Pointer to the source lv_gltf_data_t object.
 */
void lv_gltf_data_copy_bounds_info(lv_gltf_model_t * to, lv_gltf_model_t * from);

/**
 * @brief Swap the red and blue channels in a pixel buffer.
 *
 * @param pixel_buffer Pointer to the pixel buffer containing the image data.
 * @param byte_total_count The total number of bytes in the pixel buffer.
 * @param has_alpha Flag indicating whether the pixel buffer includes an alpha channel.
 */
void lv_gltf_data_rgb_to_bgr(uint8_t * pixel_buffer,
                             size_t byte_total_count,
                             bool has_alpha);


#endif /*LV_USE_GLTF*/
#endif /* LV_GLTFDATA_PRIVATE_H */
