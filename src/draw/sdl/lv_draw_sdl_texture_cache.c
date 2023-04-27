/**
 * @file lv_draw_sdl_texture_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_draw_sdl_texture_cache.h"

#include "lv_draw_sdl_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    SDL_Texture * texture;
    void * userdata;
    lv_lru_free_t * userdata_free;
    lv_draw_sdl_cache_flag_t flags;
} draw_cache_value_t;

typedef struct {
    lv_sdl_cache_key_magic_t magic;
} temp_texture_key_t;

typedef struct {
    lv_coord_t width, height;
} temp_texture_userdata_t;

static void draw_cache_free_value(draw_cache_value_t *);

static draw_cache_value_t * draw_cache_get_entry(lv_draw_sdl_ctx_t * ctx, const void * key, size_t key_length,
                                                 bool * found);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sdl_texture_cache_init(lv_draw_sdl_ctx_t * ctx)
{
    ctx->internals->texture_cache = lv_lru_create(LV_GPU_SDL_LRU_SIZE, 65536,
                                                  (lv_lru_free_t *) draw_cache_free_value, NULL);
}

void lv_draw_sdl_texture_cache_deinit(lv_draw_sdl_ctx_t * ctx)
{
    lv_lru_del(ctx->internals->texture_cache);
}

SDL_Texture * lv_draw_sdl_texture_cache_get(lv_draw_sdl_ctx_t * ctx, const void * key, size_t key_length, bool * found)
{
    return lv_draw_sdl_texture_cache_get_with_userdata(ctx, key, key_length, found, NULL);
}

SDL_Texture * lv_draw_sdl_texture_cache_get_with_userdata(lv_draw_sdl_ctx_t * ctx, const void * key, size_t key_length,
                                                          bool * found, void ** userdata)
{
    draw_cache_value_t * value = draw_cache_get_entry(ctx, key, key_length, found);
    if(!value) return NULL;
    if(userdata) {
        *userdata = value->userdata;
    }
    return value->texture;
}

bool lv_draw_sdl_texture_cache_put(lv_draw_sdl_ctx_t * ctx, const void * key, size_t key_length, SDL_Texture * texture)
{
    return lv_draw_sdl_texture_cache_put_advanced(ctx, key, key_length, texture, NULL, NULL, 0);
}

bool lv_draw_sdl_texture_cache_put_advanced(lv_draw_sdl_ctx_t * ctx, const void * key, size_t key_length,
                                            SDL_Texture * texture, void * userdata, void userdata_free(void *),
                                            lv_draw_sdl_cache_flag_t flags)
{
    lv_lru_t * lru = ctx->internals->texture_cache;
    draw_cache_value_t * value = SDL_malloc(sizeof(draw_cache_value_t));
    value->texture = texture;
    value->userdata = userdata;
    value->userdata_free = userdata_free;
    value->flags = flags;
    if(!texture) {
        return lv_lru_set(lru, key, key_length, value, 1) == LV_LRU_OK;
    }
    if(flags & LV_DRAW_SDL_CACHE_FLAG_MANAGED) {
        /* Managed texture doesn't count into cache size */
        LV_LOG_INFO("cache texture %p", texture);
        return lv_lru_set(lru, key, key_length, value, 1) == LV_LRU_OK;
    }
    Uint32 format;
    int access, width, height;
    if(SDL_QueryTexture(texture, &format, &access, &width, &height) != 0) {
        return false;
    }
    LV_LOG_INFO("cache texture %p, %d*%d@%dbpp", texture, width, height, SDL_BITSPERPIXEL(format));
    return lv_lru_set(lru, key, key_length, value, width * height * SDL_BITSPERPIXEL(format) / 8) == LV_LRU_OK;
}

lv_draw_sdl_cache_key_head_img_t * lv_draw_sdl_texture_img_key_create(const void * src, int32_t frame_id, size_t * size)
{
    lv_draw_sdl_cache_key_head_img_t header;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&header, 0, sizeof(header));
    header.magic = LV_GPU_CACHE_KEY_MAGIC_IMG;
    header.type = lv_img_src_get_type(src);
    header.frame_id = frame_id;
    void * key;
    size_t key_size;
    if(header.type == LV_IMG_SRC_FILE || header.type == LV_IMG_SRC_SYMBOL) {
        size_t srclen = SDL_strlen(src);
        key_size = sizeof(header) + srclen;
        key = SDL_malloc(key_size);
        SDL_memcpy(key, &header, sizeof(header));
        /*Copy string content as key value*/
        SDL_memcpy(key + sizeof(header), src, srclen);
    }
    else {
        key_size = sizeof(header) + sizeof(void *);
        key = SDL_malloc(key_size);
        SDL_memcpy(key, &header, sizeof(header));
        /*Copy address number as key value*/
        SDL_memcpy(key + sizeof(header), &src, sizeof(void *));
    }
    *size = key_size;
    return (lv_draw_sdl_cache_key_head_img_t *) key;
}

static void draw_cache_free_value(draw_cache_value_t * value)
{
    if(value->texture && !(value->flags & LV_DRAW_SDL_CACHE_FLAG_MANAGED)) {
        LV_LOG_INFO("destroy texture %p", value->texture);
        SDL_DestroyTexture(value->texture);
    }
    if(value->userdata_free) {
        value->userdata_free(value->userdata);
    }
    SDL_free(value);
}

static draw_cache_value_t * draw_cache_get_entry(lv_draw_sdl_ctx_t * ctx, const void * key, size_t key_length,
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

#endif /*LV_USE_GPU_SDL*/

