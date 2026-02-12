/**
 * @file lv_gltf_model_loader.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_model_loader.h"
#include "lv_gltf_data_internal.h"
#if LV_USE_GLTF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t hash;
    uint32_t id;
} lv_opengl_texture_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline lv_rb_compare_res_t opengl_texture_compare_cb(const lv_opengl_texture_t * lhs,
                                                            const lv_opengl_texture_t * rhs)
{
    return lhs->hash - rhs->hash;
}

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_gltf_model_loader_t * lv_gltf_model_loader_create(void)
{
    lv_gltf_model_loader_t * loader = lv_zalloc(sizeof(*loader));
    if(!loader) {
        return NULL;
    }

    lv_rb_init(&loader->textures_map,
               (lv_rb_compare_t)opengl_texture_compare_cb,
               sizeof(lv_opengl_texture_t));
    return loader;
}

void lv_gltf_model_loader_delete(lv_gltf_model_loader_t * loader)
{
    if(!loader) {
        return;
    }
    lv_rb_destroy(&loader->textures_map);
    lv_free(loader);
}

void lv_gltf_model_loader_store_texture(lv_gltf_model_loader_t * loader, uint32_t texture_hash, uint32_t texture_id)
{
    lv_opengl_texture_t key = { .id = texture_id, .hash = texture_hash };
    lv_rb_node_t * node = lv_rb_insert(&loader->textures_map, &key);
    if(!node) {
        LV_LOG_WARN("Failed to cache texture hash: %d id: %d",
                    texture_hash, texture_id);
        return;
    }
    lv_memcpy(node->data, &key, sizeof(key));
}


uint32_t lv_gltf_model_loader_get_texture(lv_gltf_model_loader_t * loader, uint32_t texture_hash)
{
    lv_opengl_texture_t key = { .hash = texture_hash };
    lv_rb_node_t * node = lv_rb_find(&loader->textures_map, &key);
    if(!node) {
        LV_LOG_INFO("Couldn't find texture with hash %d in cache",
                    texture_hash);
        return GL_NONE;
    }
    return ((lv_opengl_texture_t *)node->data)->id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif /*LV_USE_GLTF*/
