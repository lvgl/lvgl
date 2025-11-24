/**
 * @file lv_gltf_model_node.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"
#if LV_USE_GLTF

#include <float.h>
#include "fastgltf/types.hpp"
#include "lv_gltf_model.h"
#include "../../../misc/lv_array.h"
#include "../../../misc/lv_assert.h"
#include "../../../misc/lv_types.h"
#include "../../../misc/lv_event_private.h"
#include "../../../stdlib/lv_string.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../core/lv_obj_pos.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

static lv_result_t add_write_op(lv_gltf_model_node_t * node, lv_gltf_node_prop_t prop, uint8_t channel, float value);
static void invalidate_model(lv_gltf_model_t * model);

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

void lv_gltf_model_node_init(lv_gltf_model_t * model, lv_gltf_model_node_t * node, fastgltf::Node * fastgltf_node,
                             const char * path,
                             const char * numeric_path)
{
    LV_ASSERT_NULL(node);
    lv_memset(node, 0, sizeof(*node));
    node->model = model;
    node->fastgltf_node = fastgltf_node;
    node->path = lv_strdup(path);
    node->numeric_path = lv_strdup(numeric_path);
    LV_ASSERT_MALLOC(node->path);
    LV_ASSERT_MALLOC(node->numeric_path);

    lv_array_init(&node->write_ops, 0, sizeof(lv_gltf_write_op_t));
}

void lv_gltf_model_node_deinit(lv_gltf_model_node_t * node)
{
    LV_ASSERT_NULL(node);
    lv_free((void *)node->path);
    lv_free((void *)node->numeric_path);
    lv_array_deinit(&node->write_ops);

    if(node->read_attrs) {
        lv_event_remove_all(&node->read_attrs->event_list);
        lv_free((void *)node->read_attrs);
    }
}

lv_gltf_model_node_t * lv_gltf_model_node_get_by_index(lv_gltf_model_t * model, size_t index)
{
    if(!model) {
        LV_LOG_WARN("Failed to get node from NULL model");
        return nullptr;
    }

    const uint32_t count = lv_array_size(&model->nodes);
    if(index >= count) {
        LV_LOG_WARN("Invalid index %zu. Max should be %" LV_PRIu32, index, count - 1);
        return nullptr;
    }
    return (lv_gltf_model_node_t *)lv_array_at(&model->nodes, index);
}

lv_gltf_model_node_t * lv_gltf_model_node_get_by_numeric_path(lv_gltf_model_t * model, const char * num_path)
{

    const uint32_t node_count = lv_array_size(&model->nodes);
    for(uint32_t i = 0; i < node_count; ++i) {
        lv_gltf_model_node_t * entry = (lv_gltf_model_node_t *) lv_array_at(&model->nodes, i);
        if(lv_streq(entry->numeric_path, num_path)) {
            return entry;
        }
    }
    return nullptr;
}

lv_gltf_model_node_t * lv_gltf_model_node_get_by_path(lv_gltf_model_t * model, const char * path)
{

    if(!model) {
        LV_LOG_WARN("Can't get node from NULL model");
        return nullptr;
    }

    const uint32_t node_count = lv_array_size(&model->nodes);
    for(uint32_t i = 0; i < node_count; ++i) {
        lv_gltf_model_node_t * entry = (lv_gltf_model_node_t *) lv_array_at(&model->nodes, i);
        if(lv_streq(entry->path, path)) {
            return entry;
        }
    }
    return nullptr;
}

lv_gltf_model_node_t * lv_gltf_model_node_get_by_internal_node(lv_gltf_model_t * model,
                                                               const fastgltf::Node * fastgltf_node)
{
    LV_ASSERT_NULL(model);
    const uint32_t node_count = lv_array_size(&model->nodes);
    for(uint32_t i = 0; i < node_count; ++i) {
        lv_gltf_model_node_t * entry = (lv_gltf_model_node_t *) lv_array_at(&model->nodes, i);
        if(entry->fastgltf_node == fastgltf_node) {
            return entry;
        }
    }
    return nullptr;
}

const char * lv_gltf_model_node_get_path(lv_gltf_model_node_t * node)
{
    if(!node) {
        LV_LOG_WARN("Can't get the path of a null node");
        return nullptr;
    }
    return node->path;
}

const char * lv_gltf_model_node_get_ip(lv_gltf_model_node_t * node)
{
    if(!node) {
        LV_LOG_WARN("Can't get the ip of a null node");
        return nullptr;
    }
    return node->numeric_path;
}
void lv_gltf_model_send_new_values(lv_gltf_model_t * model)
{
    LV_ASSERT_NULL(model);
    if(!model->write_ops_flushed) {
        return;
    }
    const uint32_t node_count = lv_array_size(&model->nodes);
    for(uint32_t i = 0; i < node_count; ++i) {
        lv_gltf_model_node_t * node = (lv_gltf_model_node_t *) lv_array_at(&model->nodes, i);
        if(!node->read_attrs || !node->read_attrs->value_changed) {
            continue;
        }
        lv_event_push_and_send(&node->read_attrs->event_list, LV_EVENT_VALUE_CHANGED, node, &node->read_attrs->node_data);
        node->read_attrs->value_changed = false;
    }
    model->write_ops_flushed = false;
}

lv_event_dsc_t * lv_gltf_model_node_add_event_cb(lv_gltf_model_node_t * node, lv_event_cb_t cb,
                                                 lv_event_code_t filter_list,
                                                 void * user_data)
{
    if(!node->read_attrs) {
        node->read_attrs = (lv_gltf_model_node_attr_t *) lv_zalloc(sizeof(*node->read_attrs));
        LV_ASSERT_MALLOC(node->read_attrs);
        if(!node->read_attrs) {
            LV_LOG_WARN("Failed to allocate memory for read attributes");
            return nullptr;
        }
        lv_array_init(&node->read_attrs->event_list.array, 1, sizeof(lv_event_dsc_t *));
    }
    return lv_event_add(&node->read_attrs->event_list, cb, filter_list, user_data);
}

lv_event_dsc_t * lv_gltf_model_node_add_event_cb_with_world_position(lv_gltf_model_node_t * node, lv_event_cb_t cb,
                                                                     lv_event_code_t filter_list,
                                                                     void * user_data)
{
    lv_event_dsc_t * dsc = lv_gltf_model_node_add_event_cb(node, cb, filter_list, user_data);
    if(!dsc) {
        return nullptr;
    }
    LV_ASSERT_NULL(node->read_attrs);
    node->read_attrs->read_world_position = true;
    return dsc;
}

lv_result_t lv_gltf_model_node_set_position_x(lv_gltf_model_node_t * node, float x)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_POSITION, LV_GLTF_NODE_CHANNEL_X, x);
}

lv_result_t lv_gltf_model_node_set_position_y(lv_gltf_model_node_t * node, float y)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_POSITION, LV_GLTF_NODE_CHANNEL_Y, y);
}

lv_result_t lv_gltf_model_node_set_position_z(lv_gltf_model_node_t * node, float z)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_POSITION, LV_GLTF_NODE_CHANNEL_Z, z);
}

lv_result_t lv_gltf_model_node_set_rotation_x(lv_gltf_model_node_t * node, float x)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_ROTATION, LV_GLTF_NODE_CHANNEL_X, x);
}

lv_result_t lv_gltf_model_node_set_rotation_y(lv_gltf_model_node_t * node, float y)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_ROTATION, LV_GLTF_NODE_CHANNEL_Y, y);
}

lv_result_t lv_gltf_model_node_set_rotation_z(lv_gltf_model_node_t * node, float z)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_ROTATION, LV_GLTF_NODE_CHANNEL_Z, z);
}

lv_result_t lv_gltf_model_node_set_scale_x(lv_gltf_model_node_t * node, float x)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_SCALE, LV_GLTF_NODE_CHANNEL_X, x);
}

lv_result_t lv_gltf_model_node_set_scale_y(lv_gltf_model_node_t * node, float y)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_SCALE, LV_GLTF_NODE_CHANNEL_Y, y);
}

lv_result_t lv_gltf_model_node_set_scale_z(lv_gltf_model_node_t * node, float z)
{
    return add_write_op(node, LV_GLTF_NODE_PROP_SCALE, LV_GLTF_NODE_CHANNEL_Z, z);
}

lv_result_t lv_gltf_model_node_get_local_position(lv_event_t * e, lv_3dpoint_t * result)
{
    if(!e || e->code != LV_EVENT_VALUE_CHANGED) {
        LV_LOG_WARN("Invalid event");
        return LV_RESULT_INVALID;
    }
    lv_gltf_model_node_t * node = (lv_gltf_model_node_t *)lv_event_get_target(e);

    if(!node) {
        LV_LOG_WARN("Cannot get property from a NULL node");
        return LV_RESULT_INVALID;
    }
    if(!node->read_attrs) {
        LV_LOG_WARN("No read event set for this node");
        return LV_RESULT_INVALID;
    }
    *result = node->read_attrs->node_data.local_position;
    return LV_RESULT_OK;
}

lv_result_t lv_gltf_model_node_get_world_position(lv_event_t * e, lv_3dpoint_t * result)
{
    if(!e || e->code != LV_EVENT_VALUE_CHANGED) {
        LV_LOG_WARN("Invalid event");
        return LV_RESULT_INVALID;
    }

    lv_gltf_model_node_t * node = (lv_gltf_model_node_t *)lv_event_get_target(e);

    if(!node) {
        LV_LOG_WARN("Cannot get property from a NULL node");
        return LV_RESULT_INVALID;
    }
    if(!node->read_attrs) {
        LV_LOG_WARN("No read event set for this node");
        return LV_RESULT_INVALID;
    }
    if(!node->read_attrs->read_world_position) {
        LV_LOG_WARN("World position is not read by this node");
        return LV_RESULT_INVALID;
    }
    *result = node->read_attrs->node_data.world_position;
    return LV_RESULT_OK;
}

lv_result_t lv_gltf_model_node_get_scale(lv_event_t * e, lv_3dpoint_t * result)
{
    if(!e || e->code != LV_EVENT_VALUE_CHANGED) {
        LV_LOG_WARN("Invalid event");
        return LV_RESULT_INVALID;
    }
    lv_gltf_model_node_t * node = (lv_gltf_model_node_t *)lv_event_get_target(e);
    if(!node) {
        LV_LOG_WARN("Cannot get property from a NULL node");
        return LV_RESULT_INVALID;
    }
    if(!node->read_attrs) {
        LV_LOG_WARN("No read event set for this node");
        return LV_RESULT_INVALID;
    }
    *result = node->read_attrs->node_data.scale;
    return LV_RESULT_OK;
}

lv_result_t lv_gltf_model_node_get_euler_rotation(lv_event_t * e, lv_3dpoint_t * result)
{
    if(!e || e->code != LV_EVENT_VALUE_CHANGED) {
        LV_LOG_WARN("Invalid event");
        return LV_RESULT_INVALID;
    }
    lv_gltf_model_node_t * node = (lv_gltf_model_node_t *)lv_event_get_target(e);
    if(!node) {
        LV_LOG_WARN("Cannot get property from a NULL node");
        return LV_RESULT_INVALID;
    }
    if(!node->read_attrs) {
        LV_LOG_WARN("No read event set for this node");
        return LV_RESULT_INVALID;
    }
    *result = node->read_attrs->node_data.rotation;
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void invalidate_model(lv_gltf_model_t * model)
{
    lv_obj_invalidate((lv_obj_t *)model->viewer);
    model->write_ops_pending = true;
}

static lv_result_t add_write_op(lv_gltf_model_node_t * node, lv_gltf_node_prop_t prop, uint8_t channel, float value)
{
    if(!node) {
        LV_LOG_WARN("Can't queue queue write operation for NULL node");
        return LV_RESULT_INVALID;
    }

    /* Try to find if a write operation for this property + channel combination exists*/
    /* Doing this is ok for now because the array will be of max size 9 (3 properties x 3 channels)
     * In case we start adding more properties we need to look into other approaches*/
    const uint32_t write_ops_count = lv_array_size(&node->write_ops);
    for(uint32_t i = 0; i < write_ops_count; ++i) {
        lv_gltf_write_op_t * write_op = (lv_gltf_write_op_t *)lv_array_at(&node->write_ops, i);
        if(write_op->prop != prop || write_op->channel != channel) {
            continue;
        }
        if(LV_ABS(write_op->value - value) > FLT_EPSILON) {
            write_op->value = value;
            invalidate_model(node->model);
        }
        return LV_RESULT_OK;
    }
    /* Else create a new one */
    lv_gltf_write_op_t write_op = {.prop = prop, .channel = channel, .value = value};
    lv_result_t res = lv_array_push_back(&node->write_ops, &write_op);
    if(res != LV_RESULT_OK) {
        return res;
    }
    invalidate_model(node->model);

    return LV_RESULT_OK;
}

#endif /*LV_USE_GLTF*/
