
#include "lv_gltf_data_internal.hpp"

#if LV_USE_GLTF
#include "../../../drivers/opengles/lv_opengles_private.h"

#include <lvgl.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

#include "lv_gltf_bind.h"

static uint32_t bind_count = 0;

void lv_gltf_bind_set(lv_gltf_bind_t * bind, uint8_t channel, float data)
{
    LV_ASSERT_NULL(bind);
    LV_ASSERT(channel < LV_GLTF_BIND_MAX_CHANNELS);

    if(data != bind->data[channel]) {
        bind->data[channel] = data;
        bind->dirty = true;
    }
}

float lv_gltf_bind_get(lv_gltf_bind_t * bind, uint8_t channel)
{
    LV_ASSERT_NULL(bind);
    LV_ASSERT(channel < LV_GLTF_BIND_MAX_CHANNELS);
    return bind->data[channel];
}

void lv_gltf_bind_bind_clean(lv_gltf_bind_t * bind)
{
    bind->dirty = false;
}

lv_gltf_bind_t * add_by_node(lv_gltf_model_t * gltf_data, fastgltf::Node * node, lv_gltf_bind_prop_t which_prop,
                             uint32_t data_mask,
                             lv_gltf_bind_dir_t dir)
{
    if(node == nullptr) {
        return nullptr;
    }

    lv_gltf_bind_t new_bind;
    new_bind.id = bind_count++;
    new_bind.prop = which_prop;
    new_bind.data_mask = data_mask;
    new_bind.data[0] = new_bind.data[1] = new_bind.data[2] = which_prop == LV_GLTF_BIND_PROP_SCALE ? 1.f : 0.f;
    new_bind.dir = dir;
    new_bind.dirty = true;
    new_bind.next_bind = nullptr;

    // Check if an bind already exists for this node
    if(gltf_data->node_binds.find(node) != gltf_data->node_binds.end()) {
        // Get the existing bind
        lv_gltf_bind_t * existingbind = gltf_data->node_binds[node];

        // Traverse to the end of the linked list of binds
        while(existingbind->next_bind != nullptr)
            existingbind = existingbind->next_bind;

        lv_array_push_back(&gltf_data->binds, &new_bind);
        existingbind->next_bind  = (lv_gltf_bind_t *)lv_array_at(&gltf_data->binds, lv_array_size(&gltf_data->binds) - 1);
        return existingbind->next_bind;
    }
    else {
        // No existing bind, insert the new one
        lv_array_push_back(&gltf_data->binds, &new_bind);
        gltf_data->node_binds[node] = (lv_gltf_bind_t *)lv_array_at(&gltf_data->binds, lv_array_size(&gltf_data->binds) - 1);
        return gltf_data->node_binds[node];
    }
    return nullptr;
}

lv_gltf_bind_t * lv_gltf_bind_add_by_index(lv_gltf_model_t * data, size_t index, lv_gltf_bind_prop_t which_prop,
                                           uint32_t data_mask,
                                           lv_gltf_bind_dir_t dir)
{
    lv_gltf_data_node_t * node = lv_gltf_data_node_get_by_index(data, index);
    if(!node) {
        return nullptr;
    }

    return add_by_node(data, node->node, which_prop, data_mask, dir);
}

lv_gltf_bind_t * lv_gltf_bind_add_by_ip(lv_gltf_model_t * data, const char * ip, lv_gltf_bind_prop_t which_prop,
                                        uint32_t data_mask,
                                        lv_gltf_bind_dir_t dir)
{
    lv_gltf_data_node_t * node = lv_gltf_data_node_get_by_ip(data, ip);
    if(!node) {
        return nullptr;
    }
    return add_by_node(data, node->node, which_prop, data_mask, dir);
}

lv_gltf_bind_t * lv_gltf_bind_add_by_path(lv_gltf_model_t * data, const char * path, lv_gltf_bind_prop_t which_prop,
                                          uint32_t data_mask, lv_gltf_bind_dir_t dir)
{
    lv_gltf_data_node_t * node = lv_gltf_data_node_get_by_path(data, path);

    if(!node) {
        return nullptr;
    }

    return add_by_node(data, node->node, which_prop, data_mask, dir);
}

lv_result_t lv_gltf_bind_remove(lv_gltf_model_t * gltf_data, lv_gltf_bind_t * bind_to_remove)
{
    for(auto pair : gltf_data->node_binds) {
        lv_gltf_bind_t * currentbind = pair.second;
        lv_gltf_bind_t * previousbind = nullptr;

        while(currentbind != nullptr) {
            if(currentbind->id == bind_to_remove->id) {
                // Found the bind to remove
                if(previousbind != nullptr) {
                    // Link the previous bind to the next one
                    previousbind->next_bind = currentbind->next_bind;
                }
                else {
                    gltf_data->node_binds[pair.first] = currentbind->next_bind;
                }

                for(uint32_t i = 0; i < lv_array_size(&gltf_data->binds); ++i) {
                    const lv_gltf_bind_t * current_entry = (const lv_gltf_bind_t *)lv_array_at(&gltf_data->binds, i);
                    if(current_entry->id == bind_to_remove->id) {
                        lv_array_remove(&gltf_data->binds, i);
                    }
                }
                return LV_RESULT_OK;
            }
            previousbind = currentbind;
            if(currentbind != nullptr) {
                if(currentbind->next_bind != nullptr) {
                    currentbind = currentbind->next_bind;
                }
                else {
                    currentbind = nullptr;
                }
            }
        }
    }
    return LV_RESULT_INVALID;
}

#endif /*LV_USE_GLTF*/
