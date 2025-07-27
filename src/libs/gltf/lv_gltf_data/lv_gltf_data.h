#ifndef LV_GLTFDATA_H
#define LV_GLTFDATA_H

#include "../../../lv_conf_internal.h"
#if LV_USE_GLTF
#include "../../../misc/lv_types.h"
#include "../../../draw/lv_image_dsc.h"
#include "../lv_gl_shader/lv_gl_shader_internal.h"

#ifdef __cplusplus
extern "C" {
#endif


size_t lv_gltf_data_get_image_count(const lv_gltf_data_t * data);
size_t lv_gltf_data_get_texture_count(const lv_gltf_data_t * data);
size_t lv_gltf_data_get_material_count(const lv_gltf_data_t * data);
size_t lv_gltf_data_get_camera_count(const lv_gltf_data_t * data);
size_t lv_gltf_data_get_node_count(const lv_gltf_data_t * data);
size_t lv_gltf_data_get_mesh_count(const lv_gltf_data_t * data);
size_t lv_gltf_data_get_scene_count(const lv_gltf_data_t * data);
size_t lv_gltf_data_get_animation_count(const lv_gltf_data_t * data);

/**
 * @brief Load the gltf file at the specified filepath
 *
 * @param gltf_path The gltf filename
 * @param ret_data Pointer to the data container that will be populated.
 * @param shaders Pointer to the shader cache object this file uses.
 */

lv_gltf_data_t *
lv_gltf_data_load_from_file(const char * file_path,
                            lv_gl_shader_manager_t * shader_manager);

/**
 * @brief Load the gltf file encoded within the supplied byte array
 *
 * @param gltf_path The gltf filename
 * @param gltf_data_size if gltf_path is instead a byte array, pass the size of that array in through this variable (or 0 if it's a file path).
 * @param ret_data Pointer to the data container that will be populated.
 * @param shaders Pointer to the shader cache object this file uses.
 */

lv_gltf_data_t *
lv_gltf_data_load_from_bytes(const uint8_t * data, size_t data_size,
                             lv_gl_shader_manager_t * shader_manager);


/**
 * @brief Retrieve the radius of the GLTF data object.
 *
 * @param D Pointer to the lv_gltf_data_t object from which to get the radius.
 * @return The radius of the GLTF data object.
 */
double lv_gltf_data_get_radius(lv_gltf_data_t * D);


/**
 * @brief Destroy a GLTF data object and free associated resources.
 *
 * @param _data Pointer to the lv_gltf_data_t object to be destroyed.
 */
void lv_gltf_data_destroy(lv_gltf_data_t * _data);

/**
 * @brief Copy the bounds information from one GLTF data object to another.
 *
 * @param to Pointer to the destination lv_gltf_data_t object.
 * @param from Pointer to the source lv_gltf_data_t object.
 */
void lv_gltf_data_copy_bounds_info(lv_gltf_data_t * to, lv_gltf_data_t * from);

/**
 * @brief Retrieve information about a specific texture in a GLTF model.
 *
 * @param data_obj Pointer to the lv_gltf_data_t object containing the model data.
 * @param model_texture_index The index of the texture in the model.
 * @param mipmapnum The mipmap level to retrieve information for.
 * @param byte_count Pointer to a size_t variable to store the byte count of the texture.
 * @param width Pointer to a uint32_t variable to store the width of the texture.
 * @param height Pointer to a uint32_t variable to store the height of the texture.
 * @param has_alpha Pointer to a bool variable to indicate if the texture has an alpha channel.
 * @return True if the texture information was successfully retrieved, false otherwise.
 */
bool lv_gltf_data_get_texture_info(lv_gltf_data_t * data_obj,
                                   uint32_t model_texture_index,
                                   uint32_t mipmapnum, size_t * byte_count,
                                   uint32_t * width, uint32_t * height,
                                   bool * has_alpha);

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

/**
 * @brief Convert a texture from a GLTF model to an image descriptor.
 *
 * @param new_image_dsc Pointer to the lv_image_dsc_t structure to be populated with the image data.
 * @param data_obj Pointer to the lv_gltf_data_t object containing the model data.
 * @param model_texture_index The index of the texture in the model to convert.
 */
void lv_gltf_data_texture_to_image_dsc(lv_image_dsc_t * new_image_dsc,
                                       lv_gltf_data_t * data_obj,
                                       uint32_t model_texture_index);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_GLTF*/
#endif /*LV_GLTFVIEW_H*/
