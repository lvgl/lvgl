/**
 * @file lv_draw_gles_texure_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES

#include "lv_draw_gles_texture_cache.h"
#include LV_GPU_GLES_GLAD_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    GLuint texture;
} draw_cache_value_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_cache_free_value(draw_cache_value_t * value);

/**********************
 *  STATIC VARIABLES
 **********************/

static draw_cache_value_t * draw_cache_get_entry(lv_draw_gles_ctx_t * ctx,
                                                 const void * key, size_t key_length,
                                                 bool * found);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_gles_texture_cache_init(lv_draw_gles_ctx_t * ctx)
{
    ctx->internals->texture_cache = lv_lru_create(LV_GPU_GLES_LRU_SIZE,
                                                  sizeof(unsigned int),
                                                  (lv_lru_free_t *) draw_cache_free_value, NULL);
}


void lv_draw_gles_texture_cache_deinit(lv_draw_gles_ctx_t * ctx)
{
    lv_lru_del(ctx->internals->texture_cache);
}

GLuint lv_draw_gles_texture_cache_get(lv_draw_gles_ctx_t * ctx, const void * key, size_t key_length, bool * found)
{
    draw_cache_value_t * value = draw_cache_get_entry(ctx, key, key_length, found);
    if(!value) return 0;
    return value->texture;
}

void lv_draw_gles_texture_cache_put(lv_draw_gles_ctx_t * ctx, const void * key, size_t key_length, GLuint texture)
{

    lv_lru_t * lru = ctx->internals->texture_cache;
    draw_cache_value_t * value = lv_mem_alloc(sizeof(draw_cache_value_t));
    value->texture = texture;
    lv_lru_set(lru, key, key_length, value, sizeof(*value));
}

lv_draw_gles_cache_key_head_img_t * lv_draw_gles_texture_img_key_create(const void * src, int32_t frame_id,
                                                                        size_t * size)
{
    lv_draw_gles_cache_key_head_img_t header;
    lv_memset_00(&header, sizeof(header));
    header.magic = LV_GPU_CACHE_KEY_MAGIC_IMG;
    header.type = lv_img_src_get_type(src);
    header.frame_id = frame_id;

    void * key;
    size_t key_size;
    if(header.type == LV_IMG_SRC_FILE || header.type == LV_IMG_SRC_SYMBOL) {
        size_t srclen = strlen(src);
        key_size = sizeof(header) + srclen;
        key = lv_mem_alloc(key_size);
        lv_memcpy(key, &header, sizeof(header));
        /*Copy string content as key value*/
        lv_memcpy(key + sizeof(header), src, srclen);
    }
    else {
        key_size = sizeof(header) + sizeof(void *);
        key = lv_mem_alloc(key_size);
        lv_memcpy(key, &header, sizeof(header));
        /*Copy address number as key value*/
        lv_memcpy(key + sizeof(header), &src, sizeof(void *));
    }
    *size = key_size;
    return (lv_draw_gles_cache_key_head_img_t *) key;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_cache_free_value(draw_cache_value_t * value)
{
    glDeleteTextures(1, &value->texture);
}



static draw_cache_value_t * draw_cache_get_entry(lv_draw_gles_ctx_t * ctx,
                                                 const void * key, size_t key_length,
                                                 bool * found)
{
    lv_lru_t * lru = ctx->internals->texture_cache;
    draw_cache_value_t * value = NULL;
    lv_lru_get(lru, key, key_length, (void **) &value);
    if(!value) {
        if(found) {
            *found = false;
        }
        return NULL;
    }
    if(found) {
        *found = true;
    }
    return value;
}


#endif /*LV_USE_GPU_GLES*/
