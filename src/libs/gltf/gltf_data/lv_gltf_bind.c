
#include "lv_gltf_data_internal.h"

#if LV_USE_GLTF
#include "../../../drivers/opengles/lv_opengles_private.h"

#include <lvgl.h>

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

lv_gltf_bind_t * lv_gltf_bind_node(lv_gltf_model_t * model, lv_gltf_model_node_t * node,
                                   lv_gltf_bind_prop_t which_prop, uint32_t data_mask,
                                   lv_gltf_bind_dir_t dir)
{
    if(!model || !node) {
        LV_LOG_WARN("Invalid parameters, ensure model and node are not NULL");
        return NULL;
    }

    lv_gltf_model_node_t * model_node = lv_gltf_model_node_get_by_ip(model, lv_gltf_model_node_get_ip(node));

    if(!model_node || model_node != node) {
        LV_LOG_ERROR("Mismatch on the node source and the model");
        return NULL;
    }

    lv_gltf_bind_t new_bind;
    new_bind.id = bind_count++;
    new_bind.prop = which_prop;
    new_bind.data_mask = data_mask;
    new_bind.data[0] = new_bind.data[1] = new_bind.data[2] = which_prop == LV_GLTF_BIND_PROP_SCALE ? 1.f : 0.f;
    new_bind.dir = dir;
    new_bind.dirty = true;
    new_bind.node = node;

    lv_rb_t * model_binds = lv_gltf_model_get_binds(model);
    lv_gltf_node_binds_t * rb_entry = NULL;

    {
        lv_gltf_node_binds_t key = {.node = node};

        lv_rb_node_t * rb_node = lv_rb_find(model_binds, &key);

        if(!rb_node) {
            rb_node = lv_rb_insert(model_binds, &key);
            if(!rb_node) {
                LV_LOG_ERROR("Failed to allocate new bind tree");
                return NULL;
            }
            lv_memcpy(rb_node->data, &key, sizeof(key));
            rb_entry = (lv_gltf_node_binds_t *)rb_node->data;
            lv_array_init(&rb_entry->binds, 1, sizeof(lv_gltf_bind_t));
        }
    }

    lv_result_t res = lv_array_push_back(&rb_entry->binds, &new_bind);
    if(res == LV_RESULT_INVALID) {
        LV_LOG_ERROR("Failed to allocate new bind");
        return NULL;
    }

    return (lv_gltf_bind_t *)lv_array_at(&rb_entry->binds, lv_array_size(&rb_entry->binds) - 1);
}

void lv_gltf_bind_remove(lv_gltf_model_t * model, lv_gltf_bind_t * bind)
{

    lv_rb_t * model_binds = lv_gltf_model_get_binds(model);

    lv_gltf_node_binds_t key = {.node = bind->node};
    lv_rb_node_t * rb_node = lv_rb_find(model_binds, &key);

    if(!rb_node) {
        return;
    }

    lv_gltf_node_binds_t * entry = (lv_gltf_node_binds_t *) rb_node->data;

    const uint32_t n = lv_array_size(&entry->binds);
    for(uint32_t i = 0; i < n; ++i) {
        lv_gltf_bind_t * curr_bind = (lv_gltf_bind_t *)lv_array_at(&entry->binds, i);
        if(curr_bind->id == bind->id) {
            lv_array_remove(&entry->binds, i);
            return;
        }
    }
}


#endif /*LV_USE_GLTF*/
