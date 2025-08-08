#ifndef LV_GLTF_MODEL_H
#define LV_GLTF_MODEL_H

#include "../../../lv_conf_internal.h"
#if LV_USE_GLTF

#include "../../../misc/lv_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Get the number of images in the glTF model
 *
 * Images in glTF are used as sources for textures and can be stored either as external files
 * or embedded as base64-encoded model within the glTF file.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of images in the model
 */
size_t lv_gltf_model_get_image_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of textures in the glTF model
 *
 * Textures define how images are sampled and applied to materials. Each texture references
 * an image and may specify sampling parameters like filtering and wrapping modes.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of textures in the model
 */
size_t lv_gltf_model_get_texture_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of materials in the glTF model
 *
 * Materials define the visual appearance of mesh primitives, including properties like
 * base color, metallic/roughness values, normal maps, and other surface characteristics.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of materials in the model
 */
size_t lv_gltf_model_get_material_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of cameras in the glTF model
 *
 * Cameras define viewpoints within the 3D scene and can be either perspective or
 * orthographic. They are typically attached to nodes in the scene graph.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of cameras in the model
 */
size_t lv_gltf_model_get_camera_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of nodes in the glTF model
 *
 * Nodes form the scene graph hierarchy and can contain transformations, meshes, cameras,
 * or other nodes as children. They define the spatial relationships between objects in the scene.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of nodes in the model
 */
size_t lv_gltf_model_get_node_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of meshes in the glTF model
 *
 * Meshes contain the geometric model for 3D objects, including vertex positions, normals,
 * texture coordinates, and indices. Each mesh can have multiple primitives with different materials.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of meshes in the model
 */
size_t lv_gltf_model_get_mesh_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of scenes in the glTF model
 *
 * Scenes define the root nodes of the scene graph. A glTF file can contain multiple scenes,
 * though typically only one is designated as the default scene to be displayed.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of scenes in the model
 */
size_t lv_gltf_model_get_scene_count(const lv_gltf_model_t * model);

/**
 * @brief Get the number of animations in the glTF model
 *
 * Animations define keyframe-based motion for nodes in the scene, including transformations
 * like translation, rotation, and scaling over time.
 *
 * @param model Pointer to the glTF model data structure
 * @return Number of animations in the model
 */
size_t lv_gltf_model_get_animation_count(const lv_gltf_model_t * model);

/**
 * @brief Select and start playing an animation
 *
 * @param model Pointer to the glTF model structure
 * @param index Animation number to start playing
 * @return LV_RESULT_OK if the animation was started else LV_RESULT_INVALID
 */
lv_result_t lv_gltf_model_play_animation(lv_gltf_model_t * model, size_t index);

/**
 * @brief Pause the current animation
 *
 * @param model Pointer to the glTF model structure
 */
void lv_gltf_model_pause_animation(lv_gltf_model_t * model);

/**
 * @brief Check if an animation is currently being played
 *
 * @param model Pointer to the glTF model structure
 */
bool lv_gltf_model_is_animation_paused(lv_gltf_model_t * model);

/**
 * @brief Get the current selected animation. To see if it's playing see `lv_gltf_model_is_animation_paused`
 *
 * @param model Pointer to the glTF model structure
 */
size_t lv_gltf_model_get_animation(lv_gltf_model_t * model);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_GLTF*/
#endif /*LV_GLTF_MODEL_H*/
