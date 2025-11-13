/**
 * @file lv_gltf_model_node.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"
#if LV_USE_GLTF
#include "fastgltf/types.hpp"
#include "lv_gltf_model.h"
#include "../../../misc/lv_array.h"
#include "../../../misc/lv_assert.h"
#include "../../../misc/lv_types.h"
#include "../../../stdlib/lv_string.h"
#include "../../../stdlib/lv_sprintf.h"

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

void lv_gltf_model_nodes_init(lv_gltf_model_t * model, size_t size)
{
    lv_array_init(&model->nodes, size, sizeof(lv_gltf_model_node_t));
}

void lv_gltf_model_node_init(lv_gltf_model_node_t * node, fastgltf::Node * fastgltf_node, const char * path,
                             const char * ip)
{
    LV_ASSERT_NULL(node);
    node->fastgltf_node = fastgltf_node;
    node->path = lv_strdup(path);
    node->ip = lv_strdup(ip);

    LV_ASSERT_MALLOC(path);
    LV_ASSERT_MALLOC(ip);
}

void lv_gltf_model_node_add(lv_gltf_model_t * model, const lv_gltf_model_node_t * model_node)
{
    lv_array_push_back(&model->nodes, model_node);
}

lv_gltf_model_node_t * lv_gltf_model_node_get_by_index(lv_gltf_model_t * model, size_t index)
{
    if(!model) {
        LV_LOG_WARN("Failed to get node from NULL model");
        return nullptr;
    }

    const uint32_t count = lv_array_size(&model->nodes);
    if(index >= count) {
        LV_LOG_WARN("Invalid index. Max should be %" LV_PRIu32, count - 1);
        return nullptr;
    }
    return (lv_gltf_model_node_t *)lv_array_at(&model->nodes, index);
}

lv_gltf_model_node_t * lv_gltf_model_node_get_by_ip(lv_gltf_model_t * model, const char * ip)
{
    for(size_t i = 0; i < model->nodes.size; ++i) {
        lv_gltf_model_node_t * entry = (lv_gltf_model_node_t *) lv_array_at(&model->nodes, i);
        if(lv_streq(entry->ip, ip)) {
            return entry;
        }
    }
    return nullptr;
}

lv_gltf_model_node_t * lv_gltf_model_node_get_by_path(lv_gltf_model_t * model, const char * path)
{

    for(size_t i = 0; i < model->nodes.size; ++i) {
        lv_gltf_model_node_t * entry = (lv_gltf_model_node_t *) lv_array_at(&model->nodes, i);
        if(lv_streq(entry->path, path)) {
            return entry;
        }
    }
    return nullptr;
}

const char * lv_gltf_model_node_get_path(lv_gltf_model_node_t * node)
{
    if(!node) {
        LV_LOG_WARN("Can't get the path of a null node");
        return NULL;
    }
    return node->path;
}

const char * lv_gltf_model_node_get_ip(lv_gltf_model_node_t * node)
{

    if(!node) {
        LV_LOG_WARN("Can't get the ip of a null node");
        return NULL;
    }
    return node->ip;
}

size_t lv_gltf_model_node_get_count(lv_gltf_model_t * model)
{
    if(!model) {
        return 0;
    }
    return lv_array_size(&model->nodes);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GLTF*/
