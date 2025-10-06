/**
 * @file lv_gltf_ibl_sampler.h
 *
 */

#ifndef LV_GLTF_ENVIRONMENT_H
#define LV_GLTF_ENVIRONMENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an IBL sampler for processing environment images
 * @param texture_size resolution of each cube map face (recommended: 128-512 for embedded)
 * @return pointer to the created sampler, or NULL on failure
 * @note Can be safely deleted after environments are created
 */
lv_gltf_ibl_sampler_t * lv_gltf_ibl_sampler_create(uint32_t texture_size);

/**
 * Delete an IBL sampler
 * @param sampler pointer to the sampler to delete
 */
void lv_gltf_ibl_sampler_delete(lv_gltf_ibl_sampler_t * sampler);

/**
 * Create an environment from an HDR or JPEG panoramic image for IBL rendering
 * @param sampler IBL sampler defining output resolution (can be deleted after this call)
 * @param file_path path to equirectangular environment image, or NULL to use default embedded image
 * @return pointer to the created environment, or NULL on failure
 * @note The source image will be downsampled to the sampler's texture_size
 * @note The environment can be shared across multiple glTF objects
 */
lv_gltf_environment_t * lv_gltf_environment_create(lv_gltf_ibl_sampler_t * sampler, const char * file_path);

/**
 * Set the rotation angle of the environment map
 * @param env pointer to the environment
 * @param angle rotation angle in degrees
 */
void lv_gltf_environment_set_angle(lv_gltf_environment_t * env, float angle);

/**
 * Delete an environment
 * @param environment pointer to the environment to delete
 */
void lv_gltf_environment_delete(lv_gltf_environment_t * environment);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTF_ENVIRONMENT_H*/


