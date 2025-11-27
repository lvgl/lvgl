/**
 * @file lv_gltf_model_node.h
 *
 */

#ifndef LV_GLTF_MODEL_NODE_H
#define LV_GLTF_MODEL_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../../../misc/lv_types.h"
#include "../math/lv_3dmath.h"
#include "../../../misc/lv_event.h"

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
 * @brief Get a glTF model node by its index
 *
 * @param data Pointer to the glTF model structure
 * @param index The index of the node to retrieve
 * @return Pointer to the glTF model node, or NULL if not found
 */
lv_gltf_model_node_t * lv_gltf_model_node_get_by_index(lv_gltf_model_t * data, size_t index);

/**
 * @brief Get a glTF model node by its numeric path
 *
 * @param data Pointer to the glTF model structure
 * @param num_path The numeric path string of the node to retrieve (eg. ".0")
 * @return Pointer to the glTF model node, or NULL if not found
 */
lv_gltf_model_node_t * lv_gltf_model_node_get_by_numeric_path(lv_gltf_model_t * data, const char * num_path);

/**
 * @brief Get a glTF model node by its path
 *
 * @param data Pointer to the glTF model structure
 * @param path The path string of the node to retrieve
 * @return Pointer to the glTF model node, or NULL if not found
 */
lv_gltf_model_node_t * lv_gltf_model_node_get_by_path(lv_gltf_model_t * data, const char * path);

/**
 * @brief Get the path of a glTF model node
 *
 * @param node Pointer to the glTF model node structure
 * @return The path string of the node, or NULL if node is invalid
 */
const char * lv_gltf_model_node_get_path(lv_gltf_model_node_t * node);

/**
 * @brief Get the IP (internal pointer/identifier) of a glTF model node
 *
 * @param node Pointer to the glTF model node structure
 * @return The IP string of the node, or NULL if node is invalid
 */
const char * lv_gltf_model_node_get_ip(lv_gltf_model_node_t * node);

/**
 * @brief Add an event callback to a glTF model node
 *
 * @param node Pointer to the glTF model node structure
 * @param cb The event callback function to add. Use lv_event_get_param() to retrieve lv_gltf_node_data_t with the node's data.
 * @param filter_list Event code filter for the callback
 * @param user_data User data to pass to the callback
 * @return Pointer to the event descriptor, or NULL if allocation failed
 */
lv_event_dsc_t * lv_gltf_model_node_add_event_cb(lv_gltf_model_node_t * node, lv_event_cb_t cb,
                                                 lv_event_code_t filter_list,
                                                 void * user_data);

/**
 * @brief Add an event callback to a glTF model node with world position computation enabled. Use this only when world position is needed, as computing it is an expensive operation.
 *
 * @param node Pointer to the glTF model node structure
 * @param cb The event callback function to add. Use lv_event_get_param() to retrieve lv_gltf_node_data_t with the node's data.
 * @param filter_list Event code filter for the callback
 * @param user_data User data to pass to the callback
 * @return Pointer to the event descriptor, or NULL if allocation failed
 */
lv_event_dsc_t * lv_gltf_model_node_add_event_cb_with_world_position(lv_gltf_model_node_t * node, lv_event_cb_t cb,
                                                                     lv_event_code_t filter_list,
                                                                     void * user_data);
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
 * @brief Set the X position of a glTF model node. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param x The X position value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_position_x(lv_gltf_model_node_t * node, float x);

/**
 * @brief Set the Y position of a glTF model node. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param y The Y position value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_position_y(lv_gltf_model_node_t * node, float y);

/**
 * @brief Set the Z position of a glTF model node. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param z The Z position value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_position_z(lv_gltf_model_node_t * node, float z);

/**
 * @brief Set the X component of a glTF model node's rotation quaternion. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param x The X rotation component value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_rotation_x(lv_gltf_model_node_t * node, float x);

/**
 * @brief Set the Y component of a glTF model node's rotation quaternion. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param y The Y rotation component value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_rotation_y(lv_gltf_model_node_t * node, float y);

/**
 * @brief Set the Z component of a glTF model node's rotation quaternion. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param z The Z rotation component value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_rotation_z(lv_gltf_model_node_t * node, float z);

/**
 * @brief Set the X scale of a glTF model node. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param x The X scale value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_scale_x(lv_gltf_model_node_t * node, float x);

/**
 * @brief Set the Y scale of a glTF model node. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param y The Y scale value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_scale_y(lv_gltf_model_node_t * node, float y);

/**
 * @brief Set the Z scale of a glTF model node. The operation is queued and applied on the next rendering phase.
 *
 * @param node Pointer to the glTF model node structure
 * @param z The Z scale value
 * @return LV_RESULT_OK if the operation is queued successfully, LV_RESULT_INVALID if node is null or no more memory to queue the operation
 */
lv_result_t lv_gltf_model_node_set_scale_z(lv_gltf_model_node_t * node, float z);

/**
 * @brief Get the local position of a glTF model node. Must be called from within an LV_EVENT_VALUE_CHANGED callback.
 *
 * Local position is relative to the node's parent.
 *
 * This function is only valid when called from an event callback registered.
 * See `lv_gltf_model_node_add_event_cb()` and `lv_gltf_model_node_add_event_cb_with_world_position()`
 *
 * @param e Pointer to the event structure from the callback
 * @param result Pointer to lv_3dpoint_t structure to store the position (x, y, z)
 * @return LV_RESULT_OK if successful, LV_RESULT_INVALID if called outside event callback or if parameters are null
 */
lv_result_t lv_gltf_model_node_get_local_position(lv_event_t * e, lv_3dpoint_t * result);

/**
 * @brief Get the world position of a glTF model node. Must be called from within an LV_EVENT_VALUE_CHANGED callback
 * registered with world position enabled.
 *
 * World position is the absolute position in global scene coordinates.
 *
 * This function requires the event callback to be registered with lv_gltf_model_node_add_event_cb_with_world_position()
 * as it involves complex matrix calculations that are computed on-demand.
 *
 * @param e Pointer to the event structure from the callback
 * @param result Pointer to lv_3dpoint_t structure to store the position (x, y, z)
 * @return LV_RESULT_OK if successful, LV_RESULT_INVALID if called outside event callback, world position not enabled, or if parameters are null
 */
lv_result_t lv_gltf_model_node_get_world_position(lv_event_t * e, lv_3dpoint_t * result);

/**
 * @brief Get the scale of a glTF model node. Must be called from within an LV_EVENT_VALUE_CHANGED callback.
 *
 * Returns the scale factors for each axis.
 *
 * This function is only valid when called from an event callback registered.
 * See `lv_gltf_model_node_add_event_cb()` and `lv_gltf_model_node_add_event_cb_with_world_position()`
 *
 * @param e Pointer to the event structure from the callback
 * @param result Pointer to lv_3dpoint_t structure to store the scale (x, y, z)
 * @return LV_RESULT_OK if successful, LV_RESULT_INVALID if called outside event callback or if parameters are null
 */
lv_result_t lv_gltf_model_node_get_scale(lv_event_t * e, lv_3dpoint_t * result);

/**
 * @brief Get the Euler rotation of a glTF model node. Must be called from within an LV_EVENT_VALUE_CHANGED callback.
 *
 * Returns rotation as Euler angles in radians (x, y, z).
 *
 * This function is only valid when called from an event callback registered.
 * See `lv_gltf_model_node_add_event_cb()` and `lv_gltf_model_node_add_event_cb_with_world_position()`
 *
 * @param e Pointer to the event structure from the callback
 * @param result Pointer to lv_3dpoint_t structure to store the rotation in radians (x, y, z)
 * @return LV_RESULT_OK if successful, LV_RESULT_INVALID if called outside event callback or if parameters are null
 */
lv_result_t lv_gltf_model_node_get_euler_rotation(lv_event_t * e, lv_3dpoint_t * result);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTF_MODEL_NODE_H*/
