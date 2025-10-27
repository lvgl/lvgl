#ifndef LV_GLTF_BIND_H
#define LV_GLTF_BIND_H

#include "lv_gltf_model.h"

#if LV_USE_GLTF

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LV_GLTF_BIND_PROP_VISIBILITY,
    LV_GLTF_BIND_PROP_POSITION,
    LV_GLTF_BIND_PROP_ROTATION,
    LV_GLTF_BIND_PROP_SCALE,
    LV_GLTF_BIND_PROP_BASE_COLOR,
    LV_GLTF_BIND_PROP_ALPHA_FACTOR,
    LV_GLTF_BIND_PROP_EMIS_COLOR,
    LV_GLTF_BIND_PROP_WORLD_POSITION
} lv_gltf_bind_prop_t;

#define LV_GLTF_BIND_MAX_CHANNELS 4
#define LV_GLTF_BIND_CHANNEL_0    (0x01)
#define LV_GLTF_BIND_CHANNEL_1    (0x02)
#define LV_GLTF_BIND_CHANNEL_2    (0x04)
#define LV_GLTF_BIND_CHANNEL_3    (0x08)

typedef enum {
    LV_GLTF_BIND_DIR_READ,
    LV_GLTF_BIND_DIR_WRITE
} lv_gltf_bind_dir_t;

struct _lv_gltf_bind {
    lv_gltf_model_node_t * node;
    lv_gltf_bind_prop_t prop;
    lv_gltf_bind_dir_t dir;
    uint32_t id;
    uint32_t data_mask;
    float data[LV_GLTF_BIND_MAX_CHANNELS];
    bool dirty;
};

typedef struct _lv_gltf_bind lv_gltf_bind_t;

void lv_gltf_bind_set(lv_gltf_bind_t * bind, uint8_t channel, float data);
float lv_gltf_bind_get(lv_gltf_bind_t * bind, uint8_t channel);

/**
 * @brief Reset the dirty flag for a given bind.
 *
 * @param bind Pointer to the lv_gltf_bind_t to reset the dirty flag for.
 */
void lv_gltf_bind_bind_clean(lv_gltf_bind_t * bind);

lv_gltf_bind_t * lv_gltf_bind_node(lv_gltf_model_t * model, lv_gltf_model_node_t * node,
                                   lv_gltf_bind_prop_t which_prop, uint32_t data_mask,
                                   lv_gltf_bind_dir_t dir);
/**
 * @brief Remove an bind from a GLTF data object.
 *
 * @param gltf_data Pointer to the lv_gltf_data_t object from which the bind will be removed.
 * @param bind The bind to be removed.
 * @param which_prop The property to bind.
 * @param data_mask A mask indicating which data fields to bind.
 */
void lv_gltf_bind_remove(lv_gltf_model_t * model, lv_gltf_bind_t * bind);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_GLTF*/
#endif /*LV_GLTF_BIND_H*/
