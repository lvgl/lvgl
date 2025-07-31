/**
 * @file lv_gltf_data_node.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"
#if LV_USE_GLTF
#include "fastgltf/types.hpp"
#include "lv_gltf_model.h"
#include "lv_gltf_data_internal.hpp"
#include "../../../misc/lv_array.h"
#include "../../../misc/lv_assert.h"
#include "../../../misc/lv_types.h"
#include "../../../stdlib/lv_string.h"

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

void lv_gltf_data_nodes_init(lv_gltf_model_t * data, size_t size)
{
    lv_array_init(&data->nodes, size, sizeof(lv_gltf_data_node_t));
}

void lv_gltf_data_node_init(lv_gltf_data_node_t * node, fastgltf::Node * fastgltf_node, const char * path,
                            const char * ip)
{
    LV_ASSERT_NULL(node);
    node->node = fastgltf_node;
    node->path = lv_strdup(path);
    node->ip = lv_strdup(ip);

    LV_ASSERT_MALLOC(path);
    LV_ASSERT_MALLOC(ip);
}

void lv_gltf_data_node_add(lv_gltf_model_t * data, const lv_gltf_data_node_t * data_node)
{
    LV_ASSERT_NULL(data);
    lv_array_push_back(&data->nodes, data_node);
}

lv_gltf_data_node_t * lv_gltf_data_node_get_by_index(lv_gltf_model_t * data, size_t index)
{
    LV_ASSERT_NULL(data);
    if(index >= lv_array_size(&data->nodes)) {
        return nullptr;
    }
    return (lv_gltf_data_node_t *)lv_array_at(&data->nodes, index);
}

lv_gltf_data_node_t * lv_gltf_data_node_get_by_ip(lv_gltf_model_t * data, const char * ip)
{
    for(size_t i = 0; i < data->nodes.size; ++i) {
        lv_gltf_data_node_t * entry = (lv_gltf_data_node_t *) lv_array_at(&data->nodes, i);
        if(lv_streq(entry->ip, ip)) {
            return entry;
        }
    }
    return nullptr;
}

lv_gltf_data_node_t * lv_gltf_data_node_get_by_path(lv_gltf_model_t * data, const char * path)
{

    for(size_t i = 0; i < data->nodes.size; ++i) {
        lv_gltf_data_node_t * entry = (lv_gltf_data_node_t *) lv_array_at(&data->nodes, i);
        if(lv_streq(entry->path, path)) {
            return entry;
        }
    }
    return nullptr;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GLTF*/
