/**
 * @file lv_cgltf.h
 * LVGL wrapper for cgltf - single-file glTF 2.0 parser
 */

#ifndef LV_CGLTF_H
#define LV_CGLTF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_CGLTF

#include "cgltf.h"
#include "../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Mesh primitive data extracted from cgltf */
typedef struct {
    float * positions;      /**< Vertex positions (3 floats per vertex) */
    float * normals;        /**< Vertex normals (3 floats per vertex) */
    float * texcoords;      /**< Texture coordinates (2 floats per vertex) */
    uint32_t * indices;     /**< Index buffer */
    uint32_t vertex_count;  /**< Number of vertices */
    uint32_t index_count;   /**< Number of indices */
} lv_cgltf_mesh_data_t;

/** Material data extracted from cgltf */
typedef struct {
    float base_color[4];    /**< Base color RGBA */
    float metallic;         /**< Metallic factor */
    float roughness;        /**< Roughness factor */
    const uint8_t * texture_data;  /**< Texture image data (if any) */
    uint32_t texture_width;
    uint32_t texture_height;
    bool has_texture;
} lv_cgltf_material_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize cgltf options with LVGL memory and file callbacks
 * @param options pointer to cgltf_options to initialize
 */
void lv_cgltf_init_options(cgltf_options * options);

/**
 * Parse a glTF file using LVGL file system
 * @param path file path (can use LVGL file system prefixes like "A:", "S:")
 * @return pointer to parsed cgltf_data, or NULL on failure
 */
cgltf_data * lv_cgltf_parse_file(const char * path);

/**
 * Parse glTF data from memory buffer
 * @param data pointer to glTF/GLB data
 * @param size size of data in bytes
 * @return pointer to parsed cgltf_data, or NULL on failure
 */
cgltf_data * lv_cgltf_parse_memory(const void * data, size_t size);

/**
 * Load buffer data for a parsed glTF file
 * @param gltf pointer to cgltf_data
 * @param gltf_path original file path (for resolving relative paths)
 * @return LV_RESULT_OK on success
 */
lv_result_t lv_cgltf_load_buffers(cgltf_data * gltf, const char * gltf_path);

/**
 * Free cgltf_data and all associated resources
 * @param gltf pointer to cgltf_data to free
 */
void lv_cgltf_free(cgltf_data * gltf);

/**
 * Get the number of meshes in the glTF data
 * @param gltf pointer to cgltf_data
 * @return number of meshes
 */
uint32_t lv_cgltf_get_mesh_count(const cgltf_data * gltf);

/**
 * Extract mesh primitive data
 * @param gltf pointer to cgltf_data
 * @param mesh_index index of the mesh
 * @param primitive_index index of the primitive within the mesh
 * @param out_data pointer to output mesh data structure
 * @return LV_RESULT_OK on success
 */
lv_result_t lv_cgltf_get_mesh_data(const cgltf_data * gltf, uint32_t mesh_index,
                                   uint32_t primitive_index, lv_cgltf_mesh_data_t * out_data);

/**
 * Free mesh data allocated by lv_cgltf_get_mesh_data
 * @param data pointer to mesh data to free
 */
void lv_cgltf_free_mesh_data(lv_cgltf_mesh_data_t * data);

/**
 * Get material data for a mesh primitive
 * @param gltf pointer to cgltf_data
 * @param mesh_index index of the mesh
 * @param primitive_index index of the primitive
 * @param out_material pointer to output material structure
 * @return LV_RESULT_OK on success
 */
lv_result_t lv_cgltf_get_material(const cgltf_data * gltf, uint32_t mesh_index,
                                  uint32_t primitive_index, lv_cgltf_material_t * out_material);

/**
 * Get the transformation matrix for a node
 * @param node pointer to cgltf_node
 * @param out_matrix output 4x4 matrix (16 floats, column-major)
 */
void lv_cgltf_get_node_transform(const cgltf_node * node, float * out_matrix);

/**
 * Get the world transformation matrix for a node (includes parent transforms)
 * @param node pointer to cgltf_node
 * @param out_matrix output 4x4 matrix (16 floats, column-major)
 */
void lv_cgltf_get_node_world_transform(const cgltf_node * node, float * out_matrix);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_CGLTF */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CGLTF_H*/
